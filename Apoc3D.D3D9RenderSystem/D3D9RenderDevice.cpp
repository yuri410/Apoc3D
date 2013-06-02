/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#include "D3D9RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "D3D9Utils.h"

#include "D3D9PixelShader.h"
#include "D3D9VertexShader.h"
#include "D3D9RenderTarget.h"
#include "D3D9Texture.h"
#include "D3D9RenderStateManager.h"
#include "D3D9ObjectFactory.h"
#include "D3D9InstancingData.h"
#include "Buffer/D3D9DepthBuffer.h"
#include "Buffer/D3D9IndexBuffer.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9Sprite.h"
#include "VolatileResource.h"

#include "apoc3d/Apoc3DException.h"

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
		namespace D3D9RenderSystem
		{
			class BasicEffect : public Effect
			{
			public:
				BasicEffect(D3D9RenderDevice* device)
					: m_device(device)
				{

				}
				virtual void Setup(Material* mtrl, const RenderOperation* rop, int count)
				{
					IDirect3DDevice9* dev = m_device->getDevice();
					if (mtrl->getTexture(0))
					{
						D3D9Texture* tex = static_cast<D3D9Texture*>(mtrl->getTexture(0)->operator->());

						dev->SetTexture(0, tex->getBaseTexture());
					}

					dev->SetTransform(D3DTS_WORLD, &reinterpret_cast<const D3DMatrix&>(rop->RootTransform));
					if (RendererEffectParams::CurrentCamera)
					{
						const Matrix& view = RendererEffectParams::CurrentCamera->getViewMatrix();
						dev->SetTransform(D3DTS_VIEW, &reinterpret_cast<const D3DMatrix&>(view));

						const Matrix& proj = RendererEffectParams::CurrentCamera->getProjMatrix();
						dev->SetTransform(D3DTS_PROJECTION, &reinterpret_cast<const D3DMatrix&>(proj));
					}
					dev->SetFVF(D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1);
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
					IDirect3DDevice9* dev = m_device->getDevice();
					dev->SetVertexShader(0);
					dev->SetPixelShader(0);
					return 1;
				}
				virtual void end()
				{

				}
			private:
				D3D9RenderDevice* m_device;
			};

			D3D9RenderDevice::D3D9RenderDevice(GraphicsDeviceManager* devManager)
				: RenderDevice(L"Direct3D9 RenderSystem"), m_devManager(devManager), 
				m_stateManager(0), m_nativeState(0), m_caps(0), m_cachedRenderTarget(0), m_defaultEffect(0)
			{

			}
			D3D9RenderDevice::~D3D9RenderDevice()
			{
				m_defaultRT->Release();
				m_defaultRT = 0;

				m_defaultDS->Release();
				m_defaultDS = 0;

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
				if (m_instancingData)
					delete m_instancingData;
			}

			D3DDevice* D3D9RenderDevice::getDevice() const { return m_devManager->getDevice(); } 

			
			void D3D9RenderDevice::OnDeviceLost()
			{
				for (int32 i=0;i<m_volatileResources.getCount();i++)
				{
					m_volatileResources[i]->ReleaseVolatileResource();
				}
				m_defaultRT->Release();
				m_defaultRT = 0;
				m_defaultDS->Release();
				m_defaultDS = 0;
			}
			void D3D9RenderDevice::OnDeviceReset()
			{
				D3DDevice* dev = m_devManager->getDevice();
				dev->GetRenderTarget(0, &m_defaultRT);
				dev->GetDepthStencilSurface(&m_defaultDS);
				for (int32 i=0;i<m_volatileResources.getCount();i++)
				{
					m_volatileResources[i]->ReloadVolatileResource();
				}
			}

			void D3D9RenderDevice::Initialize()
			{

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[D3D9]Initializing D3D9 Render Device. ", 
					LOGLVL_Infomation);

				D3DDevice* dev = getDevice();
				D3DCAPS9 caps;
				dev->GetDeviceCaps(&caps);
				
				m_nativeState = new NativeD3DStateManager(this);
				m_stateManager = new D3D9RenderStateManager(this, m_nativeState);
				m_renderStates = m_stateManager;
				m_objectFactory = new D3D9ObjectFactory(this);


				//caps.MaxSimultaneousTextures
				m_cachedRenderTarget = new D3D9RenderTarget*[caps.NumSimultaneousRTs];
				memset(m_cachedRenderTarget, 0, sizeof(D3D9RenderTarget*) * caps.NumSimultaneousRTs);

				m_caps = new D3D9Capabilities(this);
				
				dev->GetRenderTarget(0, &m_defaultRT);
				dev->GetDepthStencilSurface(&m_defaultDS);

				m_defaultEffect = new BasicEffect(this);

				m_instancingData = new D3D9InstancingData(this);
			}
			
			void D3D9RenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
				getDevice()->BeginScene();
			}
			void D3D9RenderDevice::EndFrame()
			{
				getDevice()->EndScene();
			}

			void D3D9RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
				assert(getDevice());
				getDevice()->Clear(0, NULL, D3D9Utils::ConvertClearFlags(flags), color, depth, stencil);
			}

			void D3D9RenderDevice::SetRenderTarget(int index, RenderTarget* rt)
			{
				D3DDevice* dev = getDevice();
				if (rt)
				{
					D3D9RenderTarget* oldRt = m_cachedRenderTarget[index];

					D3D9RenderTarget* drt = static_cast<D3D9RenderTarget*>(rt);
					
					dev->SetRenderTarget(index, drt->getColorSurface());
					if (drt->getDepthSurface())
					{
						if (index)
						{
							throw AP_EXCEPTION(EX_InvalidOperation, L"Render targets with a depth buffer can only be set at index 0.");
						}
						dev->SetDepthStencilSurface(drt->getDepthSurface());
					}

					m_cachedRenderTarget[index] = drt;

					if (oldRt && oldRt != drt)
					{
						m_cachedRenderTarget[index]->Resolve();
					}
				}
				else 
				{
					if (index == 0)
					{
						dev->SetRenderTarget(0, m_defaultRT);
						dev->SetDepthStencilSurface(m_defaultDS);

						m_cachedRenderTarget[0] = 0;
					}
					else
					{
						dev->SetRenderTarget(index, 0);
						m_cachedRenderTarget[index] = 0;
					}
				}
			}

			RenderTarget* D3D9RenderDevice::GetRenderTarget(int index)
			{
				return m_cachedRenderTarget[index];
			}

			void D3D9RenderDevice::BindVertexShader(VertexShader* shader)
			{
				if (shader)
				{
					D3D9VertexShader* vs = static_cast<D3D9VertexShader*>(shader);
					getDevice()->SetVertexShader(vs->getD3DVS());
				}
				else
				{
					getDevice()->SetVertexShader(0);
				}
			}
			void D3D9RenderDevice::BindPixelShader(PixelShader* shader)
			{
				if (shader)
				{
					D3D9PixelShader* ps = static_cast<D3D9PixelShader*>(shader);
					getDevice()->SetPixelShader(ps->getD3DPS());
				}
				else
				{
					getDevice()->SetPixelShader(0);
				}
			}

			void D3D9RenderDevice::Render(Material* mtrl, const RenderOperation* op, int count, int passSelID)
			{
				if (!op || count == 0)
					return;

				//getDevice()->SetVertexShader(0);
				//getDevice()->SetPixelShader(0);

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
				if (m_nativeState->getPointSpriteEnabled() != mtrl->UsePointSprite)
				{
					m_nativeState->SetPointParameters(m_nativeState->getPointSize(), m_nativeState->getPointSizeMax(), m_nativeState->getPointSizeMin(), mtrl->UsePointSprite);
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

					if (fx->SupportsInstancing())
					{
						// here the input render operation list is guaranteed to have the same geometry data,
						// instancing drawing is done here once the effect supports it
						
						const RenderOperation& rop = op[0];
						const GeometryData* gm = rop.GeometryData;

						if (gm->usesIndex())
						{
							//count = 1;
							m_batchCount++;
							m_primitiveCount += gm->PrimitiveCount*count;
							m_vertexCount += gm->VertexCount*count;


							const RenderOperation& rop = op[0];
							const GeometryData* gm = rop.GeometryData;

							VertexDeclaration* vtxDecl = static_cast<D3D9VertexDeclaration*>(gm->VertexDecl);
							getDevice()->SetVertexDeclaration(m_instancingData->ExpandVertexDecl(vtxDecl));

							
							D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
							getDevice()->SetIndices(dib->getD3DBuffer());

							int currentIndex = 0;
							while (currentIndex<count)
							{
								uint actual = (uint)m_instancingData->Setup(op, count, currentIndex);

								fx->Setup(mtrl, op+currentIndex, actual);

									
								D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);
								getDevice()->SetStreamSourceFreq(0,
									(D3DSTREAMSOURCE_INDEXEDDATA | actual));
								getDevice()->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);


								getDevice()->SetStreamSourceFreq(1,
									(D3DSTREAMSOURCE_INSTANCEDATA | 1U));
								getDevice()->SetStreamSource(1, m_instancingData->GetInstanceBuffer(), 0, m_instancingData->getInstanceDataSize());



								D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
								getDevice()->SetIndices(dib->getD3DBuffer());

								getDevice()->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
									gm->BaseVertex, 0,
									gm->VertexCount, 0, 
									gm->PrimitiveCount);
								

								currentIndex += actual;
							}

							getDevice()->SetStreamSourceFreq(0,1);
							getDevice()->SetStreamSourceFreq(1,1);
						}

					}
					else // the ordinary way. Go through one by one
					{
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

							D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);
							getDevice()->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);

							getDevice()->SetVertexDeclaration(static_cast<D3D9VertexDeclaration*>(gm->VertexDecl)->getD3DDecl());

							if (gm->usesIndex())
							{
								D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
								getDevice()->SetIndices(dib->getD3DBuffer());

								getDevice()->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
									gm->BaseVertex, 0,
									gm->VertexCount, 0, 
									gm->PrimitiveCount);
							}
							else
							{
								getDevice()->SetIndices(0);

								getDevice()->DrawPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType),
									0, gm->PrimitiveCount);
							}
						}
					}
					
					fx->EndPass();
				}
				fx->End();
			}

			Viewport D3D9RenderDevice::getViewport()
			{
				D3DVIEWPORT9 dvp;
				getDevice()->GetViewport(&dvp);

				Viewport vp(dvp.X, dvp.Y, dvp.Width, dvp.Height, dvp.MinZ, dvp.MaxZ);
				return vp;
			}
			void D3D9RenderDevice::setViewport(const Viewport& vp)
			{
				D3DVIEWPORT9 dvp;
				dvp.X = vp.X;
				dvp.Y = vp.Y;
				dvp.Width = vp.Width;
				dvp.Height = vp.Height;
				dvp.MinZ = vp.MinZ;
				dvp.MaxZ = vp.MaxZ;

				getDevice()->SetViewport(&dvp);
			}
			Capabilities* const D3D9RenderDevice::getCapabilities() const
			{
				return m_caps; 
			}
			PixelFormat D3D9RenderDevice::GetDefaultRTFormat()
			{
				D3DSURFACE_DESC desc;
				m_defaultRT->GetDesc(&desc);

				return D3D9Utils::ConvertBackPixelFormat(desc.Format);
			}
			DepthFormat D3D9RenderDevice::GetDefaultDepthStencilFormat()
			{
				D3DSURFACE_DESC desc;
				m_defaultDS->GetDesc(&desc);

				return D3D9Utils::ConvertBackDepthFormat(desc.Format);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			bool D3D9Capabilities::SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
				IDirect3D9* d3d9 = devMgr->getDirect3D();
				const DeviceSettings* setting = devMgr->getCurrentSetting();

				if (multisampleCount)
				{
					if (depthFormat == DEPFMT_Count)
					{
						DWORD quality;
						HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
							D3D9Utils::ConvertPixelFormat(pixFormat),
							setting->D3D9.PresentParameters.Windowed, 
							D3D9Utils::ConvertMultisample(multisampleCount),
							&quality);

						return hr == S_OK;
					}
					else
					{
						DWORD quality;
						HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
							D3D9Utils::ConvertPixelFormat(pixFormat),
							setting->D3D9.PresentParameters.Windowed, 
							D3D9Utils::ConvertMultisample(multisampleCount),
							&quality);

						if (FAILED(hr))
							return false;
						
						hr = d3d9->CheckDeviceMultiSampleType(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType,
							D3D9Utils::ConvertDepthFormat(depthFormat),
							setting->D3D9.PresentParameters.Windowed, 
							D3D9Utils::ConvertMultisample(multisampleCount),
							&quality);

						return hr == S_OK;
					}
				}
				else
				{
					if (depthFormat == DEPFMT_Count)
					{
						HRESULT hr = d3d9->CheckDeviceFormat(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat, 
							D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
						return hr == S_OK;
					}
					else
					{
						HRESULT hr = d3d9->CheckDeviceFormat( setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat,
							D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3D9Utils::ConvertDepthFormat(depthFormat));

						if (FAILED(hr))
							return false;

						hr = d3d9->CheckDeviceFormat(setting->D3D9.AdapterOrdinal, setting->D3D9.DeviceType, setting->D3D9.AdapterFormat, 
							D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
						return hr == S_OK;
					}
					
				}
			}

			bool D3D9Capabilities::SupportsPixelShader(int majorVer, int minorVer)
			{
				D3DCAPS9 caps;
				m_device->getDevice()->GetDeviceCaps(&caps);
				return caps.PixelShaderVersion >= D3DPS_VERSION((uint)majorVer, (uint)minorVer);
			}

			bool D3D9Capabilities::SupportsVertexShader(int majorVer, int minorVer)
			{
				D3DCAPS9 caps;
				m_device->getDevice()->GetDeviceCaps(&caps);
				return caps.VertexShaderVersion >= D3DVS_VERSION((uint)majorVer, (uint)minorVer);
			}

			int D3D9Capabilities::GetMRTCount()
			{
				D3DCAPS9 caps;
				m_device->getDevice()->GetDeviceCaps(&caps);
				return caps.NumSimultaneousRTs;
			}
		}
	}
}