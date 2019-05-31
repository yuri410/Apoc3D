
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
//#include "GL1InstancingData.h"

#include "GL3Sprite.h"

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
					//IDirect3DDevice9* dev = m_device->getDevice();
					//dev->SetVertexShader(0);
					//dev->SetPixelShader(0);
					return 1;
				}
				virtual void end()
				{

				}
			private:
				GL3RenderDevice* m_device;
			};

			GL3RenderDevice::GL3RenderDevice(GraphicsDeviceManager* devManager)
				: RenderDevice(L"OpenGL 3.1 RenderSystem"), m_devManager(devManager), 
				m_stateManager(0), m_nativeState(0), m_caps(0), m_cachedRenderTarget(0), m_defaultEffect(0)
			{

			}
			GL3RenderDevice::~GL3RenderDevice()
			{
				//m_defaultRT->Release();
				//m_defaultRT = 0;

				//m_defaultDS->Release();
				//m_defaultDS = 0;

				if (m_caps)
					delete m_caps;

				if (m_cachedRenderTarget)
					delete[] m_cachedRenderTarget;
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
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_TEXTURE_1D);
				glEnable(GL_TEXTURE_3D);
				glEnable(GL_TEXTURE_CUBE_MAP);
				//GL_EXT_texture_compression_s3tc
				
				
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL3]Initializing OpenGL 3.1 Render Device. ", 
					LOGLVL_Infomation);

				m_nativeState = new NativeGL3StateManager(this);
				m_stateManager = new GL3RenderStateManager(this, m_nativeState);
				m_renderStates = m_stateManager;
				m_objectFactory = new GL3ObjectFactory(this);


				m_caps = new GL3Capabilities(this);

				m_cachedRenderTarget = new GL3RenderTarget*[m_caps->GetMRTCount()];
				memset(m_cachedRenderTarget, 0, sizeof(GL3RenderTarget*) * m_caps->GetMRTCount());

				//dev->GetRenderTarget(0, &m_defaultRT);
				//dev->GetDepthStencilSurface(&m_defaultDS);

				m_defaultEffect = new BasicEffect(this);

				//m_instancingData = new D3D9InstancingData(this);
			}
			
			void GL3RenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
				//getDevice()->BeginScene();
			}
			void GL3RenderDevice::EndFrame()
			{
				//getDevice()->EndScene();
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
						glDepthMask( GL_TRUE );
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

			void GL3RenderDevice::SetRenderTarget(int index, RenderTarget* rt)
			{
				//D3DDevice* dev = getDevice();
				//if (rt)
				//{
				//	D3D9RenderTarget* oldRt = m_cachedRenderTarget[index];

				//	D3D9RenderTarget* drt = static_cast<D3D9RenderTarget*>(rt);
				//	
				//	dev->SetRenderTarget(index, drt->getColorSurface());
				//	if (drt->getDepthSurface())
				//	{
				//		if (index)
				//		{
				//			throw Apoc3DException::createException(EX_InvalidOperation, L"Render targets with a depth buffer can only be set at index 0.");
				//		}
				//		dev->SetDepthStencilSurface(drt->getDepthSurface());
				//	}

				//	m_cachedRenderTarget[index] = drt;

				//	if (oldRt && oldRt != drt)
				//	{
				//		m_cachedRenderTarget[index]->Resolve();
				//	}
				//}
				//else 
				//{
				//	if (index == 0)
				//	{
				//		dev->SetRenderTarget(0, m_defaultRT);
				//		dev->SetDepthStencilSurface(m_defaultDS);

				//		m_cachedRenderTarget[0] = 0;
				//	}
				//	else
				//	{
				//		dev->SetRenderTarget(index, 0);
				//		m_cachedRenderTarget[index] = 0;
				//	}
				//}
			}

			RenderTarget* GL3RenderDevice::GetRenderTarget(int index)
			{
				return m_cachedRenderTarget[index];
			}

			void GL3RenderDevice::BindVertexShader(Shader* shader)
			{
				//if (shader)
				//{
				//	D3D9VertexShader* vs = static_cast<D3D9VertexShader*>(shader);
				//	getDevice()->SetVertexShader(vs->getD3DVS());
				//}
				//else
				//{
				//	getDevice()->SetVertexShader(0);
				//}
			}
			void GL3RenderDevice::BindPixelShader(Shader* shader)
			{
				//glBindProgramARB(GL_VERTEX_PROGRAM_ARB, );

				//if (shader)
				//{
				//	D3D9PixelShader* ps = static_cast<D3D9PixelShader*>(shader);
				//	getDevice()->SetPixelShader(ps->getD3DPS());
				//}
				//else
				//{
				//	getDevice()->SetPixelShader(0);
				//}
			}

			void GL3RenderDevice::Render(Material* mtrl, const RenderOperation* op, int count, int passSelID)
			{
				if (!op || count == 0)
					return;

				// glEnable
				// glBindProgramARB

				Effect* fx = mtrl->getPassEffect(passSelID);
				if (!fx)
				{
					fx = m_defaultEffect;
					return;
				}
				
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



				if (m_nativeState->getAlphaTestEnable() != mtrl->AlphaTestEnabled ||
					m_nativeState->getAlphaReference() != mtrl->AlphaReference)
				{
					m_nativeState->SetAlphaTestParameters(mtrl->AlphaTestEnabled, 
						mtrl->AlphaReference);
				}

				if (m_nativeState->getDepthBufferEnabled() != mtrl->DepthTestEnabled ||
					m_nativeState->getDepthBufferWriteEnabled() != mtrl->DepthWriteEnabled)
				{
					m_nativeState->SetDepth(mtrl->DepthTestEnabled,
						mtrl->DepthWriteEnabled);
				}
				int passCount = fx->Begin();
				for (int p = 0; p < passCount; p++)
				{
					fx->BeginPass(p);

					for (int j=0;j<count;j++)
					{
						const RenderOperation& rop = op[j];
						const GeometryData* gm = rop.GeometryData;
							
						if (!gm->VertexCount || !gm->PrimitiveCount)
						{
							fx->EndPass();
							break;
						}

						m_primitiveCount += gm->PrimitiveCount;
						m_vertexCount += gm->VertexCount;
						m_batchCount++;

						// setup effect
						fx->Setup(mtrl, &rop, 1);

						//D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);
						//getDevice()->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);

						//getDevice()->SetVertexDeclaration(static_cast<D3D9VertexDeclaration*>(gm->VertexDecl)->getD3DDecl());

						//if (gm->usesIndex())
						//{
						//	D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
						//	getDevice()->SetIndices(dib->getD3DBuffer());

						//	getDevice()->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
						//		gm->BaseVertex, 0,
						//		gm->VertexCount, 0, 
						//		gm->PrimitiveCount);
						//}
						//else
						//{
						//	getDevice()->SetIndices(0);

						//	getDevice()->DrawPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType),
						//		0, gm->PrimitiveCount);
						//}
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
			Capabilities* const GL3RenderDevice::getCapabilities() const
			{
				return m_caps; 
			}
			PixelFormat GL3RenderDevice::GetDefaultRTFormat()
			{
				D3DSURFACE_DESC desc;
				m_defaultDS->GetDesc(&desc);

				return D3D9Utils::ConvertBackPixelFormat(desc.Format);
			}
			DepthFormat GL3RenderDevice::GetDefaultDepthStencilFormat()
			{
				D3DSURFACE_DESC desc;
				m_defaultDS->GetDesc(&desc);

				return D3D9Utils::ConvertBackDepthFormat(desc.Format);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			void getGlVersion(int *major, int *minor)
			{
				const char *verstr = (const char *) glGetString(GL_VERSION);
				if ((verstr == NULL) || (sscanf(verstr,"%d.%d", major, minor) != 2))
				{
					*major = *minor = 0;
					fprintf(stderr, "Invalid GL_VERSION format!!!\n");
				}
			}

			void getGlslVersion(int *major, int *minor)
			{
				int gl_major, gl_minor;
				getGlVersion(&gl_major, &gl_minor);
				*major = *minor = 0;
				if(gl_major == 1)
				{
					if (GLEW_ARB_shading_language_100)
					{
						*major = 1;
						*minor = 0;
					}
				}
				else if (gl_major >= 2)
				{
					/* GL v2.0 and greater must parse the version string */
					const char *verstr =
						(const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
					if((verstr == NULL) ||
						(sscanf(verstr, "%d.%d", major, minor) != 2))
					{
						*major = *minor = 0;
						fprintf(stderr,
							"Invalid GL_SHADING_LANGUAGE_VERSION format!!!\n");
					}
				}
			}
			bool GL3Capabilities::SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
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
			}

			bool GL3Capabilities::SupportsPixelShader(int majorVer, int minorVer)
			{
				int ma, mi;
				getGlslVersion(&ma, &mi);

				switch (ma)
				{
				case 1:
					{
						if (mi >= 4)
							return majorVer <= 3; // GLSL 1.5 == SM3.0 at least
						else if (mi <= 3)
							return majorVer <= 2;
						
					}
					break;
				case 3:
					{
						if (mi >= 3)
							return majorVer <= 4; // GLSL 3.3 = SM4.0, minorVer ignored
						else
							return majorVer <= 3;
					}
					break;
				case 4:
					{
						if (mi >= 1)
							return majorVer <= 5; // GLSL 4.1 = SM5.0, minorVer ignored
						else
							return majorVer <= 4;
					}
					break;
				}
				return false;
			}

			bool GL3Capabilities::SupportsVertexShader(int majorVer, int minorVer)
			{
				return SupportsPixelShader(majorVer, minorVer);
			}

			int GL3Capabilities::GetMRTCount()
			{
				GLint maxbuffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
				return maxbuffers;
			}
		}
	}
}