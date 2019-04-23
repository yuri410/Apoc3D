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
#ifndef GL1RENDERDEVICE_H
#define GL1RENDERDEVICE_H

#include "GL1Common.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"

using namespace std;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class GL1Capabilities;

			class GL1RenderDevice : public RenderDevice
			{
			public:

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_devManager; } 


				
				bool isInitialized() const { return !!m_stateManager; }



				GL1RenderDevice(GraphicsDeviceManager* devManager);
				~GL1RenderDevice();
				
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


				NativeGL1StateManager* getNativeState() const { return m_nativeState; }
			private:
				friend class VolatileResource;

				Effect* m_defaultEffect;
				GraphicsDeviceManager* m_devManager;
				GL1RenderStateManager* m_stateManager;
				NativeGL1StateManager* m_nativeState;

				GL1RenderTarget** m_cachedRenderTarget;

				//IDirect3DSurface9* m_defaultRT;
				//IDirect3DSurface9* m_defaultDS;

				GL1Capabilities* m_caps;

			};

			class GL1Capabilities : public Capabilities
			{
			public:
				GL1Capabilities(GL1RenderDevice* device)
					: m_device(device)
				{

				}

				virtual bool SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat);
				virtual bool SupportsPixelShader(int majorVer, int minorVer);
				virtual bool SupportsVertexShader(int majorVer, int minorVer);

				virtual int GetMRTCount();
			private:
				GL1RenderDevice* m_device;
			};
		}
	}
}

#endif