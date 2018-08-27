#pragma once
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

#ifndef NRSRENDERDEVICE_H
#define NRSRENDERDEVICE_H

#include "NRSCommon.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSCapabilities;

			class NRSRenderDevice final : public RenderDevice
			{
				RTTI_DERIVED(NRSRenderDevice, RenderDevice);
			public:
				static const int32 ResolutionX = 1280;
				static const int32 ResolutionY = 720;

				NRSRenderDevice();
				~NRSRenderDevice();
				
				virtual void Initialize() override;
				
				virtual void BeginFrame() override;
				virtual void EndFrame() override;
				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil) override;

				virtual void SetRenderTarget(int32 index, RenderTarget* rt) override;
				virtual RenderTarget* GetRenderTarget(int32 index) override;

				virtual void SetDepthStencilBuffer(DepthStencilBuffer* buf) override;
				virtual DepthStencilBuffer* GetDepthStencilBuffer() override;


				virtual PixelFormat GetDefaultRTFormat() override;
				virtual DepthFormat GetDefaultDepthStencilFormat() override;
				
				virtual void BindVertexShader(Shader* shader) override;
				virtual void BindPixelShader(Shader* shader) override;

				virtual void Render(Material* mtrl, const RenderOperation* op, int32 count, int32 passSelID) override;

				virtual Viewport getViewport() override { return m_currentViewport; }
				virtual void setViewport(const Viewport& vp) override { m_currentViewport = vp; }

				virtual Capabilities* const getCapabilities() const override;

				virtual uint32 GetAvailableVideoRamInMB() override;

				NRSCapabilities* getNRSCapabilities() const { return m_caps; }
				NativeStateManager* getNativeStateManager() const { return m_nativeState; }

				bool isInitialized() const { return m_stateManager != nullptr; }

			private:

				NRSRenderStateManager* m_stateManager = nullptr;
				NativeStateManager* m_nativeState = nullptr;

				RenderTarget** m_cachedRenderTarget = nullptr;
				NRSDepthBuffer* m_currentDepthStencil = nullptr;


				NRSCapabilities* m_caps = nullptr;

				NRSInstancingData* m_instancingData = nullptr;

				NRSVertexShader* m_currentVS = nullptr;
				NRSPixelShader* m_currentPS = nullptr;
				Viewport m_currentViewport;

			};

			class NRSCapabilities final : public Capabilities
			{
			public:
				static const int32 MaxTextureWidth = 8192;
				static const int32 MaxTextureHeight = 8192;
				static const int32 MaxVolumeExtent = 256;

				struct AAProfile
				{
					String Name;
				};

				NRSCapabilities(NRSRenderDevice* device);
				~NRSCapabilities();

				bool SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat) override;
				bool SupportsPixelShader(const char* implType, int majorVer, int minorVer) override;
				bool SupportsVertexShader(const char* implType, int majorVer, int minorVer) override;

				void EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes) override;
				const String* FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat) override;
				
				int32 GetMRTCount() override;
				bool SupportsMRTDifferentBits() override;
				bool SupportsMRTWriteMasks() override;

				const AAProfile* LookupAAProfile(const String& name, PixelFormat pixFormat, DepthFormat depthFormat);

				bool FindCompatibleTextureFormat(PixelFormat& format);
				bool FindCompatibleTextureDimension(int32& width, int32& height, int32& miplevels);
				bool FindCompatibleCubeTextureDimension(int32& length, int32& miplevels);
				bool FindCompatibleVolumeTextureDimension(int32& width, int32& height, int32& depth, int32& miplevels);


			private:
				typedef HashMap<String, AAProfile> ProfileTable;
				typedef HashMap<uint32, ProfileTable*> ProfileCacheTable;

				ProfileTable* EnsureCurrentAAModes(PixelFormat pixFormat, DepthFormat depthFormat);
				ProfileTable* GenerateSupportedAAModes(PixelFormat pixFormat, DepthFormat depthFormat);

				NRSRenderDevice* m_device;
				ProfileCacheTable m_aaProfileLookup;
			};
		}
	}
}

#endif