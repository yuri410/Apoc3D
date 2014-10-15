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
#include "Game.h"

#include "GameClock.h"
#include "GraphicsDeviceManager.h"
#include "GameWindow.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"

#include "apoc3d/Utility/StringUtils.h"

#include "D3D9RenderDevice.h"
#include "D3D9RenderWindow.h"

//using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			Game::Game(D3D9RenderWindow* wnd, const String& name, IDirect3D9* d3d9)
				: m_target(wnd),
				m_maxElapsedTime(0.5f), m_inactiveSleepTime(20),
				m_updatesSinceRunningSlowly1(MAXINT32), m_updatesSinceRunningSlowly2(MAXINT32),
				m_accumulatedElapsedGameTime(0), m_lastFrameElapsedGameTime(0), m_lastFrameElapsedRealTime(0),
				m_totalGameTime(0), m_forceElapsedTimeToZero(false), m_drawRunningSlowly(false), m_lastUpdateFrame(0),
				m_lastUpdateTime(0), m_fps(0),
				m_maxSkipFrameCount(10)
			{
				m_gameClock = new GameClock();

				m_gameWindow = new GameWindow(L"d5325676b0844be1a06964bc3f6603ec", name);
				m_gameWindow->eventApplicationActivated.Bind(this, &Game::Window_ApplicationActivated);
				m_gameWindow->eventApplicationDeactivated.Bind(this, &Game::Window_ApplicationDeactivated);
				m_gameWindow->eventPaint.Bind(this, &Game::Window_Paint);
				m_gameWindow->eventResume.Bind(this, &Game::Window_Resume);
				m_gameWindow->eventSuspend.Bind(this, &Game::Window_Suspend);

				m_graphicsDeviceManager = new GraphicsDeviceManager(this, d3d9);
			}
			Game::~Game()
			{
				m_gameWindow->eventApplicationActivated.Unbind(this, &Game::Window_ApplicationActivated);
				m_gameWindow->eventApplicationDeactivated.Unbind(this, &Game::Window_ApplicationDeactivated);
				m_gameWindow->eventPaint.Unbind(this, &Game::Window_Paint);
				m_gameWindow->eventResume.Unbind(this, &Game::Window_Resume);
				m_gameWindow->eventSuspend.Unbind(this, &Game::Window_Suspend);

				delete m_graphicsDeviceManager;
				delete m_gameWindow;

				delete m_gameClock;
			}


			void Game::Create(const RenderParameters& params)
			{
				m_gameWindow->Load(params.BackBufferWidth, params.BackBufferHeight, params.IsFixedWindow);

				D3D9RenderDevice* device = new D3D9RenderDevice(getGraphicsDeviceManager());
				m_target->setDevice(device);
				//m_window->m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);

				LogManager::getSingleton().Write(LOG_Graphics,
					L"[D3D9]Creating render window. ",
					LOGLVL_Infomation);

				getGraphicsDeviceManager()->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;

				// Initialize() and Load() are called as the device is being created.
				getGraphicsDeviceManager()->ChangeDevice(params);

				LogManager::getSingleton().Write(LOG_Graphics,
					L"[D3D9]Render window created. ",
					LOGLVL_Infomation);
			}

			void Game::Release()
			{
				m_graphicsDeviceManager->ReleaseDevice();
				m_target->OnFinalize();
			}


			void Game::Initialize()
			{
				D3DADAPTER_IDENTIFIER9 did;
				getGraphicsDeviceManager()->getDirect3D()->GetAdapterIdentifier(getGraphicsDeviceManager()->getCurrentSetting()->AdapterOrdinal, NULL, &did);
				m_target->m_hardwareName = StringUtils::toPlatformWideString(did.Description);

				// The window will be only initialized once, even in some cases, like device lost
				// when this is called again.
				if (!((D3D9RenderDevice*)m_target->getRenderDevice())->isInitialized())
				{
					m_target->getRenderDevice()->Initialize();
					m_target->OnInitialize(); // will make the event handler interface to process the event
				}
			}
			void Game::LoadContent()
			{
				m_target->OnLoad();

			}
			void Game::UnloadContent() 
			{
				m_target->OnUnload();
			}
			void Game::OnDeviceLost() 
			{
				D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_target->getRenderDevice());
				if (device)
					device->OnDeviceLost();
			}
			void Game::OnDeviceReset()
			{
				D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_target->getRenderDevice());
				if (device)
					device->OnDeviceReset();
			}



			D3DDevice* Game::getDevice() const { return m_graphicsDeviceManager->getDevice(); }

			bool Game::OnFrameStart()
			{
				bool re = false;
				eventFrameStart.Invoke(&re);
				if (re)
				{
					m_target->OnFrameStart();
				}
				return re;
			}

			void Game::OnFrameEnd()
			{
				m_target->OnFrameEnd();

				eventFrameEnd.Invoke();
			}

			void Game::Render(const GameTime* time)
			{
				m_target->OnDraw(time);
			}
			
			void Game::Update(const GameTime* time)
			{
				m_target->OnUpdate(time);
			}


			void Game::DrawFrame(const GameTime* time)
			{
				if (!m_gameWindow->getIsMinimized())
				{
					if (!OnFrameStart())
					{
						Render(time);
						OnFrameEnd();
					}
				}

				m_lastFrameElapsedGameTime = 0;
				m_lastFrameElapsedRealTime = 0;
			}
			void Game::Exit()
			{
				m_exiting = true;
				m_gameWindow->Close();
			}

			void Game::MainLoop()
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
						Tick();
					}
				}
			}

			void Game::Tick()
			{
				if (m_exiting)
					return;

				if (!m_active)
					Sleep(static_cast<int>(m_inactiveSleepTime));

				m_gameClock->Step();

				float elapsedRealTime = (float)m_gameClock->getElapsedTime();
				float totalRealTime = (float)m_gameClock->getCurrentTime();

				m_lastFrameElapsedRealTime += (float)m_gameClock->getElapsedTime();

				float elapsedAdjustedTime = m_gameClock->getElapsedAdjustedTime();
				if (elapsedAdjustedTime < 0)
					elapsedAdjustedTime = 0;

				if (m_forceElapsedTimeToZero)
				{
					elapsedRealTime = 0;
					m_lastFrameElapsedRealTime = elapsedAdjustedTime = 0;
					m_forceElapsedTimeToZero = false;
				}

				m_accumulatedElapsedGameTime += elapsedAdjustedTime;

				float targetElapsedTime = TargetElapsedTime;
				float ratio = m_accumulatedElapsedGameTime / TargetElapsedTime;


				m_accumulatedElapsedGameTime = fmod(m_accumulatedElapsedGameTime, targetElapsedTime);
				m_lastFrameElapsedGameTime = 0;

				if (ratio == 0)
					return;


				if (ratio > 1)
				{
					m_updatesSinceRunningSlowly2 = m_updatesSinceRunningSlowly1;
					m_updatesSinceRunningSlowly1 = 0;
				}
				else
				{
					if (m_updatesSinceRunningSlowly1 < MAXINT32)
						m_updatesSinceRunningSlowly1++;
					if (m_updatesSinceRunningSlowly2 < MAXINT32)
						m_updatesSinceRunningSlowly2++;
				}

				m_drawRunningSlowly = m_updatesSinceRunningSlowly2 < 20;

#if _DEBUG
				if (ratio>m_maxSkipFrameCount)
					ratio=1;
#else
				if (ratio>m_maxSkipFrameCount)
					ratio= static_cast<float>(m_maxSkipFrameCount);
#endif

				// keep up update calls:
				// update until it's time to draw the next frame
				while (ratio > 1)
				{
					ratio -= 1;

					//try
					{
						GameTime gt(TargetElapsedTime, m_totalGameTime,
							elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
						Update(&gt);
					}
					//catch (Apoc3DException &e)
					//{
						//m_lastFrameElapsedGameTime += targetElapsedTime;
						//m_totalGameTime += targetElapsedTime;
					//}

					m_lastFrameElapsedGameTime += targetElapsedTime;
					m_totalGameTime += targetElapsedTime;

				}

				{
					GameTime gt(TargetElapsedTime, m_totalGameTime,
						elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
					DrawFrame(&gt);
				}
				

				// refresh the FPS counter once per second
				m_lastUpdateFrame++;
				if (totalRealTime - m_lastUpdateTime > 1.0f)
				{
					m_fps = (float)m_lastUpdateFrame / (float)(totalRealTime - m_lastUpdateTime);
					m_lastUpdateTime = totalRealTime;
					m_lastUpdateFrame = 0;
				}
			}

			void Game::Window_ApplicationActivated()
			{
				m_active = true;
			}
			void Game::Window_ApplicationDeactivated()
			{
				m_active = false;
			}
			void Game::Window_Suspend()
			{
				m_gameClock->Suspend();
			}
			void Game::Window_Resume()
			{
				m_gameClock->Resume();
			}
			void Game::Window_Paint()
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