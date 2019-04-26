#pragma once

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

#ifndef GL3RENDERDEVICE_H
#define GL3RENDERDEVICE_H

#include "GL3Common.h"

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
		namespace GL3RenderSystem
		{
			class GL3Capabilities;

			class GL3RenderDevice : public RenderDevice
			{
			public:

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_devManager; } 


				
				bool isInitialized() const { return !!m_stateManager; }



				GL3RenderDevice(GraphicsDeviceManager* devManager);
				~GL3RenderDevice();
				
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


				NativeGL3StateManager* getNativeState() const { return m_nativeState; }
			private:
				friend class VolatileResource;

				Effect* m_defaultEffect;
				GraphicsDeviceManager* m_devManager;
				GL3RenderStateManager* m_stateManager;
				NativeGL3StateManager* m_nativeState;

				GL3RenderTarget** m_cachedRenderTarget;

				//IDirect3DSurface9* m_defaultRT;
				//IDirect3DSurface9* m_defaultDS;

				GL3Capabilities* m_caps;

			};

			class GL3Capabilities : public Capabilities
			{
			public:
				GL3Capabilities(GL3RenderDevice* device)
					: m_device(device)
				{

				}

				virtual bool SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat);
				virtual bool SupportsPixelShader(int majorVer, int minorVer);
				virtual bool SupportsVertexShader(int majorVer, int minorVer);

				virtual int GetMRTCount();
			private:
				GL3RenderDevice* m_device;
			};
		}
	}
}

#endif