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

#include "NRSRenderDevice.h"

#include "NRSShader.h"
#include "NRSRenderTarget.h"
#include "NRSTexture.h"
#include "NRSRenderStateManager.h"
#include "NRSObjects.h"
#include "NRSSprite.h"

#include "apoc3d/Exception.h"

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
		namespace NullRenderSystem
		{
			NRSRenderDevice::NRSRenderDevice()
				: RenderDevice(L"Null RenderSystem"), m_currentViewport({0,0,ResolutionX,ResolutionY})
			{ }

			NRSRenderDevice::~NRSRenderDevice()
			{
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
				if (m_instancingData)
					delete m_instancingData;
			}

			

			void NRSRenderDevice::Initialize()
			{
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[NRS]Initializing Null Render Device. ", 
					LOGLVL_Infomation);

				m_nativeState = new NativeStateManager(this);
				m_stateManager = new NRSRenderStateManager(this, m_nativeState);
				m_renderStates = m_stateManager;
				m_objectFactory = new NRSObjectFactory(this);

				m_caps = new NRSCapabilities(this);
				m_cachedRenderTarget = new RenderTarget*[m_caps->GetMRTCount()]();

				m_instancingData = new NRSInstancingData(this);
			}
			
			void NRSRenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
			}
			void NRSRenderDevice::EndFrame()
			{
				RenderDevice::EndFrame();
			}

			void NRSRenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
			}

			static void RetriveRTTex(RenderTarget* rt, Texture*& rtTex)
			{
				rtTex = nullptr;

				NRSRenderTarget* drt = up_cast<NRSRenderTarget*>(rt);
				if (drt)
				{
					rtTex = drt->getTextureRef();
				}
				else
				{
					NRSCubemapRenderTarget::RefRenderTarget* crt = up_cast<NRSCubemapRenderTarget::RefRenderTarget*>(rt);
					if (crt)
					{
						rtTex = crt->getParent()->GetColorTexture();
					}
				}
			}

			void NRSRenderDevice::SetRenderTarget(int32 index, RenderTarget* rt)
			{
				RenderTarget* oldRt = m_cachedRenderTarget[index];

				if (rt == oldRt)
					return;

				if (rt)
				{
					Texture* rtTex;
					RetriveRTTex(rt, rtTex);
					
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
					m_cachedRenderTarget[index] = nullptr;
				}
			}

			RenderTarget* NRSRenderDevice::GetRenderTarget(int32 index)
			{
				return m_cachedRenderTarget[index];
			}

			void NRSRenderDevice::SetDepthStencilBuffer(DepthStencilBuffer* buf)
			{
				if (m_currentDepthStencil != buf)
				{
					m_currentDepthStencil = static_cast<NRSDepthBuffer*>(buf);
				}
			}
			DepthStencilBuffer* NRSRenderDevice::GetDepthStencilBuffer()
			{
				return m_currentDepthStencil;
			}

			void NRSRenderDevice::BindVertexShader(Shader* shader)
			{
				if (m_currentVS != shader)
				{
					m_currentVS = static_cast<NRSVertexShader*>(shader);
				}
			}
			void NRSRenderDevice::BindPixelShader(Shader* shader)
			{
				if (m_currentPS != shader)
				{
					m_currentPS = static_cast<NRSPixelShader*>(shader);
				}
			}

			void NRSRenderDevice::Render(Material* mtrl, const RenderOperation* op, int32 count, int32 passSelID)
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

							uint32 lastActuallConut = 0;
							int currentIndex = 0;
							while (currentIndex<count)
							{
								uint32 actual = (uint)m_instancingData->Setup(op, count, currentIndex);

								fx->Setup(mtrl, op+currentIndex, actual);

								m_batchCount++;
								currentIndex += actual;
							}
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
						}
					}
					
					fx->EndPass();
				}
				fx->End();
			}

			PixelFormat NRSRenderDevice::GetDefaultRTFormat()
			{
				return FMT_A8R8G8B8;
			}
			DepthFormat NRSRenderDevice::GetDefaultDepthStencilFormat()
			{
				return DEPFMT_Depth24Stencil8;
			}
			uint32 NRSRenderDevice::GetAvailableVideoRamInMB()
			{
				return 128;
			}
			Capabilities* const NRSRenderDevice::getCapabilities() const 
			{
				return m_caps; 
			}


			/************************************************************************/
			/*  NRSCapabilities                                                     */
			/************************************************************************/

			inline const uint32 packAAProfileTableHash(PixelFormat pixFormat, DepthFormat depthFormat)
			{
				return ((uint32)pixFormat << 8) | (uint32)depthFormat;
			}

			NRSCapabilities::NRSCapabilities(NRSRenderDevice* device)
				: m_device(device)
			{
				
			}
			NRSCapabilities::~NRSCapabilities()
			{
				m_aaProfileLookup.DeleteValuesAndClear();
			}


			bool NRSCapabilities::SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				if (!RenderTarget::IsMultisampleModeStringNone(multisampleMode))
				{
					return !!LookupAAProfile(multisampleMode, pixFormat, depthFormat);
				}
				else
				{
					return true;
				}
			}

			bool NRSCapabilities::SupportsPixelShader(const char* implType, int majorVer, int minorVer)
			{
				if (strcmp(implType, "hlsl"))
				{
					return false;
				}

				return majorVer < 4;
			}

			bool NRSCapabilities::SupportsVertexShader(const char* implType, int majorVer, int minorVer)
			{
				if (strcmp(implType, "hlsl"))
				{
					return false;
				}
				return majorVer < 4;
			}

			int32 NRSCapabilities::GetMRTCount()
			{
				return 4;
			}
			bool NRSCapabilities::SupportsMRTDifferentBits()
			{
				return true;
			}
			bool NRSCapabilities::SupportsMRTWriteMasks()
			{
				return true;
			}

			void NRSCapabilities::EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes)
			{
				ProfileTable* profileTable = EnsureCurrentAAModes(pixFormat, depthFormat);

				List<AAProfile> profileListing;
				profileTable->FillValues(profileListing);

				for (AAProfile& aap : profileListing)
					modes.Add(aap.Name);
			}
			const String* NRSCapabilities::FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				ProfileTable& profileTable = *EnsureCurrentAAModes(pixFormat, depthFormat);

				return &(*profileTable.begin()).Value.Name;
			}

			const NRSCapabilities::AAProfile* NRSCapabilities::LookupAAProfile(const String& name, PixelFormat pixFormat, DepthFormat depthFormat)
			{
				ProfileTable* profileTable = EnsureCurrentAAModes(pixFormat, depthFormat);

				return profileTable->TryGetValue(name);
			}

			NRSCapabilities::ProfileTable* NRSCapabilities::EnsureCurrentAAModes(PixelFormat pixFormat, DepthFormat depthFormat)
			{
				uint32 tableCode = packAAProfileTableHash(pixFormat, depthFormat);
				ProfileTable* profileTable = nullptr;
				if (!m_aaProfileLookup.TryGetValue(tableCode, profileTable))
				{
					profileTable = GenerateSupportedAAModes(pixFormat, depthFormat);
					m_aaProfileLookup.Add(tableCode, profileTable);
				}
				return profileTable;
			}
			NRSCapabilities::ProfileTable* NRSCapabilities::GenerateSupportedAAModes(PixelFormat pixFormat, DepthFormat depthFormat)
			{
				ProfileTable* profiles = new ProfileTable();

				AAProfile p;
				p.Name = L"4x MSAA";

				if (!profiles->Contains(p.Name))
				{
					profiles->Add(p.Name, p);
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


			bool NRSCapabilities::FindCompatibleTextureFormat(PixelFormat& format)
			{
				return true;
			}
			bool NRSCapabilities::FindCompatibleTextureDimension(int32& width, int32& height, int32& miplevels)
			{
				if (width > (int32)MaxTextureWidth)
					width = (int32)MaxTextureWidth;

				if (height > (int32)MaxTextureHeight)
					height = (int32)MaxTextureHeight;

				if (miplevels>1)
				{
					int32 mipCount = CalculateLevelCount(Math::Max(width, height));

					if (miplevels>mipCount)
						miplevels = mipCount;
				}
				return true;
			}
			bool NRSCapabilities::FindCompatibleCubeTextureDimension(int32& length, int32& miplevels)
			{
				if (length > (int32)MaxTextureWidth)
					length = (int32)MaxTextureWidth;

				if (length > (int32)MaxTextureHeight)
					length = (int32)MaxTextureHeight;

				if (miplevels>1)
				{
					int32 mipCount = CalculateLevelCount(length);

					if (miplevels>mipCount)
						miplevels = mipCount;
				}
				return true;
			}
			bool NRSCapabilities::FindCompatibleVolumeTextureDimension(int32& width, int32& height, int32& depth, int32& miplevels)
			{
				if (width > (int32)MaxVolumeExtent)
					width = (int32)MaxVolumeExtent;
				if (height > (int32)MaxVolumeExtent)
					height = (int32)MaxVolumeExtent;
				if (depth > (int32)MaxVolumeExtent)
					depth = (int32)MaxVolumeExtent;

				if (miplevels>1)
				{
					int32 maxDimension = Math::Max(Math::Max(width, height), depth);
					int32 mipCount = CalculateLevelCount(maxDimension);

					if (miplevels > mipCount)
						miplevels = mipCount;
				}

				return true;
			}


		}
	}
}