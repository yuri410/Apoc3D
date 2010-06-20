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
		: m_maxElapsedTime(0.5f), m_targetElapsedTime(1/60.0f), m_inactiveSleepTime(20),
		  m_updatesSinceRunningSlowly1(MaxInt32), m_updatesSinceRunningSlowly2(MaxInt32)
	{
		m_gameClock = new GameClock();

		m_gameWindow = new GameWindow(instance, nCmdShow, name, name);
		m_graphicsDeviceManager = new GraphicsDeviceManager(this);		
	}

	void Game::DrawFrame()
	{
		try
        {
			if (!m_exiting && !m_gameWindow->getIsMinimized())
			{
				if (OnFrameStart())
				{
					Render();
					OnFrameEnd();
				}				
			}
		}
		catch (Apoc3DException &e)
		{
			m_lastFrameElapsedGameTime = 0;
			m_lastFrameElapsedRealTime = 0;
		}
		m_lastFrameElapsedGameTime = 0;
		m_lastFrameElapsedRealTime = 0;
	}

	void Game::Run()
	{
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT )
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

		if (m_active)
			Sleep(m_inactiveSleepTime);

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

		// update until it's time to draw the next frame
		while (ratio > 0 && !m_exiting)
		{
			ratio -= 1;
			
			try
			{
				GameTime gt(m_targetElapsedTime, m_totalGameTime,
					elapsedRealTime,totalRealTime, m_fps, m_drawRunningSlowly);
				Update(&gt);
			}
			catch (Apoc3DException &e)
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


	Game::~Game(void)
	{
	}
}