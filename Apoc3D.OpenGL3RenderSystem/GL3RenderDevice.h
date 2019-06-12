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
				GL3RenderDevice(GraphicsDeviceManager* devManager);
				~GL3RenderDevice();
				
				virtual void Initialize();
				
				virtual void BeginFrame();
				virtual void EndFrame();
				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil);

				virtual void SetRenderTarget(int32 index, RenderTarget* rt) override;
				virtual RenderTarget* GetRenderTarget(int32 index) override;

				virtual void SetDepthStencilBuffer(DepthStencilBuffer* buf) override;
				virtual DepthStencilBuffer* GetDepthStencilBuffer() override;

				virtual PixelFormat GetDefaultRTFormat();
				virtual DepthFormat GetDefaultDepthStencilFormat();


				virtual void BindVertexShader(Shader* shader);
				virtual void BindPixelShader(Shader* shader);

				virtual void Render(Material* mtrl, const RenderOperation* op, int count, int passSelID);

				virtual Viewport getViewport();
				virtual void setViewport(const Viewport& vp);

				virtual Capabilities* getCapabilities() const;

				virtual uint32 GetAvailableVideoRamInMB() override;

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_devManager; } 
				NativeGL3StateManager* getNativeState() const { return m_nativeState; }

				bool isInitialized() const { return !!m_stateManager; }

				void NotifyDestruction(GL3Shader* shader);
				void NotifyDestruction(GL3RenderTarget* rt);
				void NotifyDestruction(GL3DepthStencilBuffer* dsb);

				// flush deferred pipeline states
				GLProgram* PostBindShaders();
				void PostBindRenderTargets();

			private:

				Effect* m_defaultEffect = nullptr;
				GraphicsDeviceManager* m_devManager = nullptr;
				GL3RenderStateManager* m_stateManager = nullptr;
				NativeGL3StateManager* m_nativeState = nullptr;

				int32 m_maxRenderTargets = 0;
				GL3RenderTarget** m_renderTargets = nullptr;
				GL3DepthStencilBuffer* m_depthStencilBuffer = nullptr;
				
				GL3Shader* m_vertexShader = nullptr;
				GL3Shader* m_pixelShader = nullptr;
				
				bool m_shaderDirty = false;
				bool m_renderTargetDirty = false;

				GL3Capabilities* m_caps = nullptr;

			private:
				struct RtKey
				{
				public:
					RtKey() { }

					RtKey(const RtKey&) = delete;
					RtKey& operator=(const RtKey&) = delete;

					RtKey(RtKey&&) = default;
					RtKey& operator=(RtKey&&) = default;

					List<GLuint> m_colorBuffers;
					GLuint m_depthStencilBuffer = 0;
				};

				struct RtKeyComparer
				{
					static bool Equals(const RtKey& x, const RtKey& y);

					static int32 GetHashCode(const RtKey& obj);
				};

				HashMap<RtKey, GLFramebuffer, RtKeyComparer> m_cachedFbo;
			};

			class GL3Capabilities : public Capabilities
			{
			public:
				GL3Capabilities(GL3RenderDevice* device);

				bool SupportsRenderTarget(const String& multisampleMode, PixelFormat pixFormat, DepthFormat depthFormat) override;
				bool SupportsPixelShader(const char* implType, int majorVer, int minorVer) override;
				bool SupportsVertexShader(const char* implType, int majorVer, int minorVer) override;

				void EnumerateRenderTargetMultisampleModes(PixelFormat pixFormat, DepthFormat depthFormat, Apoc3D::Collections::List<String>& modes) override;
				const String* FindClosesetMultisampleMode(uint32 sampleCount, PixelFormat pixFormat, DepthFormat depthFormat) override;

				int32 GetMRTCount() override;
				bool SupportsMRTDifferentBits() override;
				bool SupportsMRTWriteMasks() override;

			private:
				GL3RenderDevice* m_device;
				GLint m_majorGlVer;
				GLint m_minorGlVer;
				GLint m_majorGlslVer;
				GLint m_minorGlslVer;
			};
		}
	}
}

#endif