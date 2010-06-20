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
#include "Graphics\GraphicsDeviceManager.h"
#include "GameWindow.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	Game::Game(HINSTANCE instance, int nCmdShow, const wchar_t* const &name)
		: m_maxElapsedTime(0.5f), m_targetElapsedTime(1/60.0f), m_inactiveSleepTime(20),
		  m_updatesSinceRunningSlowly1(0x7fffffff), m_updatesSinceRunningSlowly2(0x7fffffff)
	{
		m_gameWindow = new GameWindow(instance, nCmdShow, name, name);
		m_graphicsDeviceManager = new GraphicsDeviceManager(this);		
	}

	void Game::DrawFrame()
	{

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

		// step clock

		
	}


	Game::~Game(void)
	{
	}
}