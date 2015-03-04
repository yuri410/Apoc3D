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

#include "D3D9RenderWindow.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"
#include "D3D9DeviceContext.h"
#include "Enumeration.h"
#include "D3D9RenderViewSet.h"
#include "RawSettings.h"

#include "apoc3d.Win32/GameWindow.h"
#include "apoc3d.Win32/GameClock.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"
#include "apoc3d/Utility/StringUtils.h"

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
			void D3D9RenderView::Present(const GameTime* time)
			{
				m_swapChain->Present(NULL,NULL,NULL,NULL, NULL);
			}

			/************************************************************************/
			/*   D3D9RenderWindow                                                   */
			/************************************************************************/
			
			D3D9RenderWindow::D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContext* dc, const RenderParameters& pm)
				: RenderWindow(dc, device, pm), m_dc(dc)
			{
				m_gameClock = new GameClock();

				m_gameWindow = new GameWindow(Win32::WindowClass, L"");
				m_gameWindow->eventApplicationActivated.Bind(this, &D3D9RenderWindow::Window_ApplicationActivated);
				m_gameWindow->eventApplicationDeactivated.Bind(this, &D3D9RenderWindow::Window_ApplicationDeactivated);
				m_gameWindow->eventPaint.Bind(this, &D3D9RenderWindow::Window_Paint);
				m_gameWindow->eventResume.Bind(this, &D3D9RenderWindow::Window_Resume);
				m_gameWindow->eventSuspend.Bind(this, &D3D9RenderWindow::Window_Suspend);

				m_graphicsDeviceManager = new GraphicsDeviceManager(this, dc->getD3D());
			}
			D3D9RenderWindow::~D3D9RenderWindow()
			{
				// clean up traces.
				m_dc->NotifyWindowClosed(this);
				//delete m_game;

				m_gameWindow->eventApplicationActivated.Unbind(this, &D3D9RenderWindow::Window_ApplicationActivated);
				m_gameWindow->eventApplicationDeactivated.Unbind(this, &D3D9RenderWindow::Window_ApplicationDeactivated);
				m_gameWindow->eventPaint.Unbind(this, &D3D9RenderWindow::Window_Paint);
				m_gameWindow->eventResume.Unbind(this, &D3D9RenderWindow::Window_Resume);
				m_gameWindow->eventSuspend.Unbind(this, &D3D9RenderWindow::Window_Suspend);

				delete m_graphicsDeviceManager;
				delete m_gameWindow;

				delete m_gameClock;
			}

			void D3D9RenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);

				m_gameWindow->MakeFixedSize(params.IsFixedWindow);

				m_graphicsDeviceManager->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;
				m_graphicsDeviceManager->ChangeDevice(params);
			}


			void D3D9RenderWindow::Exit()
			{
				RenderWindow::Exit();
				
				m_gameWindow->Close();
			}

			void D3D9RenderWindow::Run()
			{
				// This Run method is called from outside client code to create a render window
				// for the first time and right after when the constructor is called. No other methods are
				// needed. 
				
				// Creates almost every thing
				D3D9_Create(getRenderParams());

				D3D9_MainLoop();
				// Releases almost every thing
				D3D9_Release();
			}

			String D3D9RenderWindow::getTitle() { return m_gameWindow->getWindowTitle(); }
			void D3D9RenderWindow::setTitle(const String& name) { m_gameWindow->setWindowTitle(name); }

			Size D3D9RenderWindow::getClientSize() { return m_gameWindow->getCurrentSize(); }

			void D3D9RenderWindow::SetVisible(bool v)
			{
				ShowWindow(m_gameWindow->getHandle(), v ? SW_NORMAL : SW_HIDE);

				m_visisble = v;
			}
			void D3D9RenderWindow::SetupFixedFrameTime(bool enabled, float targetTime)
			{
				m_targetElapsedTime = targetTime;
				m_useFixedTimeStep = enabled;
			}

			void D3D9RenderWindow::Minimize()
			{
				if (m_visisble)
					ShowWindow(m_gameWindow->getHandle(), SW_MINIMIZE);
			}
			void D3D9RenderWindow::Restore()
			{
				if (m_visisble)
					ShowWindow(m_gameWindow->getHandle(), SW_RESTORE);
			}
			void D3D9RenderWindow::Maximize()
			{
				if (m_visisble)
					ShowWindow(m_gameWindow->getHandle(), SW_MAXIMIZE);
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

			void D3D9RenderWindow::D3D9_LoadContent() { OnLoad(); }
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

			void D3D9RenderWindow::D3D9_MainLoop()
			{
				MSG msg;
				ZeroMemory(&msg, sizeof(msg));
				while (msg.message != WM_QUIT)
				{
					if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						D3D9_Tick();
					}
				}
			}

			void D3D9RenderWindow::D3D9_Tick()
			{
				if (m_isExiting)
					return;

				if (!m_active)
					Sleep(m_inactiveSleepTime);

				m_gameClock->Step();

				float dt = (float)m_gameClock->getElapsedTime();
				float totalRealTime = (float)m_gameClock->getCurrentTime();

				if (dt < 0)
					dt = 0;

				float fps = getFPS();

				if (m_useFixedTimeStep)
				{
					m_accumulatedDt_fixedStep += dt;

					int32 maxFrameSkip = m_maxSkipFrameCount_fixedStep;
#if _DEBUG
					maxFrameSkip = 5;
#endif

					bool renderingSlow = m_accumulatedDt_fixedStep > m_targetElapsedTime * 1.2f;

					if (renderingSlow)
						m_slowRenderFrameHits_fixedStep++;
					else
						m_slowRenderFrameHits_fixedStep--;

					renderingSlow = m_slowRenderFrameHits_fixedStep > 5;

					int32 framesSkipped = 0;

					// keep up update calls:
					// update until it's time to draw the next frame
					while (m_accumulatedDt_fixedStep >= m_targetElapsedTime)
					{
						if (framesSkipped < maxFrameSkip)
						{
							GameTime gt(m_targetElapsedTime, m_totalGameTime_fixedStep,
								dt, totalRealTime, fps, renderingSlow);
							D3D9_Update(&gt);

							m_totalGameTime_fixedStep += m_targetElapsedTime;
						}

						framesSkipped++;
						m_accumulatedDt_fixedStep -= m_targetElapsedTime;
					}

					GameTime gt(m_targetElapsedTime, m_totalGameTime_fixedStep,
						dt, totalRealTime, fps, renderingSlow);
						D3D9_DrawFrame(&gt);
				}
				else
				{
					GameTime gt(dt, totalRealTime, dt, totalRealTime, fps, false);

					D3D9_Update(&gt);
					D3D9_DrawFrame(&gt);
				}
			}

			void D3D9RenderWindow::D3D9_DrawFrame(const GameTime* time)
			{
				if (!m_gameWindow->getIsMinimized())
				{
					if (!D3D9_OnFrameStart())
					{
						D3D9_Render(time);
						D3D9_OnFrameEnd();
					}
				}
			}

			void D3D9RenderWindow::D3D9_Render(const GameTime* time) { OnDraw(time); }
			void D3D9RenderWindow::D3D9_Update(const GameTime* time) { OnUpdate(time); }



			bool D3D9RenderWindow::D3D9_OnFrameStart()
			{
				bool re = false;
				eventFrameStart.Invoke(&re);
				if (!re)
					OnFrameStart();
				return re;
			}
			void D3D9RenderWindow::D3D9_OnFrameEnd()
			{
				OnFrameEnd();

				eventFrameEnd.Invoke();
			}

			void D3D9RenderWindow::Window_ApplicationActivated()
			{
				m_active = true;
			}
			void D3D9RenderWindow::Window_ApplicationDeactivated()
			{
				m_active = false;
			}
			void D3D9RenderWindow::Window_Suspend()
			{
				//m_gameClock->Suspend();
			}
			void D3D9RenderWindow::Window_Resume()
			{
				//m_gameClock->Resume();
			}
			void D3D9RenderWindow::Window_Paint()
			{
				//#if _DEBUG
				// If WM_PAINT frame have errors when debugging in windows, the error dlgs 
				// can not be displayed. 

				// The frame fails and another WM_PAINT will arrive in attempt to render the window.
				// And if the error can not be recovered,
				// WM_PAINT frames will arrive and fail again and again. Causing normal loop render unable
				// to start. And in windows error dialogs cannot display.
				return;
				//#endif
				//// the paint event may be raised before device init -- just created wnd class
				//if (getDevice())
				//{

				//	const float elapsedRealTime = (float)m_gameClock->getElapsedTime();
				//	const float totalRealTime = (float)m_gameClock->getCurrentTime();

				//	GameTime gt(m_targetElapsedTime, m_totalGameTime,
				//		elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
				//	DrawFrame(&gt);
				//}		
			}
		}
	}
}