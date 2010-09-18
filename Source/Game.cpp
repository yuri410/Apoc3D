/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#include "GameTime.h"
#include "GameClock.h"
#include "Graphics\GraphicsDeviceManager.h"
#include "GameWindow.h"
#include "Apoc3DException.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	Game::Game(HINSTANCE instance, int nCmdShow, const wchar_t* const &name)
		: m_maxElapsedTime(0.5f), m_targetElapsedTime(1.0f / 60.0f), m_inactiveSleepTime(20),
		m_updatesSinceRunningSlowly1(MaxInt32), m_updatesSinceRunningSlowly2(MaxInt32),
		m_exiting(false),
		m_accumulatedElapsedGameTime(0), m_lastFrameElapsedGameTime(0), m_lastFrameElapsedRealTime(0),
		m_totalGameTime(0), m_forceElapsedTimeToZero(false), m_drawRunningSlowly(false), m_lastUpdateFrame(0),
		m_lastUpdateTime(0), m_fps(0), m_inst(instance)
	{
		m_gameClock = new GameClock();

		m_gameWindow = new GameWindow(instance, nCmdShow, name, name);
		m_gameWindow->eventApplicationActivated()->bind(this, &Game::Window_ApplicationActivated);
		m_gameWindow->eventApplicationDeactivated()->bind(this, &Game::Window_ApplicationDeactivated);
		m_gameWindow->eventPaint()->bind(this, &Game::Window_Paint);
		m_gameWindow->eventResume()->bind(this, &Game::Window_Resume);
		m_gameWindow->eventSuspend()->bind(this, &Game::Window_Suspend);

		m_graphicsDeviceManager = new GraphicsDeviceManager(this);
	}
	void Game::Release()
	{
		m_graphicsDeviceManager->ReleaseDevice();
	}
	void Game::Create()
	{
		m_gameWindow->Load();
	}
	Game::~Game()
	{		
		delete m_graphicsDeviceManager;
		delete m_gameWindow;

		delete m_gameClock;
	}

	Device* Game::getDevice() const
	{
		return m_graphicsDeviceManager->getDevice();
	}

	bool Game::OnFrameStart()
	{
		bool re = false;
		m_eFrameStart(&re);
		return re;
	}

	void Game::OnFrameEnd()
	{
		m_eFrameEnd();
	}

	void Game::DrawFrame()
	{
		try
		{
			if (!m_gameWindow->getIsMinimized())
			{
				if (!OnFrameStart())
				{
					Render();
					OnFrameEnd();
				}
			}
		}
		catch (GameException &e)
		{
			m_lastFrameElapsedGameTime = 0;
			m_lastFrameElapsedRealTime = 0;
		}
		m_lastFrameElapsedGameTime = 0;
		m_lastFrameElapsedRealTime = 0;
	}
	void Game::Exit()
	{
		m_gameWindow->Close();
	}

	void Game::Run()
	{
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT)
		{			
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
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

		float targetElapsedTime = m_targetElapsedTime;
		float ratio = m_accumulatedElapsedGameTime / m_targetElapsedTime;


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
			if (m_updatesSinceRunningSlowly1 < MaxInt32)
				m_updatesSinceRunningSlowly1++;
			if (m_updatesSinceRunningSlowly2 < MaxInt32)
				m_updatesSinceRunningSlowly2++;
		}

		m_drawRunningSlowly = m_updatesSinceRunningSlowly2 < 20;

#if _DEBUG
		if (ratio>10)
			ratio=0;
#endif

		// update until it's time to draw the next frame
		while (ratio > 1)
		{
			ratio -= 1;

			try
			{
				GameTime gt(m_targetElapsedTime, m_totalGameTime,
					elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
				Update(&gt);
			}
			catch (GameException &e)
			{
				m_lastFrameElapsedGameTime += targetElapsedTime;
				m_totalGameTime += targetElapsedTime;
			}

			m_lastFrameElapsedGameTime += targetElapsedTime;
			m_totalGameTime += targetElapsedTime;

		}

		DrawFrame();

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
		// the paint event may be raised before device init -- just created wnd class
		if (getDevice())
		{
			DrawFrame();
		}		
	}

}