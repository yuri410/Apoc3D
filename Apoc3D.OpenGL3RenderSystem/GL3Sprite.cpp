/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "GL3Sprite.h"
#include "GL3RenderDevice.h"
#include "GL3Texture.h"
#include "GL3RenderStateManager.h"
#include "GL3VertexDeclaration.h"
#include "GL3Buffers.h"
#include "GL3Utils.h"
#include "GL3ObjectFactory.h"
#include "GL3Shader.h"
#include "Apoc3D/IOLib/IOUtils.h"

//#include "Apoc3D/Graphics/RenderSystem/HardwareBuffer.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			char* CreateCodeBlob(const char* src)
			{
				int32 srcLen = (int32)strlen(src);
				char* r = new char[srcLen + sizeof(int) * 2];
				i32_mb_le(srcLen, r);
				i32_mb_le(srcLen, r + sizeof(int));
				memcpy(r + sizeof(int) * 2, src, srcLen);
				return r;
			}

			GL3Sprite::GL3Sprite(GL3RenderDevice* device)
				: Sprite(device), m_device(device)
			{
				m_vtxDecl = new GL3VertexDeclaration(
				{
					VertexElement(0, VEF_Vector4, VEU_Position),
					VertexElement(16, VEF_Color, VEU_Color),
					VertexElement(20, VEF_Vector2, VEU_TextureCoordinate, 0)
				});
				
				m_quadBuffer = new GL3VertexBuffer(device, (MaxDeferredDraws * 4), m_vtxDecl->GetVertexSize(), (BufferUsageFlags)(BU_Dynamic | BU_WriteOnly));
				m_quadIndices = new GL3IndexBuffer(device, IndexBufferFormat::Bit16, sizeof(uint16) * MaxDeferredDraws * 6, BU_WriteOnly);

				{
					uint16* indices = (uint16*)m_quadIndices->Lock(LOCK_None);

					for (uint16 i = 0; i < (uint16)MaxDeferredDraws; i++)
					{
						uint16 base = i * 4;

						*indices++ = base + 0;
						*indices++ = base + 1;
						*indices++ = base + 2;

						*indices++ = base + 2;
						*indices++ = base + 1;
						*indices++ = base + 3;
					}

					m_quadIndices->Unlock();
				}

				m_storedState.oldBlendFactor = 0;

				GL3ObjectFactory* objFac = static_cast<GL3ObjectFactory*>(m_device->getObjectFactory());

				const char* vsCode = "#version 150\nin vec2 a_Position; void main() { gl_Position = vec4( a_Position, 0.0, 1.0 ); }";
				const char* psCode = "#version 150\nout vec4 outColor; void main() { outColor = vec4( 1.0, 0.0, 0.0, 1.0 ); }";

				char* vsBlob = CreateCodeBlob(vsCode);
				char* psBlob = CreateCodeBlob(psCode);

				m_vertexShader = static_cast<GL3VertexShader*>(objFac->CreateVertexShader((const byte*)vsBlob));
				m_pixelShader = static_cast<GL3PixelShader*>(objFac->CreatePixelShader((const byte*)psBlob));

				delete[] vsBlob;
				delete[] psBlob;

				m_vertexShader->NotifyLinkage({ m_vertexShader, m_pixelShader });
			}
			GL3Sprite::~GL3Sprite()
			{
				delete m_vtxDecl;
				delete m_quadBuffer;
				delete m_quadIndices;
			}
			void GL3Sprite::Begin(SpriteSettings settings)
			{
				Sprite::Begin(settings);

				SetRenderState();
			}
			void GL3Sprite::End()
			{
				Sprite::End();

				RestoreRenderState();
			}
			
			void GL3Sprite::Submit(const SpriteDrawEntries& batch)
			{
				if (batch.getCount() == 0)
					return;

				for (auto& entries : batch.getGroupAccessor<MaxDeferredDraws>())
				{
					const int32 entryCount = entries.getCount();

					char* vtxData = (char*)m_quadBuffer->Lock(0, entryCount * 4 * sizeof(QuadVertex), LOCK_Discard);

					for (int i = 0; i < entryCount; i++)
					{
						memcpy(vtxData, &entries[i].TL, sizeof(QuadVertex) * 4);

						vtxData += sizeof(QuadVertex) * 4;
					}

					m_quadBuffer->Unlock();


					NativeGL3StateManager* mgr = m_device->getNativeState();

					Texture* currentTexture = entries[0].Tex;
					int32 lastIndex = 0;
					bool currentUVExtend = entries[0].IsUVExtended;

					mgr->SetTexture(0, static_cast<GL3Texture*>(currentTexture));
					SetUVExtendedState(currentUVExtend);

					for (int i = 0; i < entryCount + 1; i++)
					{
						const DrawEntry* de;

						if (i == entryCount)
						{
							// last entry, make a dummy next DrawEntry to force state change
							// then the previous entries will be drawn
							static DrawEntry dummy;
							dummy.Tex = nullptr;
							dummy.IsUVExtended = !currentUVExtend;
							de = &dummy;
						}
						else
						{
							de = &entries[i];
						}

						bool textureChanged = false;
						bool uvModeChanged = false;

						if (de->Tex != currentTexture)
						{
							currentTexture = de->Tex;
							textureChanged = true;
						}

						if (de->IsUVExtended != currentUVExtend)
						{
							currentUVExtend = de->IsUVExtended;
							uvModeChanged = true;
						}

						if (textureChanged || uvModeChanged)
						{
							int32 startIndex = lastIndex * 6;
							//int32 startVertex = lastIndex * 4;
							int32 quadCount = i - lastIndex; // not including i

							int32 indexOffset = startIndex * m_quadIndices->getIndexElementSize();

							glDrawElements(GL_TRIANGLES, quadCount * 2, GLUtils::ConvertIndexBufferFormat(m_quadIndices->getIndexType()), (const GLvoid*)indexOffset);

							m_batchCount++;

							lastIndex = i;

							if (i != entryCount)
							{
								if (textureChanged)
									mgr->SetTexture(0, static_cast<GL3Texture*>(currentTexture));

								if (uvModeChanged)
									SetUVExtendedState(currentUVExtend);
							}
						}
					}

				}
			}

			void GL3Sprite::SetRenderState()
			{
				NativeGL3StateManager* mgr = m_device->getNativeState();

				if (getSettings() & SPR_RestoreState)
				{
					m_storedState.oldAlphaBlendEnable = mgr->getAlphaBlendEnable();
					m_storedState.oldBlendFunc = mgr->getAlphaBlendOperation();
					m_storedState.oldSrcBlend = mgr->getAlphaSourceBlend();
					m_storedState.oldDstBlend = mgr->getAlphaDestinationBlend();
					m_storedState.oldBlendFactor = mgr->getAlphaBlendFactor();
					m_storedState.oldDepthEnabled = mgr->getDepthBufferEnabled();
					m_storedState.oldCull = mgr->getCullMode();
				}

				if (getSettings() & SPR_ChangeState)
				{
					mgr->SetDepth(false, mgr->getDepthBufferWriteEnabled());
					mgr->SetCullMode(CullMode::None);

					if (getSettings() & SPR_AlphaBlended)
						mgr->SetAlphaBlend(true, BlendFunction::Add, Blend::SourceAlpha, Blend::InverseSourceAlpha, m_storedState.oldBlendFactor);

					ShaderSamplerState state = mgr->getSampler(0);
					state.MinFilter = TextureFilter::Linear;
					state.MagFilter = TextureFilter::Linear;
					state.MipFilter = TextureFilter::None;
					state.AddressU = TextureAddressMode::Clamp;
					state.AddressV = TextureAddressMode::Clamp;
					state.MaxMipLevel = 0;
					state.MipMapLODBias = 0;
					mgr->SetSampler(0, state);

					m_device->PostBindRenderTargets();

					m_device->BindVertexShader(m_vertexShader);
					m_device->BindPixelShader(m_pixelShader);

					GLProgram* fxProg = m_device->PostBindShaders();
					assert(fxProg);
					m_vtxDecl->Bind(fxProg, m_quadBuffer);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIndices->getGLBufferID());
				}
			}
			void GL3Sprite::RestoreRenderState()
			{
				if (getSettings() & SPR_RestoreState)
				{
					NativeGL3StateManager* mgr = m_device->getNativeState();

					if (getSettings() & SPR_AlphaBlended)
						mgr->SetAlphaBlend(m_storedState.oldAlphaBlendEnable, m_storedState.oldBlendFunc, m_storedState.oldSrcBlend, m_storedState.oldDstBlend, m_storedState.oldBlendFactor);

					mgr->SetDepth(m_storedState.oldDepthEnabled, mgr->getDepthBufferWriteEnabled());
					mgr->SetCullMode(m_storedState.oldCull);
				}
			}


			void GL3Sprite::SetUVExtendedState(bool isExtended)
			{
				NativeGL3StateManager* mgr = m_device->getNativeState();
				ShaderSamplerState state = mgr->getSampler(0);

				if (isExtended)
				{
					state.AddressU = TextureAddressMode::Wrap;
					state.AddressV = TextureAddressMode::Wrap;
				}
				else
				{
					state.AddressU = TextureAddressMode::Clamp;
					state.AddressV = TextureAddressMode::Clamp;
				}

				mgr->SetSampler(0, state);
			}


		}
	}
}