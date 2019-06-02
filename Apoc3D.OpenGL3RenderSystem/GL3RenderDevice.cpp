
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#include "GL3RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "GL3Utils.h"

#include "GL3Texture.h"
#include "GL3RenderStateManager.h"
#include "GL3ObjectFactory.h"
#include "GL3RenderTarget.h"
#include "GL3Buffers.h"
#include "GL3Sprite.h"
#include "GL3Shader.h"
#include "GL3VertexDeclaration.h"

#include "GL/GLProgram.h"
#include "GL/GLFramebuffer.h"

#include "apoc3d/Graphics/Camera.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class BasicEffect : public Effect
			{
			public:
				BasicEffect(GL3RenderDevice* device)
					: m_device(device)
				{

				}
				virtual void Setup(Material* mtrl, const RenderOperation* rop, int count)
				{
					//IDirect3DDevice9* dev = m_device->getDevice();
					//if (mtrl->getTexture(0))
					//{
					//	D3D9Texture* tex = static_cast<D3D9Texture*>(mtrl->getTexture(0)->operator->());

					//	dev->SetTexture(0, tex->getBaseTexture());
					//}

					//dev->SetTransform(D3DTS_WORLD, &reinterpret_cast<const D3DMatrix&>(rop->RootTransform));
					//if (RendererEffectParams::CurrentCamera)
					//{
					//	const Matrix& view = RendererEffectParams::CurrentCamera->getViewMatrix();
					//	dev->SetTransform(D3DTS_VIEW, &reinterpret_cast<const D3DMatrix&>(view));

					//	const Matrix& proj = RendererEffectParams::CurrentCamera->getProjMatrix();
					//	dev->SetTransform(D3DTS_PROJECTION, &reinterpret_cast<const D3DMatrix&>(proj));
					//}
					//dev->SetFVF(D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1);
				}

				virtual void BeginPass(int passId)
				{

				}
				virtual void EndPass()
				{

				}

			protected:
				virtual int begin()
				{
					return 1;
				}
				virtual void end()
				{

				}
			private:
				GL3RenderDevice* m_device;
			};

			GL3RenderDevice::GL3RenderDevice(GraphicsDeviceManager* devManager)
				: RenderDevice(L"OpenGL 3.1 RenderSystem")
				, m_devManager(devManager)
			{

			}
			GL3RenderDevice::~GL3RenderDevice()
			{
				if (m_caps)
					delete m_caps;

				if (m_renderTargets)
					delete[] m_renderTargets;
				if (m_nativeState)
					delete m_nativeState;
				if (m_stateManager)
					delete m_stateManager;
				if (m_objectFactory)
					delete m_objectFactory;
				if (m_defaultEffect)
					delete m_defaultEffect;
				//if (m_instancingData)
				//	delete m_instancingData;
			}



			void GL3RenderDevice::Initialize()
			{
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL3]Initializing OpenGL 3.1 Render Device. ", 
					LOGLVL_Infomation);

				m_nativeState = new NativeGL3StateManager(this);
				m_stateManager = new GL3RenderStateManager(this, m_nativeState);
				m_renderStates = m_stateManager;
				m_objectFactory = new GL3ObjectFactory(this);

				m_caps = new GL3Capabilities(this);

				m_renderTargets = new GL3RenderTarget*[m_caps->GetMRTCount()]();

				//dev->GetRenderTarget(0, &m_defaultRT);
				//dev->GetDepthStencilSurface(&m_defaultDS);

				m_defaultEffect = new BasicEffect(this);

				//m_instancingData = new D3D9InstancingData(this);
			}
			
			void GL3RenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
				
			}
			void GL3RenderDevice::EndFrame()
			{
				
			}

			void GL3RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
				GLbitfield field = 0;
				
				if ((flags & CLEAR_ColorBuffer) == CLEAR_ColorBuffer)
				{
					field |= GL_COLOR_BUFFER_BIT;
					

					float r = CV_GetColorR(color) / 255.0f;
					float g = CV_GetColorG(color) / 255.0f;
					float b = CV_GetColorB(color) / 255.0f;
					float a = CV_GetColorA(color) / 255.0f;

					glClearColor(r, g, b, a);
				}
				if (flags & CLEAR_DepthBuffer)
				{
					field |= GL_DEPTH_BUFFER_BIT;

					// Enable buffer for writing if it isn't
					if (!m_nativeState->getDepthBufferWriteEnabled())
					{
						glDepthMask(GL_TRUE);
					}

					glClearDepth(depth);
				}


				if (flags & CLEAR_Stencil)
				{
					field |= GL_STENCIL_BUFFER_BIT;
					// Enable buffer for writing if it isn't
					glStencilMask(0xFFFFFFFF);

					glClearStencil(stencil);
				}

				// Should be enable scissor test due the clear region is
				// relied on scissor box bounds.
				GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
				if (!scissorTestEnabled)
				{
					glEnable(GL_SCISSOR_TEST);
				}

				// Sets the scissor box as same as viewport
				GLint viewport[4], scissor[4];
				glGetIntegerv(GL_VIEWPORT, viewport);
				glGetIntegerv(GL_SCISSOR_BOX, scissor);
				bool scissorBoxDifference =
					viewport[0] != scissor[0] || viewport[1] != scissor[1] ||
					viewport[2] != scissor[2] || viewport[3] != scissor[3];
				if (scissorBoxDifference)
				{
					glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
				}

				assert(field);
				glClear(field);

				// Restore scissor box
				if (scissorBoxDifference)
				{
					glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
				}
				// Restore scissor test
				if (!scissorTestEnabled)
				{
					glDisable(GL_SCISSOR_TEST);
				}

				// Reset buffer write state
				if (!m_nativeState->getDepthBufferWriteEnabled() && (flags & CLEAR_DepthBuffer))
				{
					glDepthMask( GL_FALSE );
				}

				if (flags & CLEAR_Stencil)
				{
					glStencilMask(m_nativeState->getStencilMask());
				}
			}

			void GL3RenderDevice::SetRenderTarget(int32 index, RenderTarget* rt)
			{
				GL3RenderTarget* grt = static_cast<GL3RenderTarget*>(rt);

				if (m_renderTargets[index] != grt)
				{
					m_renderTargets[index] = grt;
					m_renderTargetDirty = true;
				}
			}

			RenderTarget* GL3RenderDevice::GetRenderTarget(int32 index)
			{
				return m_renderTargets[index];
			}

			void GL3RenderDevice::SetDepthStencilBuffer(DepthStencilBuffer* buf)
			{
				GL3DepthStencilBuffer* gdsb = static_cast<GL3DepthStencilBuffer*>(buf);

				if (m_depthStencilBuffer != gdsb)
				{
					m_depthStencilBuffer = gdsb;
					m_renderTargetDirty = true;
				}
			}

			DepthStencilBuffer* GL3RenderDevice::GetDepthStencilBuffer()
			{
				return m_depthStencilBuffer;
			}

			void GL3RenderDevice::BindVertexShader(Shader* shader)
			{
				GL3Shader* gs = static_cast<GL3Shader*>(shader);
				
				if (m_currentVertexShader != gs)
				{
					m_currentVertexShader = gs;
					m_shaderDirty = true;
				}
			}
			void GL3RenderDevice::BindPixelShader(Shader* shader)
			{
				GL3Shader* gs = static_cast<GL3Shader*>(shader);

				if (m_currentPixelShader != gs)
				{
					m_currentPixelShader = gs;
					m_shaderDirty = true;
				}
			}

			void GL3RenderDevice::Render(Material* mtrl, const RenderOperation* op, int count, int passSelID)
			{
				if (!op || count == 0)
					return;

				if (HasBatchReportRequest)
					RenderDevice::Render(mtrl, op, count, passSelID);

				Effect* fx = mtrl->GetPassEffect(passSelID);
				if (!fx)
				{
					return;
				}

				PostBindRenderTargets();

				if (m_nativeState->getAlphaBlendEnable() != mtrl->IsBlendTransparent)
				{
					m_nativeState->setAlphaBlendEnable(mtrl->IsBlendTransparent);
				}
				if (m_nativeState->getAlphaSourceBlend() != mtrl->SourceBlend)
				{
					m_nativeState->setAlphaSourceBlend(mtrl->SourceBlend);
				}
				if (m_nativeState->getAlphaDestinationBlend() != mtrl->DestinationBlend)
				{
					m_nativeState->setAlphaDestinationBlend(mtrl->DestinationBlend);
				}
				if (m_nativeState->getAlphaBlendOperation() != mtrl->BlendFunction)
				{
					m_nativeState->setAlphaBlendOperation(mtrl->BlendFunction);
				}

				if (m_nativeState->getCullMode() != mtrl->Cull)
				{
					m_nativeState->SetCullMode(mtrl->Cull);
				}


				if (m_nativeState->getDepthBufferEnabled() != mtrl->DepthTestEnabled ||
					m_nativeState->getDepthBufferWriteEnabled() != mtrl->DepthWriteEnabled)
				{
					m_nativeState->SetDepth(mtrl->DepthTestEnabled, mtrl->DepthWriteEnabled);
				}

				for (uint32 i = 0; i < 4; i++)
				{
					ColorWriteMasks masks = mtrl->GetTargetWriteMask(i);
					if (m_nativeState->GetColorWriteMasks(i) != masks)
						m_nativeState->SetColorWriteMasks(i, masks);
				}


				int passCount = fx->Begin();
				for (int p = 0; p < passCount; p++)
				{
					fx->BeginPass(p);

					if (fx->SupportsInstancing())
					{
						// here the input render operation list is guaranteed to have the same geometry data,
						// instancing drawing is done here once the effect supports it

						const RenderOperation& fristROP = op[0];
						const GeometryData* firstGM = fristROP.GeometryData;

						m_primitiveCount += firstGM->PrimitiveCount * count;
						m_vertexCount += firstGM->VertexCount * count;

						if (firstGM->usesIndex())
						{
							const GeometryData* gm = firstGM;

							GLProgram* fxProg = PostBindShaders();

							GL3IndexBuffer* ib = static_cast<GL3IndexBuffer*>(gm->IndexBuffer);
							GL3VertexBuffer* vb = static_cast<GL3VertexBuffer*>(gm->VertexBuffer);
							GL3VertexDeclaration* vdecl = static_cast<GL3VertexDeclaration*>(gm->VertexDecl);

							vdecl->Bind(fxProg, vb);
							ib->Bind();

							GLenum idxType = GLUtils::ConvertIndexBufferFormat(ib->getIndexType());
							GLenum primitiveType = GLUtils::ConvertPrimitiveType(gm->PrimitiveType);

							const int InstancingBatchSize = 50;
							for (int currentIndex = 0; currentIndex < count; currentIndex += InstancingBatchSize)
							{
								const uint32 actualCount = Math::Min(InstancingBatchSize, count - currentIndex);

								fx->Setup(mtrl, op + currentIndex, actualCount);

								glDrawElementsInstancedBaseVertex(primitiveType, gm->PrimitiveCount, idxType, nullptr, actualCount, gm->BaseVertex);

								m_batchCount++;
							}
						}
					}
					for (int j = 0; j < count; j++)
					{
						const RenderOperation& rop = op[j];
						const GeometryData* gm = rop.GeometryData;

						if (gm->VertexCount == 0 || gm->PrimitiveCount == 0)
						{
							fx->EndPass();
							break;
						}

						m_primitiveCount += gm->PrimitiveCount;
						m_vertexCount += gm->VertexCount;
						m_batchCount++;

						// setup effect
						fx->Setup(mtrl, &rop, 1);

						GLenum primitiveType = GLUtils::ConvertPrimitiveType(gm->PrimitiveType);

						GLProgram* fxProg = PostBindShaders();

						GL3VertexBuffer* vb = static_cast<GL3VertexBuffer*>(gm->VertexBuffer);
						GL3VertexDeclaration* vdecl = static_cast<GL3VertexDeclaration*>(gm->VertexDecl);

						vdecl->Bind(fxProg, vb);

						if (gm->usesIndex())
						{
							GL3IndexBuffer* ib = static_cast<GL3IndexBuffer*>(gm->IndexBuffer);
							GLenum idxType = GLUtils::ConvertIndexBufferFormat(ib->getIndexType());

							ib->Bind();

							glDrawElementsBaseVertex(primitiveType, gm->PrimitiveCount, idxType, nullptr, gm->BaseVertex);
						}
						else
						{
							glDrawArrays(primitiveType, gm->BaseVertex, gm->VertexCount);
						}
					}
					fx->EndPass();
				}
				fx->End();
			}

			Viewport GL3RenderDevice::getViewport()
			{
				GLint viewport[4];
				glGetIntegerv(GL_VIEWPORT, viewport);

				Viewport vp(viewport[0], viewport[1], viewport[2], viewport[3], 0, 1);
				return vp;
			}
			void GL3RenderDevice::setViewport(const Viewport& vp)
			{
				glViewport(vp.X, vp.Y, vp.Width, vp.Height);
			}
			Capabilities* GL3RenderDevice::getCapabilities() const
			{
				return m_caps; 
			}
			uint32 GL3RenderDevice::GetAvailableVideoRamInMB()
			{
				return 0;
			}
			PixelFormat GL3RenderDevice::GetDefaultRTFormat()
			{
				return FMT_A8R8G8B8;
			}
			DepthFormat GL3RenderDevice::GetDefaultDepthStencilFormat()
			{
				return DEPFMT_Depth24Stencil8;
			}

			GLProgram* GL3RenderDevice::PostBindShaders()
			{
				// flush deferred pipeline states
				GLProgram* fxProg = nullptr;

				if (m_shaderDirty)
				{
					m_shaderDirty = false;

					if (m_currentVertexShader && m_currentPixelShader)
					{
						assert(m_currentVertexShader->getGLProgram() == m_currentPixelShader->getGLProgram());
					}
				}

				if (m_currentVertexShader && m_currentPixelShader)
				{
					fxProg = m_currentVertexShader->getGLProgram();;
				}

				if (fxProg)
					fxProg->Bind();

				return fxProg;
			}

			void GL3RenderDevice::PostBindRenderTargets()
			{
				
			}

			/************************************************************************/
			/* GL3Capabilities                                                      */
			/************************************************************************/

			GL3Capabilities::GL3Capabilities(GL3RenderDevice* device)
				: m_device(device)
			{
				glGetIntegerv(GL_MAJOR_VERSION, &m_majorGlVer);
				glGetIntegerv(GL_MINOR_VERSION, &m_minorGlVer);

				m_majorGlslVer = m_minorGlslVer = 0;
				if (m_majorGlVer == 1)
				{
					m_majorGlslVer = 1;
					m_minorGlslVer = 0;
				}
				if (m_majorGlVer == 2)
				{
					if (m_minorGlVer == 0)
					{
						m_majorGlslVer = 1;
						m_minorGlslVer = 1;
					}
					else
					{
						m_majorGlslVer = 1;
						m_minorGlslVer = 2;
					}
				}
				else if (m_majorGlVer == 3 && m_minorGlVer < 3)
				{
					if (m_minorGlVer == 0)
					{
						m_majorGlslVer = 1;
						m_minorGlslVer = 3;
					}
					else if (m_minorGlVer == 1)
					{
						m_majorGlslVer = 1;
						m_minorGlslVer = 4;
					}
					else if (m_minorGlVer == 2)
					{
						m_majorGlslVer = 1;
						m_minorGlslVer = 5;
					}
				}
				else
				{
					m_majorGlslVer = m_majorGlVer;
					m_minorGlslVer = m_minorGlVer;
				}
			}

			bool GL3Capabilities::SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				//GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
				//IDirect3D9* d3d9 = devMgr->getDirect3D();
				//const DeviceSettings* setting = devMgr->getCurrentSetting();

				//if (multisampleCount)
				//{
				//	if (depthFormat == DEPFMT_Count)
				//	{
				//		DWORD quality;
				//		HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
				//			D3D9Utils::ConvertPixelFormat(pixFormat),
				//			setting->D3D9.PresentParameters.Windowed, 
				//			D3D9Utils::ConvertMultisample(multisampleCount),
				//			&quality);

				//		return hr == S_OK;
				//	}
				//	else
				//	{
				//		DWORD quality;
				//		HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
				//			D3D9Utils::ConvertPixelFormat(pixFormat),
				//			setting->D3D9.PresentParameters.Windowed, 
				//			D3D9Utils::ConvertMultisample(multisampleCount),
				//			&quality);

				//		if (FAILED(hr))
				//			return false;
				//		
				//		hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
				//			D3D9Utils::ConvertDepthFormat(depthFormat),
				//			setting->D3D9.PresentParameters.Windowed, 
				//			D3D9Utils::ConvertMultisample(multisampleCount),
				//			&quality);

				//		return hr == S_OK;
				//	}
				//}
				//else
				//{
				//	if (depthFormat == DEPFMT_Count)
				//	{
				//		HRESULT hr = d3d9->CheckDeviceFormat(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat, 
				//			D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
				//		return hr == S_OK;
				//	}
				//	else
				//	{
				//		HRESULT hr = d3d9->CheckDeviceFormat( setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat,
				//			D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3D9Utils::ConvertDepthFormat(depthFormat));

				//		if (FAILED(hr))
				//			return false;

				//		hr = d3d9->CheckDeviceFormat(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat, 
				//			D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
				//		return hr == S_OK;
				//	}
				//	
				//}
				return true;
			}

			bool GL3Capabilities::SupportsPixelShader(const char* implType, int majorVer, int minorVer)
			{
				switch (m_majorGlslVer)
				{
					case 1:
					{
						if (m_minorGlslVer >= 4)
							return majorVer <= 3; // GLSL 1.5 == SM3.0 at least
						else if (m_minorGlslVer <= 3)
							return majorVer <= 2;

					}
					break;
					case 3:
					{
						if (m_minorGlslVer >= 3)
							return majorVer <= 4; // GLSL 3.3 = SM4.0, minorVer ignored
						else
							return majorVer <= 3;
					}
					break;
					case 4:
					{
						if (m_minorGlslVer >= 1)
							return majorVer <= 5; // GLSL 4.1 = SM5.0, minorVer ignored
						else
							return majorVer <= 4;
					}
					break;
				}
				return false;
			}

			void GL3Capabilities::EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes)
			{

			}

			const String* GL3Capabilities::FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				return nullptr;
			}

			bool GL3Capabilities::SupportsVertexShader(const char* implType, int majorVer, int minorVer)
			{
				return SupportsPixelShader(implType, majorVer, minorVer);
			}

			int GL3Capabilities::GetMRTCount()
			{
				GLint maxbuffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
				return maxbuffers;
			}

			bool GL3Capabilities::SupportsMRTDifferentBits()
			{
				return true;
			}

			bool GL3Capabilities::SupportsMRTWriteMasks()
			{
				return true;
			}
		}
	}
}