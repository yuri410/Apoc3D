#pragma once
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

			class D3D9RenderDevice : public RenderDevice
			{
			public:
				D3DDevice* getDevice() const;
				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_devManager; } 

				
				bool isInitialized() const { return !!m_stateManager; }

				D3D9RenderDevice(GraphicsDeviceManager* devManager);
				~D3D9RenderDevice();
				
				void OnDeviceReset();
				void OnDeviceLost();


				virtual void Initialize();
				
				virtual void BeginFrame();
				virtual void EndFrame();
				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil);

				virtual void SetRenderTarget(int index, RenderTarget* rt);

				virtual RenderTarget* GetRenderTarget(int index);

				virtual PixelFormat GetDefaultRTFormat();
				virtual DepthFormat GetDefaultDepthStencilFormat();
				//virtual void SetTexture(int index, Texture* texture);
				//virtual Texture* GetTexture(int index);

				virtual void BindVertexShader(Shader* shader);
				virtual void BindPixelShader(Shader* shader);

				virtual void Render(Material* mtrl, const RenderOperation* op, int count, int passSelID);

				virtual Viewport getViewport();
				virtual void setViewport(const Viewport& vp);

				virtual Capabilities* const getCapabilities() const;

				D3D9Capabilities* getD3D9Capabilities() const { return m_caps; }
				NativeD3DStateManager* getNativeStateManager() const { return m_nativeState; }
			private:
				friend class VolatileResource;

				Effect* m_defaultEffect;
				GraphicsDeviceManager* m_devManager;
				D3D9RenderStateManager* m_stateManager;
				NativeD3DStateManager* m_nativeState;

				D3D9RenderTarget** m_cachedRenderTarget;

				IDirect3DSurface9* m_defaultRT;
				IDirect3DSurface9* m_defaultDS;

				D3D9Capabilities* m_caps;

				List<VolatileResource*> m_volatileResources;

				D3D9InstancingData* m_instancingData;

				D3D9VertexShader* m_currentVS;
				D3D9PixelShader* m_currentPS;


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

			class D3D9Capabilities : public Capabilities
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

				virtual bool SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat);
				virtual bool SupportsPixelShader(const char* implType, int majorVer, int minorVer);
				virtual bool SupportsVertexShader(const char* implType, int majorVer, int minorVer);

				virtual void EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes);
				virtual const String* FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat);
				
				virtual int GetMRTCount();

				const AAProfile* LookupAAProfile(const String& name, PixelFormat pixFormat, DepthFormat depthFormat);

				bool FindCompatibleTextureFormat(PixelFormat& format);
				bool FindCompatibleTextureDimension(int32& width, int32& height, int32& miplevels);
				bool FindCompatibleCubeTextureDimension(int32& length, int32& miplevels);
				bool FindCompatibleVolumeTextureDimension(int32& width, int32& height, int32& depth, int32& miplevels);


			private:
				typedef HashMap<String, AAProfile> ProfileTable;
				typedef HashMap<uint32, ProfileTable*> ProfileCacheTable;

				ProfileTable* EnsureCurrentAAModes(PixelFormat pixFormat, DepthFormat depthFormat);
				ProfileTable* GenerateSupportedAAModes(const DeviceSettings* setting, PixelFormat pixFormat, DepthFormat depthFormat);

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