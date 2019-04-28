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

#ifndef D3D9RENDERWINDOW_H
#define D3D9RENDERWINDOW_H

#include "D3D9Common.h"
#include "D3D9RenderDevice.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "apoc3d.Win32/Win32RenderWindow.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RenderView : public RenderView
			{
			public:
				D3D9RenderView(D3D9RenderDevice* device, D3D9DeviceContext* dc, D3D9RenderViewSet* viewSet, IDirect3DSwapChain9* chain, const RenderParameters& pm);
				~D3D9RenderView();

				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present() override;

			private:
				D3D9RenderViewSet* m_viewSet;
				D3D9RenderDevice* m_device;
				IDirect3DSwapChain9* m_swapChain;
				HANDLE m_controlHandle;

			};

			class D3D9RenderWindow final : public Win32RenderWindow
			{
			public:
				D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContext* dc, const RenderParameters& pm);
				~D3D9RenderWindow();

				virtual void Run() override;
				virtual void Present() override;

				void D3D9_Initialize(bool isDeviceReset);
				void D3D9_Finalize(bool isDeviceReset);

				void D3D9_OnDeviceLost();
				void D3D9_OnDeviceReset();

				void D3D9_LoadContent();
				void D3D9_UnloadContent();

				const String& getHardwareName() const { return m_hardwareName; }

			private:

				/** initialize the graphics device,
				 *  with the specific parameters and settings provided.
				 *  As GraphicsDeviceManager has creates the device, Game::LoadContent and Game::Initialize
				 *  will be called.
				 */
				void D3D9_Create(const RenderParameters& params);

				/** This method will ask the GraphicsDeviceManager to release resources. And
				 *  will cause the Game::UnloadContent to be called.
				 */
				void D3D9_Release();

				bool D3D9_OnFrameStart();
				void D3D9_OnFrameEnd();

				void OnRenderFrame(const GameTime* time) override;

				bool IsDeviceReady() override;
				void ExecuteChangeDevice() override;
				
				D3D9DeviceContext* m_dc;
				String m_hardwareName;

				GraphicsDeviceManager* m_graphicsDeviceManager;
			
			};
		}
	}
}

#endif