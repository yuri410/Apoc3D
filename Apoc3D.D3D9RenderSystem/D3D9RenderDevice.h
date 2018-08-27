#pragma once
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

#ifndef D3D9RENDERDEVICE_H
#define D3D9RENDERDEVICE_H

#include "D3D9Common.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9Capabilities;

			class D3D9RenderDevice final : public RenderDevice
			{
				RTTI_DERIVED(D3D9RenderDevice, RenderDevice);
			public:
				D3D9RenderDevice(GraphicsDeviceManager* devManager);
				~D3D9RenderDevice();
				
				void OnDeviceReset();
				void OnDeviceLost();

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

				virtual Viewport getViewport() override;
				virtual void setViewport(const Viewport& vp) override;

				virtual Capabilities* const getCapabilities() const override;

				virtual uint32 GetAvailableVideoRamInMB() override;

				D3D9Capabilities* getD3D9Capabilities() const { return m_caps; }
				NativeD3DStateManager* getNativeStateManager() const { return m_nativeState; }

				D3DDevice* getDevice() const;
				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_devManager; }

				bool isInitialized() const { return m_stateManager != nullptr; }

			private:
				friend class VolatileResource;

				Effect* m_defaultEffect = nullptr;
				GraphicsDeviceManager* m_devManager = nullptr;
				D3D9RenderStateManager* m_stateManager = nullptr;
				NativeD3DStateManager* m_nativeState = nullptr;

				RenderTarget** m_cachedRenderTarget = nullptr;
				D3D9DepthBuffer* m_currentDepthStencil = nullptr;

				IDirect3DSurface9* m_defaultRT = nullptr;
				IDirect3DSurface9* m_defaultDS = nullptr;

				D3D9Capabilities* m_caps = nullptr;

				List<VolatileResource*> m_volatileResources;

				D3D9InstancingData* m_instancingData = nullptr;

				D3D9VertexShader* m_currentVS = nullptr;
				D3D9PixelShader* m_currentPS = nullptr;

#if _DEBUG
				bool m_hasRtOrDSChangedSinceLastCheck = false;
#endif

				// This is called by the VolatileResource itself
				void TrackVolatileResource(VolatileResource* res)
				{
					m_volatileResources.Add(res);
				}
				void UntrackVolatileResource(VolatileResource* res)
				{
					m_volatileResources.Remove(res);
				}
			};

			class D3D9Capabilities final : public Capabilities
			{
			public:
				struct AAProfile
				{
					String Name;

					int32 Sorter;

					D3DMULTISAMPLE_TYPE SampleType;
					DWORD SampleQuality;
				};

				D3D9Capabilities(D3D9RenderDevice* device);
				~D3D9Capabilities();

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
				ProfileTable* GenerateSupportedAAModes(const RawSettings* setting, PixelFormat pixFormat, DepthFormat depthFormat);

				void RefreshCaps();
				

				D3D9RenderDevice* m_device;

				/**
				 *  This table caches a set of AA profiles for each adapter+color+depth permutation.
				 *  Only generate as needed.
				 */
				ProfileCacheTable m_aaProfileLookup;

				D3DCAPS9 m_caps;
			};
		}
	}
}

#endif