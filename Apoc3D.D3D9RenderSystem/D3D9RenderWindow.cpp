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

#include "D3D9RenderWindow.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"
#include "D3D9DeviceContext.h"
#include "Enumeration.h"
#include "D3D9RenderViewSet.h"
#include "RawSettings.h"

#include "apoc3d.Win32/Win32Window.h"
#include "apoc3d.Win32/Win32Clock.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RenderView::D3D9RenderView(D3D9RenderDevice* device, D3D9DeviceContext* dc, D3D9RenderViewSet* viewSet, IDirect3DSwapChain9* chain, const RenderParameters& pm)
				: RenderView(dc, device, pm), m_viewSet(viewSet),  m_swapChain(chain), m_device(NULL)//, m_deviceLost(false)
			{
				m_controlHandle = reinterpret_cast<HANDLE>(pm.TargetHandle);
			}
			D3D9RenderView::~D3D9RenderView()
			{
				m_swapChain->Release();
			}

			void D3D9RenderView::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderParameters p2 = params;
				p2.IsWindowed = true;
				p2.RefreshRate = 0;

				m_viewSet->ChangeDevice(p2, 0);
			}
			void D3D9RenderView::Present()
			{
				m_swapChain->Present(NULL,NULL,NULL,NULL, NULL);
			}

			/************************************************************************/
			/*   D3D9RenderWindow                                                   */
			/************************************************************************/
			
			D3D9RenderWindow::D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContext* dc, const RenderParameters& pm)
				: Win32RenderWindow(dc, device, pm), m_dc(dc)
			{
				m_graphicsDeviceManager = new GraphicsDeviceManager(this, dc->getD3D());
			}
			D3D9RenderWindow::~D3D9RenderWindow()
			{
				// clean up traces.
				m_dc->NotifyWindowClosed(this);

				delete m_graphicsDeviceManager;
			}

			void D3D9RenderWindow::Run()
			{
				// This Run method is called from outside client code to create a render window
				// for the first time and right after when the constructor is called. No other methods are
				// needed. 
				
				// Creates almost every thing
				D3D9_Create(getRenderParams());

				MainLoop();
				// Releases almost every thing
				D3D9_Release();
			}

			void D3D9RenderWindow::Present()
			{
				
			}

			//////////////////////////////////////////////////////////////////////////

			void D3D9RenderWindow::D3D9_Create(const RenderParameters& params)
			{
				m_gameWindow->Load(params.BackBufferWidth, params.BackBufferHeight, params.IsFixedWindow);

				m_renderDevice = new D3D9RenderDevice(m_graphicsDeviceManager);
				//m_window->m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);

				ApocLog(LOG_Graphics, L"[D3D9]Creating render window.", LOGLVL_Infomation);

				m_graphicsDeviceManager->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;

				// Initialize() and Load() are called as the device is being created.
				m_graphicsDeviceManager->ChangeDevice(params);

				ApocLog(LOG_Graphics, L"[D3D9]Render window created.", LOGLVL_Infomation);
			}

			void D3D9RenderWindow::D3D9_Release()
			{
				// Unload() and Finalize() will be called here
				m_graphicsDeviceManager->ReleaseDevice(false);
				delete m_renderDevice;
			}

			void D3D9RenderWindow::D3D9_Initialize(bool isDeviceReset)
			{
				D3DADAPTER_IDENTIFIER9 did;
				m_graphicsDeviceManager->getDirect3D()->GetAdapterIdentifier(m_graphicsDeviceManager->getCurrentSetting()->AdapterOrdinal, NULL, &did);
				m_hardwareName = StringUtils::toPlatformWideString(did.Description);

				if (!isDeviceReset)
				{
					// The window will be only initialized once, even in some cases, like device lost
					// when this is called again.
					D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_renderDevice);

					assert(!device->isInitialized());

					device->Initialize();
					OnInitialize(); // will make the event handler interface to process the event
				}
			}

			void D3D9RenderWindow::D3D9_Finalize(bool isDeviceReset)
			{
				if (!isDeviceReset)
					OnFinalize();
			}

			void D3D9RenderWindow::D3D9_LoadContent()   { OnLoad(); }
			void D3D9RenderWindow::D3D9_UnloadContent() { OnUnload(); }

			void D3D9RenderWindow::D3D9_OnDeviceLost()
			{
				D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_renderDevice);
				if (device)
					device->OnDeviceLost();
			}

			void D3D9RenderWindow::D3D9_OnDeviceReset()
			{
				D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_renderDevice);
				if (device)
					device->OnDeviceReset();
			}

			void D3D9RenderWindow::OnRenderFrame(const GameTime* time)
			{
				if (!m_gameWindow->getIsMinimized())
				{
					if (!D3D9_OnFrameStart())
					{
						OnDraw(time);
						D3D9_OnFrameEnd();
					}
				}
			}

			bool D3D9RenderWindow::D3D9_OnFrameStart()
			{
				if (!m_graphicsDeviceManager->FrameStart())
				{
					OnFrameStart();
					return false;
				}
				return true;
			}

			void D3D9RenderWindow::D3D9_OnFrameEnd()
			{
				OnFrameEnd();
				m_graphicsDeviceManager->FrameEnd();
			}

			bool D3D9RenderWindow::IsDeviceReady()
			{
				return m_graphicsDeviceManager->IsDeviceReady();
			}

			void D3D9RenderWindow::ExecuteChangeDevice()
			{
				const RenderParameters& params = getRenderParams();
				m_gameWindow->MakeFixedSize(params.IsFixedWindow);

				m_graphicsDeviceManager->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;
				m_graphicsDeviceManager->ChangeDevice(params);
			}
		}
	}
}