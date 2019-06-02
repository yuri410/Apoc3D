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

#include "D3D9RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "D3D9Utils.h"

#include "D3D9TypedShader.h"
#include "D3D9RenderTarget.h"
#include "D3D9Texture.h"
#include "D3D9RenderStateManager.h"
#include "D3D9ObjectFactory.h"
#include "D3D9InstancingData.h"
#include "D3D9DepthBuffer.h"
#include "Buffer/D3D9IndexBuffer.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9Sprite.h"
#include "VolatileResource.h"
#include "RawSettings.h"

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
					: m_device(device) { }

				virtual void Setup(Material* mtrl, const RenderOperation* rop, int count)
				{
					IDirect3DDevice9* dev = m_device->getDevice();
					if (mtrl->getTexture(0))
					{
						D3D9Texture* tex = static_cast<D3D9Texture*>(mtrl->getTexture(0)->ObtainLoaded());

						dev->SetTexture(0, tex->getBaseTexture());
					}

					dev->SetTransform(D3DTS_WORLD, &reinterpret_cast<const D3DMATRIX&>(rop->RootTransform));
					if (RendererEffectParams::CurrentCamera)
					{
						const Matrix& view = RendererEffectParams::CurrentCamera->getViewMatrix();
						dev->SetTransform(D3DTS_VIEW, &reinterpret_cast<const D3DMATRIX&>(view));

						const Matrix& proj = RendererEffectParams::CurrentCamera->getProjMatrix();
						dev->SetTransform(D3DTS_PROJECTION, &reinterpret_cast<const D3DMATRIX&>(proj));
					}
					dev->SetFVF(D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1);
				}

				virtual void BeginPass(int passId)
				{ }
				virtual void EndPass()
				{ }

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
				: RenderDevice(L"Direct3D9 RenderSystem"), m_devManager(devManager)
			{ }

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
				for (int32 i = 0; i < m_volatileResources.getCount(); i++)
				{
					m_volatileResources[i]->ReleaseVolatileResource();
				}
				if (m_defaultRT)
				{
					m_defaultRT->Release();
					m_defaultRT = nullptr;
				}
				if (m_defaultDS)
				{
					m_defaultDS->Release();
					m_defaultDS = nullptr;
				}
			}
			void D3D9RenderDevice::OnDeviceReset()
			{
				D3DDevice* dev = m_devManager->getDevice();
				dev->GetRenderTarget(0, &m_defaultRT);
				dev->GetDepthStencilSurface(&m_defaultDS);
				for (int32 i = 0; i < m_volatileResources.getCount(); i++)
				{
					m_volatileResources[i]->ReloadVolatileResource();
				}
				m_nativeState->Reset();
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
				m_cachedRenderTarget = new RenderTarget*[caps.NumSimultaneousRTs]();
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
				RenderDevice::EndFrame();
			}

			void D3D9RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
				assert(getDevice());
				getDevice()->Clear(0, NULL, D3D9Utils::ConvertClearFlags(flags), color, depth, stencil);
			}

			static void RetriveRTSurface(RenderTarget* rt, IDirect3DSurface9*& rtSurface, Texture*& rtTex)
			{
				rtSurface = nullptr;
				rtTex = nullptr;

				D3D9RenderTarget* drt = up_cast<D3D9RenderTarget*>(rt);
				if (drt)
				{
					rtSurface = drt->getColorSurface();
					rtTex = drt->getTextureRef();
				}
				else
				{
					D3D9CubemapRenderTarget::RefRenderTarget* crt = up_cast<D3D9CubemapRenderTarget::RefRenderTarget*>(rt);
					if (crt)
					{
						rtSurface = crt->getColorSurface();
						rtTex = crt->getParent()->GetColorTexture();
					}
				}
			}

			static void NotifyRTDirty(RenderTarget* rt)
			{
				D3D9RenderTarget* drt = up_cast<D3D9RenderTarget*>(rt);
				if (drt)
				{
					drt->NotifyDirty();
				}
			}

			void D3D9RenderDevice::SetRenderTarget(int32 index, RenderTarget* rt)
			{
				D3DDevice* dev = getDevice();
				RenderTarget* oldRt = m_cachedRenderTarget[index];

				if (rt == oldRt)
					return;

				if (rt)
				{
					IDirect3DSurface9* drt;
					Texture* rtTex;
					RetriveRTSurface(rt, drt, rtTex);
					
					HRESULT hr = dev->SetRenderTarget(index, drt);
					assert(SUCCEEDED(hr));

					m_cachedRenderTarget[index] = rt;
					
					if (rtTex)
					{
						for (int32 i = 0; i < m_nativeState->getTextureSlotCount(); i++)
						{
							if (m_nativeState->getTexture(i) == rtTex)
							{
								m_nativeState->SetTexture(i, NULL);
							}
						}
					}
				}
				else 
				{
					if (index == 0)
					{
						HRESULT hr = dev->SetRenderTarget(0, m_defaultRT);
						assert(SUCCEEDED(hr));	
					}
					else
					{
						HRESULT hr = dev->SetRenderTarget(index, 0);
						assert(SUCCEEDED(hr));
					}
					m_cachedRenderTarget[index] = nullptr;
				}

				if (oldRt)
				{
					NotifyRTDirty(oldRt);
				}

#if _DEBUG
				m_hasRtOrDSChangedSinceLastCheck = true;
#endif
			}

			RenderTarget* D3D9RenderDevice::GetRenderTarget(int32 index)
			{
				return m_cachedRenderTarget[index];
			}

			void D3D9RenderDevice::SetDepthStencilBuffer(DepthStencilBuffer* buf)
			{
				D3DDevice* dev = getDevice();

				if (m_currentDepthStencil != buf)
				{
					if (buf)
					{
						D3D9DepthBuffer* dbuf = static_cast<D3D9DepthBuffer*>(buf);

						HRESULT hr = dev->SetDepthStencilSurface(dbuf->getD3DBuffer());
						assert(SUCCEEDED(hr));

						m_currentDepthStencil = dbuf;
					}
					else
					{
						HRESULT hr = dev->SetDepthStencilSurface(m_defaultDS);
						assert(SUCCEEDED(hr));

						m_currentDepthStencil = nullptr;
					}

#if _DEBUG
					m_hasRtOrDSChangedSinceLastCheck = true;
#endif
				}
			}
			DepthStencilBuffer* D3D9RenderDevice::GetDepthStencilBuffer()
			{
				return m_currentDepthStencil;
			}

			void D3D9RenderDevice::BindVertexShader(Shader* shader)
			{
				if (m_currentVS != shader)
				{
					if (shader)
					{
						D3D9VertexShader* vs = static_cast<D3D9VertexShader*>(shader);
						getDevice()->SetVertexShader(vs->getD3DVS());
						m_currentVS = vs;
					}
					else
					{
						getDevice()->SetVertexShader(0);
						m_currentVS = nullptr;
					}
				}
			}
			void D3D9RenderDevice::BindPixelShader(Shader* shader)
			{
				if (m_currentPS != shader)
				{
					if (shader)
					{
						D3D9PixelShader* ps = static_cast<D3D9PixelShader*>(shader);
						getDevice()->SetPixelShader(ps->getD3DPS());
						m_currentPS = ps;
					}
					else
					{
						getDevice()->SetPixelShader(0);
						m_currentPS = nullptr;
					}
				}
			}

			void GetLegitVertexRangeUsed(const GeometryData* gm, D3D9VertexBuffer* dvb,
				int32& vertexRangeStart, int32& vertexRangeCount)
			{
				vertexRangeStart = gm->UsedVertexRangeStart;
				vertexRangeCount = gm->UsedVertexRangeCount;

				if (vertexRangeCount == 0)
				{
					vertexRangeStart = 0;
					vertexRangeCount = dvb->getVertexCount();
				}
			}

			void D3D9RenderDevice::Render(Material* mtrl, const RenderOperation* op, int32 count, int32 passSelID)
			{
				if (!op || count == 0)
					return;

				if (HasBatchReportRequest)
					RenderDevice::Render(mtrl, op, count, passSelID);

				Effect* fx = mtrl->GetPassEffect(passSelID);
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
					m_nativeState->SetAlphaTestParameters(mtrl->AlphaTestEnabled, CompareFunction::GreaterEqual, mtrl->AlphaReference);
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

				D3DDevice* d3dd = getDevice();

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

						m_primitiveCount += firstGM->PrimitiveCount*count;
						m_vertexCount += firstGM->VertexCount*count;

						if (firstGM->usesIndex())
						{
							const GeometryData* gm = firstGM;

							VertexDeclaration* vtxDecl = static_cast<D3D9VertexDeclaration*>(gm->VertexDecl);
							d3dd->SetVertexDeclaration(m_instancingData->ExpandVertexDecl(vtxDecl));

							D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
							D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);

							d3dd->SetIndices(dib->getD3DBuffer());
							d3dd->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);

							d3dd->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1U));
							d3dd->SetStreamSource(1, m_instancingData->GetInstanceBuffer(), 0, m_instancingData->getInstanceDataSize());

							int32 vertexRangeStart, vertexRangeCount;
							GetLegitVertexRangeUsed(gm, dvb, vertexRangeStart, vertexRangeCount);

							uint32 lastActuallConut = 0;
							int currentIndex = 0;
							while (currentIndex<count)
							{
								uint32 actual = (uint)m_instancingData->Setup(op, count, currentIndex);

								fx->Setup(mtrl, op+currentIndex, actual);

								if (lastActuallConut != actual)
									d3dd->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | actual));

								d3dd->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
									gm->BaseVertex, 
									vertexRangeStart, vertexRangeCount, 
									gm->StartIndex,
									gm->PrimitiveCount);

								m_batchCount++;

								currentIndex += actual;
							}

							d3dd->SetStreamSourceFreq(0,1);
							d3dd->SetStreamSourceFreq(1,1);
						}

					}
					else // the ordinary way. Go through one by one
					{
						for (int j=0;j<count;j++)
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

							D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);
							d3dd->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);

							d3dd->SetVertexDeclaration(static_cast<D3D9VertexDeclaration*>(gm->VertexDecl)->getD3DDecl());

							if (gm->usesIndex())
							{
								int32 vertexRangeStart, vertexRangeCount;
								GetLegitVertexRangeUsed(gm, dvb, vertexRangeStart, vertexRangeCount);

								D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
								d3dd->SetIndices(dib->getD3DBuffer());

								d3dd->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
									gm->BaseVertex, 
									vertexRangeStart, vertexRangeCount,
									gm->StartIndex,
									gm->PrimitiveCount);
							}
							else
							{
								d3dd->SetIndices(0);

								d3dd->DrawPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType),
									gm->BaseVertex, gm->PrimitiveCount);
							}
						}
					}
					
					fx->EndPass();
				}
				fx->End();

#if _DEBUG
				if (m_hasRtOrDSChangedSinceLastCheck)
				{
					m_hasRtOrDSChangedSinceLastCheck = false;

					bool isAnyRtMultisampled = false;
					bool isAnyRtNotMultisampled = false;
					for (int32 i = 0; i < m_caps->GetMRTCount(); i++)
					{
						if (i > 0 && m_cachedRenderTarget[i] == nullptr)
							break;

						bool rtIsMultisampeld = (m_cachedRenderTarget[i] && m_cachedRenderTarget[i]->isMultiSampled()) ||
							(i == 0 && m_devManager->getCurrentSetting()->PresentParameters.MultiSampleType != D3DMULTISAMPLE_NONE);

						isAnyRtMultisampled |= rtIsMultisampeld;
						isAnyRtNotMultisampled |= !rtIsMultisampeld;
					}

					if (isAnyRtMultisampled == isAnyRtNotMultisampled)
					{
						ApocLog(LOG_Graphics, L"Bound render targets has multisample setting mismatch.", LOGLVL_Error);
						DebugBreak();
					}

					bool currentDSIsMultisampeld = (m_currentDepthStencil && m_currentDepthStencil->isMultiSampled()) ||
						m_devManager->getCurrentSetting()->PresentParameters.MultiSampleType != D3DMULTISAMPLE_NONE;

					if (isAnyRtMultisampled != currentDSIsMultisampeld)
					{
						ApocLog(LOG_Graphics, L"Bound depth stencil has multisample setting mismatching render target's.", LOGLVL_Error);
						DebugBreak();
					}
				}
				
#endif
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
			Capabilities* D3D9RenderDevice::getCapabilities() const
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
			uint32 D3D9RenderDevice::GetAvailableVideoRamInMB()
			{
				D3DDevice* dev = m_devManager->getDevice();
				return dev->GetAvailableTextureMem();
			}

			/************************************************************************/
			/*  D3D9Capabilities                                                    */
			/************************************************************************/

			inline const uint32 packAAProfileTableHash(uint32 adapterOrd, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				return (adapterOrd << 16) | ((uint32)pixFormat << 8) | (uint32)depthFormat;
			}

			D3D9Capabilities::D3D9Capabilities(D3D9RenderDevice* device)
				: m_device(device)
			{
				RefreshCaps();
			}
			D3D9Capabilities::~D3D9Capabilities()
			{
				m_aaProfileLookup.DeleteValuesAndClear();
			}


			bool D3D9Capabilities::SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				if (!RenderTarget::IsMultisampleModeStringNone(multisampleMode))
				{
					return !!LookupAAProfile(multisampleMode, pixFormat, depthFormat);
				}
				else
				{
					GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
					IDirect3D9* d3d9 = devMgr->getDirect3D();
					const RawSettings* setting = devMgr->getCurrentSetting();

					if (depthFormat == DEPFMT_Count)
					{
						HRESULT hr = d3d9->CheckDeviceFormat(setting->AdapterOrdinal, setting->DeviceType, setting->AdapterFormat, 
							D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
						return hr == S_OK;
					}
					else
					{
						HRESULT hr = d3d9->CheckDeviceFormat( setting->AdapterOrdinal, setting->DeviceType, setting->AdapterFormat,
							D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3D9Utils::ConvertDepthFormat(depthFormat));

						if (FAILED(hr))
							return false;

						hr = d3d9->CheckDeviceFormat(setting->AdapterOrdinal, setting->DeviceType, setting->AdapterFormat, 
							D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, D3D9Utils::ConvertPixelFormat(pixFormat) );
						return hr == S_OK;
					}
					
				}
			}

			bool D3D9Capabilities::SupportsPixelShader(const char* implType, int majorVer, int minorVer)
			{
				if (strcmp(implType, "hlsl"))
				{
					return false;
				}

				return m_caps.PixelShaderVersion >= D3DPS_VERSION((uint)majorVer, (uint)minorVer);
			}

			bool D3D9Capabilities::SupportsVertexShader(const char* implType, int majorVer, int minorVer)
			{
				if (strcmp(implType, "hlsl"))
				{
					return false;
				}

				return m_caps.VertexShaderVersion >= D3DVS_VERSION((uint)majorVer, (uint)minorVer);
			}

			int32 D3D9Capabilities::GetMRTCount()
			{
				return (int32)m_caps.NumSimultaneousRTs;
			}
			bool D3D9Capabilities::SupportsMRTDifferentBits()
			{
				return (m_caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
			}
			bool D3D9Capabilities::SupportsMRTWriteMasks()
			{
				return (m_caps.PrimitiveMiscCaps & D3DPMISCCAPS_INDEPENDENTWRITEMASKS) != 0;
			}
			int32 AAProfileComparison(const D3D9Capabilities::AAProfile& a, const D3D9Capabilities::AAProfile& b)
			{
				return Apoc3D::Collections::OrderComparer(a.Sorter, b.Sorter);
			}

			void D3D9Capabilities::EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes)
			{
				ProfileTable* profileTable = EnsureCurrentAAModes(pixFormat, depthFormat);

				List<AAProfile> profileListing;
				profileTable->FillValues(profileListing);
				profileListing.Sort(AAProfileComparison);

				for (AAProfile& aap : profileListing)
					modes.Add(aap.Name);
			}
			const String* D3D9Capabilities::FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				const String* closest = nullptr;
				int32 closestD = 0x7fffffff;

				ProfileTable& profileTable = *EnsureCurrentAAModes(pixFormat, depthFormat);

				for (const AAProfile& ap : profileTable.getValueAccessor())
				{
					uint32 cnt = D3D9Utils::ConvertBackMultiSample(ap.SampleType);
					int32 d = (int32)sampleCount - cnt;
					if (d < 0) d = -d;
					
					if (d < closestD)
					{
						closest = &ap.Name;
						closestD = d;
					}
				}
				return closest;
			}

			const D3D9Capabilities::AAProfile* D3D9Capabilities::LookupAAProfile(const String& name, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				ProfileTable* profileTable = EnsureCurrentAAModes(pixFormat, depthFormat);

				return profileTable->TryGetValue(name);
			}

			D3D9Capabilities::ProfileTable* D3D9Capabilities::EnsureCurrentAAModes(PixelFormat pixFormat, DepthFormat depthFormat)
			{
				GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
				const RawSettings* setting = devMgr->getCurrentSetting();

				uint32 tableCode = packAAProfileTableHash(setting->AdapterOrdinal, pixFormat, depthFormat);
				ProfileTable* profileTable = nullptr;
				if (!m_aaProfileLookup.TryGetValue(tableCode, profileTable))
				{
					profileTable = GenerateSupportedAAModes(setting, pixFormat, depthFormat);
					m_aaProfileLookup.Add(tableCode, profileTable);
				}
				return profileTable;
			}
			D3D9Capabilities::ProfileTable* D3D9Capabilities::GenerateSupportedAAModes(const RawSettings* setting, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
				IDirect3D9* d3d9 = devMgr->getDirect3D();

				D3DADAPTER_IDENTIFIER9 aid;
				d3d9->GetAdapterIdentifier(setting->AdapterOrdinal, 0, &aid);
				bool isNvidia = aid.VendorId == 0x10DE;
				bool isAMD = aid.VendorId == 0x1002;

				struct VenderSpecificAACombo
				{
					const wchar_t* name;
					D3DMULTISAMPLE_TYPE sampleType;
					DWORD qualityLevel;

					int32 sorter;
				};
				const VenderSpecificAACombo nvidiaPresets[] = 
				{
					{ L"8x CSAA", D3DMULTISAMPLE_4_SAMPLES, 2, 81 },
					{ L"8xQ CSAA", D3DMULTISAMPLE_8_SAMPLES, 0, 82 },
					{ L"16x CSAA", D3DMULTISAMPLE_4_SAMPLES, 4, 161 },
					{ L"16xQ CSAA", D3DMULTISAMPLE_8_SAMPLES, 2, 162 },
				};
				const VenderSpecificAACombo amdPresets[] = 
				{
					{ L"2x MSAA", D3DMULTISAMPLE_NONMASKABLE, 0, 20 },
					{ L"2f4x EQAA", D3DMULTISAMPLE_NONMASKABLE, 1, 21 },
					{ L"4x MSAA", D3DMULTISAMPLE_NONMASKABLE, 2, 40 },
					{ L"2f8x EQAA", D3DMULTISAMPLE_NONMASKABLE, 3, 41 },
					{ L"4f8x EQAA", D3DMULTISAMPLE_NONMASKABLE, 4, 42 },
					{ L"4f16x EQAA", D3DMULTISAMPLE_NONMASKABLE, 5, 43 },
					{ L"8x MSAA", D3DMULTISAMPLE_NONMASKABLE, 6, 80 },
					{ L"8f16x EQAA", D3DMULTISAMPLE_NONMASKABLE, 7, 81 },

					{ L"2x MSAA", D3DMULTISAMPLE_NONMASKABLE, 0, 20 },
					{ L"2x MSAA", D3DMULTISAMPLE_NONMASKABLE, 1, 20 },
					{ L"2f4x EQAA", D3DMULTISAMPLE_NONMASKABLE, 2, 21 },
					{ L"2f4x EQAA", D3DMULTISAMPLE_NONMASKABLE, 3, 21 },
					{ L"2f8x EQAA", D3DMULTISAMPLE_NONMASKABLE, 4, 41 },

					{ L"4x MSAA", D3DMULTISAMPLE_NONMASKABLE, 0, 40 },
					{ L"4x MSAA", D3DMULTISAMPLE_NONMASKABLE, 1, 40 },
					{ L"4f8x EQAA", D3DMULTISAMPLE_NONMASKABLE, 2, 42 },

					{ L"8x MSAA", D3DMULTISAMPLE_NONMASKABLE, 0, 80 },
					{ L"8x MSAA", D3DMULTISAMPLE_NONMASKABLE, 1, 80 },
					{ L"8f16x EQAA", D3DMULTISAMPLE_NONMASKABLE, 27, 81 },
				};

				struct  
				{
					const wchar_t* name;
					D3DMULTISAMPLE_TYPE type;

				} const possibleMultisampleTypes[] = {
					{ L"", D3DMULTISAMPLE_NONMASKABLE },
					{ L"2x", D3DMULTISAMPLE_2_SAMPLES },
					{ L"3x", D3DMULTISAMPLE_3_SAMPLES },
					{ L"4x", D3DMULTISAMPLE_4_SAMPLES },
					{ L"5x", D3DMULTISAMPLE_5_SAMPLES },
					{ L"6x", D3DMULTISAMPLE_6_SAMPLES },
					{ L"7x", D3DMULTISAMPLE_7_SAMPLES },
					{ L"8x", D3DMULTISAMPLE_8_SAMPLES },
					{ L"9x", D3DMULTISAMPLE_9_SAMPLES },
					{ L"10x", D3DMULTISAMPLE_10_SAMPLES },
					{ L"11x", D3DMULTISAMPLE_11_SAMPLES },
					{ L"12x", D3DMULTISAMPLE_12_SAMPLES },
					{ L"13x", D3DMULTISAMPLE_13_SAMPLES },
					{ L"14x", D3DMULTISAMPLE_14_SAMPLES },
					{ L"15x", D3DMULTISAMPLE_15_SAMPLES },
					{ L"16x", D3DMULTISAMPLE_16_SAMPLES }
				};

				ProfileTable* profiles = new ProfileTable();
				for (const auto& e : possibleMultisampleTypes)
				{
					const D3DMULTISAMPLE_TYPE& type = e.type;
					const wchar_t* name = e.name;

					bool supported = false;
					DWORD qualityCount;
					if (depthFormat == DEPFMT_Count)
					{
						HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->AdapterOrdinal, setting->DeviceType, 
							D3D9Utils::ConvertPixelFormat(pixFormat), setting->PresentParameters.Windowed, type, &qualityCount);
						supported = SUCCEEDED(hr);
					}
					else if (pixFormat == FMT_Count)
					{
						HRESULT hr = d3d9->CheckDeviceMultiSampleType(setting->AdapterOrdinal, setting->DeviceType,
							D3D9Utils::ConvertDepthFormat(depthFormat), setting->PresentParameters.Windowed, type, &qualityCount);
						supported = SUCCEEDED(hr);
					}
					else
					{
						DWORD quality;
						DWORD quality2;

						HRESULT hr1 = d3d9->CheckDeviceMultiSampleType(setting->AdapterOrdinal, setting->DeviceType, 
							D3D9Utils::ConvertPixelFormat(pixFormat), setting->PresentParameters.Windowed, type, &quality);
						HRESULT hr2 = d3d9->CheckDeviceMultiSampleType(setting->AdapterOrdinal, setting->DeviceType, 
							D3D9Utils::ConvertDepthFormat(depthFormat), setting->PresentParameters.Windowed, type, &quality2);

						if (SUCCEEDED(hr1) && SUCCEEDED(hr2) && quality2 == quality && quality > 0)
						{
							supported = true;
							qualityCount = quality;
						}
					}


					if (supported && qualityCount > 0)
					{
						bool isVenderSpecial = false;
						DWORD maxSpecialAAQualityLevel = 0;

						const VenderSpecificAACombo* venderPreset = nullptr;
						int32 venderPresetCount = 0;

						if (isNvidia)
						{
							venderPreset = nvidiaPresets;
							venderPresetCount = ARRAYSIZE(nvidiaPresets);
						}
						if (isAMD)
						{
							venderPreset = amdPresets;
							venderPresetCount = ARRAYSIZE(amdPresets);
						}

						for (int32 j=0;j<venderPresetCount;j++)
						{
							const VenderSpecificAACombo& aac = venderPreset[j];
							if (aac.sampleType == type && aac.qualityLevel < qualityCount)
							{
								AAProfile p;
								p.Name = aac.name;
								p.SampleQuality = aac.qualityLevel;
								p.SampleType = aac.sampleType;
								p.Sorter = aac.sorter;

								if (!profiles->Contains(p.Name))
								{
									profiles->Add(p.Name, p);
								}

								//modes.Add(aac.name);
								isVenderSpecial = true;
								if (aac.qualityLevel > maxSpecialAAQualityLevel)
									maxSpecialAAQualityLevel = aac.qualityLevel;
							}
						}


						if (isVenderSpecial && maxSpecialAAQualityLevel + 1 == qualityCount)
						{
							// no nothing, already added as a vender setting
						}
						else
						{
							if (type == D3DMULTISAMPLE_NONMASKABLE)
							{
								AAProfile p;
								p.Name = L"Maximum";
								p.SampleQuality = qualityCount-1;
								p.SampleType = type;
								p.Sorter = 65535;

								if (!profiles->Contains(p.Name))
								{
									profiles->Add(p.Name, p);
								}
							}
							else
							{
								AAProfile p;
								p.Name = String(name) + L" MSAA";
								p.SampleQuality = qualityCount-1;
								p.SampleType = type;
								p.Sorter = D3D9Utils::ConvertBackMultiSample(type) * 10;

								if (!profiles->Contains(p.Name))
								{
									profiles->Add(p.Name, p);
								}
							}
						}
					}
				}

				return profiles;
			}

			

			int32 CalculateLevelCount(int32 maxDimension)
			{
				int32 lvlCount = 1;
				while (maxDimension > 1)
				{
					maxDimension /= 2;
					lvlCount++;
				}
				return lvlCount;
			}


			bool D3D9Capabilities::FindCompatibleTextureFormat(PixelFormat& format)
			{
				GraphicsDeviceManager* devMgr = m_device->getGraphicsDeviceManager();
				IDirect3D9* d3d9 = devMgr->getDirect3D();
				const RawSettings* setting = devMgr->getCurrentSetting();

				if (format == FMT_Unknown)
					format = FMT_A8R8G8B8;

				HRESULT hr = d3d9->CheckDeviceFormat(setting->AdapterOrdinal, setting->DeviceType, setting->AdapterFormat,
					0, D3DRTYPE_TEXTURE, D3D9Utils::ConvertPixelFormat(format));
				if (FAILED(hr))
				{
					int bestScore = INT_MAX;

					int32 originalChBitDepths[4];
					PixelFormatUtils::GetChannelBitDepth(format, originalChBitDepths);

					int32 originalChCount = PixelFormatUtils::GetChannelCount(format);
					bool allow24bits = PixelFormatUtils::GetBPP(format) == 3;

					format = FMT_Unknown;

					for (int32 i=FMT_Unknown+1;i<FMT_Count;i++)
					{
						PixelFormat curFmt = (PixelFormat)i;

						if (PixelFormatUtils::IsCompressed(curFmt))
							continue;

						int32 curBpp = PixelFormatUtils::GetBPP(curFmt);
						if (curBpp <= 0)
							continue; // these formats can not be manipulated

						int32 chnCount = PixelFormatUtils::GetChannelCount(curFmt);
						if (chnCount<originalChCount)
							continue;

						if (!allow24bits && curBpp==3)
							continue;

						hr = d3d9->CheckDeviceFormat(setting->AdapterOrdinal, setting->DeviceType, setting->AdapterFormat, 
							0, D3DRTYPE_TEXTURE, D3D9Utils::ConvertPixelFormat(curFmt));
						if (FAILED(hr))
							continue;

						int32 penalty = 4 * (chnCount - originalChCount);

						int32 curChBitDepths[4];
						PixelFormatUtils::GetChannelBitDepth(curFmt, curChBitDepths);

						for (int32 j = 0; j < 4; j++)
						{
							int bitPenalty = curChBitDepths[j] - originalChBitDepths[j];
							if (bitPenalty<0)
								bitPenalty = -bitPenalty*4;
							penalty += bitPenalty;
						}

						if (penalty < bestScore)
						{
							bestScore = penalty;
							format = curFmt;
						}
					}
				}

				return format != FMT_Unknown;
			}
			bool D3D9Capabilities::FindCompatibleTextureDimension(int32& width, int32& height, int32& miplevels)
			{
				if (m_caps.TextureCaps & D3DPTEXTURECAPS_POW2)
				{
					if (!IsPowerOfTwo(width))
						width = Math::Pow2RoundUp(width);

					if (!IsPowerOfTwo(height))
						height = Math::Pow2RoundUp(height);
				}
				
				if (m_caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
				{
					int32 maxLen = Math::Min((int32)m_caps.MaxTextureWidth, (int32)m_caps.MaxTextureHeight);
					if (width > maxLen) width = maxLen;
					if (height > maxLen) height = maxLen;

					width = height = Math::Max(width, height);
				}
				else
				{
					if (width > (int32)m_caps.MaxTextureWidth)
						width = (int32)m_caps.MaxTextureWidth;

					if (height > (int32)m_caps.MaxTextureHeight)
						height = (int32)m_caps.MaxTextureHeight;
				}

				if (miplevels>1)
				{
					int32 mipCount = CalculateLevelCount(Math::Max(width, height));

					if (miplevels>mipCount)
						miplevels = mipCount;
				}
				return true;
			}
			bool D3D9Capabilities::FindCompatibleCubeTextureDimension(int32& length, int32& miplevels)
			{
				if ((m_caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) == 0)
					return false;

				if ((m_caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2) && !IsPowerOfTwo(length))
					length = Math::Pow2RoundUp(length);

				if (length > (int32)m_caps.MaxTextureWidth)
					length = (int32)m_caps.MaxTextureWidth;

				if (length > (int32)m_caps.MaxTextureHeight)
					length = (int32)m_caps.MaxTextureHeight;

				if ((m_caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP) == 0)
				{
					miplevels = 1;
				}
				else if (miplevels>1)
				{
					int32 mipCount = CalculateLevelCount(length);

					if (miplevels>mipCount)
						miplevels = mipCount;
				}
				return true;
			}
			bool D3D9Capabilities::FindCompatibleVolumeTextureDimension(int32& width, int32& height, int32& depth, int32& miplevels)
			{
				if ((m_caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) == 0)
					return false;

				if (m_caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2)
				{
					if (!IsPowerOfTwo(width)) 
						width = Math::Pow2RoundUp(width);

					if (!IsPowerOfTwo(height)) 
						height = Math::Pow2RoundUp(height);

					if (!IsPowerOfTwo(depth)) 
						depth = Math::Pow2RoundUp(depth);
				}

				if (width > (int32)m_caps.MaxVolumeExtent)
					width = (int32)m_caps.MaxVolumeExtent;
				if (height > (int32)m_caps.MaxVolumeExtent)
					height = (int32)m_caps.MaxVolumeExtent;
				if (depth > (int32)m_caps.MaxVolumeExtent)
					depth = (int32)m_caps.MaxVolumeExtent;

				if ((m_caps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP) == 0)
				{
					miplevels = 1;
				}
				else if (miplevels>1)
				{
					int32 maxDimension = Math::Max(Math::Max(width, height), depth);
					int32 mipCount = CalculateLevelCount(maxDimension);

					if (miplevels > mipCount)
						miplevels = mipCount;
				}

				return true;
			}


			void D3D9Capabilities::RefreshCaps()
			{
				assert(m_device->getDevice());
				HRESULT hr = m_device->getDevice()->GetDeviceCaps(&m_caps);
				assert(SUCCEEDED(hr));
			}
		}
	}
}