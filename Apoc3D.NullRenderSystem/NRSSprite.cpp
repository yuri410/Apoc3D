
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NRSSprite.h"
#include "NRSRenderDevice.h"
#include "NRSTexture.h"
#include "NRSObjects.h"
#include "NRSRenderStateManager.h"

#include "apoc3d/Graphics/RenderSystem/Shader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			
			NRSSprite::NRSSprite(NRSRenderDevice* device)
				: Sprite(device), m_device(device)
			{
				{
					const List<VertexElement> elements = 
					{
						VertexElement(0, VEF_Vector4, VEU_PositionTransformed),
						VertexElement(16, VEF_Color, VEU_Color),
						VertexElement(20, VEF_Vector2, VEU_TextureCoordinate, 0)
					};

					m_vtxDecl = new NRSVertexDeclaration(device, elements);
				}
				{
					const List<VertexElement> elements =
					{
						VertexElement(0, VEF_Vector4, VEU_Position),
						VertexElement(16, VEF_Color, VEU_Color),
						VertexElement(20, VEF_Vector2, VEU_TextureCoordinate, 0)
					};

					m_vtxDeclShadable = new NRSVertexDeclaration(device, elements);
				}

				m_quadBuffer = new NRSVertexBuffer(device, (MaxDeferredDraws * 4), m_vtxDecl->GetVertexSize(), (BufferUsageFlags)(BU_Dynamic|BU_WriteOnly));
				m_quadIndices = new NRSIndexBuffer(device, IndexBufferFormat::Bit16, sizeof(uint16) * MaxDeferredDraws * 6, BU_WriteOnly);

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
			}
			NRSSprite::~NRSSprite()
			{
				delete m_vtxDecl;
				delete m_vtxDeclShadable;
				delete m_quadBuffer;
				delete m_quadIndices;
			}
			void NRSSprite::Begin(SpriteSettings settings)
			{
				Sprite::Begin(settings);

				SetRenderState();
			}
			void NRSSprite::End()
			{
				Sprite::End();

				Flush();
				RestoreRenderState();
			}
			void NRSSprite::SetRenderState()
			{
				NativeStateManager* mgr = m_device->getNativeStateManager();

				if ((getSettings() & SPR_RestoreState) == SPR_RestoreState)
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

					if ((getSettings() & SPR_AlphaBlended) == SPR_AlphaBlended)
						mgr->SetAlphaBlend(true, BlendFunction::Add, Blend::SourceAlpha, Blend::InverseSourceAlpha, m_storedState.oldBlendFactor);

					//m_rawDevice->SetStreamSource(0, m_quadBuffer->getD3DBuffer(), 0, sizeof(QuadVertex));
					//m_rawDevice->SetIndices(m_quadIndices->getD3DBuffer());

					if (getSettings() & SPR_AllowShading)
					{
						//m_rawDevice->SetVertexDeclaration(m_vtxDeclShadable->getD3DDecl());
					}
					else
					{
						ShaderSamplerState state = mgr->getPixelSampler(0);
						state.MinFilter = TextureFilter::Linear;
						state.MagFilter = TextureFilter::Linear;
						state.MipFilter = TextureFilter::None;
						state.AddressU = TextureAddressMode::Clamp;
						state.AddressV = TextureAddressMode::Clamp;
						state.MaxMipLevel = 0;
						state.MipMapLODBias = 0;

						mgr->SetPixelSampler(0, state);


						//m_rawDevice->SetVertexDeclaration(m_vtxDecl->getD3DDecl());

						m_device->BindVertexShader(nullptr);
						m_device->BindPixelShader(nullptr);

						//m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
						//m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
						//m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
					}
				}
			}
			void NRSSprite::RestoreRenderState()
			{
				if ((getSettings() & SPR_RestoreState) == SPR_RestoreState)
				{
					NativeStateManager* mgr = m_device->getNativeStateManager();

					if ((getSettings() & SPR_AlphaBlended) == SPR_AlphaBlended)
						mgr->SetAlphaBlend(m_storedState.oldAlphaBlendEnable, m_storedState.oldBlendFunc, m_storedState.oldSrcBlend, m_storedState.oldDstBlend, m_storedState.oldBlendFactor);

					mgr->SetDepth(m_storedState.oldDepthEnabled, mgr->getDepthBufferWriteEnabled());
					mgr->SetCullMode(m_storedState.oldCull);
				}
			}


			void NRSSprite::Flush()
			{
				if (m_deferredDraws.getCount() == 0)
					return;

				char* vtxData = (char*)m_quadBuffer->Lock(0, m_deferredDraws.getCount() * 4 * sizeof(QuadVertex), LOCK_Discard);

				for (int i = 0; i < m_deferredDraws.getCount(); i++)
				{
					memcpy(vtxData, &m_deferredDraws[i].TL, sizeof(QuadVertex) * 4);

					vtxData += sizeof(QuadVertex) * 4;
				}

				m_quadBuffer->Unlock();

				NativeStateManager* mgr = m_device->getNativeStateManager();

				Texture* currentTexture = m_deferredDraws[0].Tex;
				int32 lastIndex = 0;
				bool currentUVExtend = m_deferredDraws[0].IsUVExtended;
				
				mgr->SetTexture(0, static_cast<NRSTexture*>(currentTexture));
				SetUVExtendedState(currentUVExtend);

				for (int i = 0; i < m_deferredDraws.getCount() + 1; i++)
				{
					const DrawEntry* de;

					if (i == m_deferredDraws.getCount())
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
						de = &m_deferredDraws[i];
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
						int32 startVertex = lastIndex * 4;
						int32 dpCount = i - lastIndex; // not including i

						int32 vtxCount = dpCount * 4;

						//m_rawDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, startVertex, vtxCount, startIndex, dpCount * 2);
						m_batchCount++;

						lastIndex = i;

						if (i != m_deferredDraws.getCount())
						{
							if (textureChanged)
								mgr->SetTexture(0, static_cast<NRSTexture*>(currentTexture));

							if (uvModeChanged)
								SetUVExtendedState(currentUVExtend);
						}
					}
				}
				
				m_deferredDraws.Clear();
			}


			void NRSSprite::SetUVExtendedState(bool isExtended)
			{
				NativeStateManager* mgr = m_device->getNativeStateManager();
				if (isExtended)
				{
					ShaderSamplerState state = mgr->getPixelSampler(0);
					state.AddressU = TextureAddressMode::Wrap;
					state.AddressV = TextureAddressMode::Wrap;
					mgr->SetPixelSampler(0, state);
				}
				else
				{
					ShaderSamplerState state = mgr->getPixelSampler(0);
					state.AddressU = TextureAddressMode::Clamp;
					state.AddressV = TextureAddressMode::Clamp;
					mgr->SetPixelSampler(0, state);
				}
			}

			static RectangleF ObtainSrcRect(Texture* texture, const RectangleF* _srcRect)
			{
				RectangleF srcRect;
				if (_srcRect)
					srcRect = *_srcRect;
				else
					srcRect = { 0, 0, static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight()) };

				return srcRect;
			}

			struct CircleParameters
			{
				float xRadius;
				float yRadius;
				RectangleF srcRect;

				float srcRectXRatio;
				float srcRectYRatio;

				int32 segmentCount;
				float angleStep;

				CircleParameters(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect,
					float beginAngle, float endAngle, int32 div, int32 minDiv = 2)
				{
					yRadius = dstRect.Height / 2;
					xRadius = dstRect.Width / 2;

					srcRect = ObtainSrcRect(texture, _srcRect);

					srcRectXRatio = srcRect.Width / dstRect.Width;
					srcRectYRatio = srcRect.Height / dstRect.Height;

					if (div < minDiv) div = minDiv;
					segmentCount = div;
					angleStep = Math::Max(0.0f, endAngle - beginAngle) / segmentCount;
				}
			};



		}
	}
}