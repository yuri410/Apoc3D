#pragma once
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
#ifndef APOC3D_D3D9_GAME_H
#define APOC3D_D3D9_GAME_H

#include "D3D9Common.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Core;
using namespace fastdelegate;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/**
			 *  This implements the game loop and creates graphics device by using GraphicsDeviceManager
			 */
			class Game
			{
			public:
				Game(D3D9RenderWindow* wnd, const String& name, IDirect3D9* d3d9);
				~Game();

				/** When derived, this method should be overridden to initialize the graphics device,
				 *  with the specific parameters and settings provided.
				 *  As GraphicsDeviceManager has creates the device, Game::LoadContent and Game::Initialize
				 *  will be called.
				 */
				void Create(const RenderParameters& params);
				
				/** This method will ask the GraphicsDeviceManager to release resources. And 
				 *  will cause the Game::UnloadContent to be called.
				 */
				void Release();

				void Initialize();
				void LoadContent();
				void UnloadContent();
				void OnDeviceLost();
				void OnDeviceReset();

				/** This should be overridden to draw a frame */
				void Render(const GameTime* time);
				/** This should be overridden to allow the game to run logic such as updating the world,
				 *  checking for collisions, gathering input, playing audio and etc.
				 */
				void Update(const GameTime* time);

				/** Enters the main loop. */
				void MainLoop();
				void Exit();

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_graphicsDeviceManager; }
				GameWindow* getWindow() const { return m_gameWindow; }
				D3DDevice* getDevice() const;
				bool getIsExiting() const { return m_exiting; }
				bool getIsActive() const { return m_active; }

				float TargetElapsedTime = 1.0f / 60.0f;
				bool UseFixedTimeStep = true;

				CancellableEventHandler eventFrameStart;
				EventHandler eventFrameEnd;

			private:
				bool OnFrameStart();
				void OnFrameEnd();

				void DrawFrame(const GameTime* time);

				void Window_ApplicationActivated();
				void Window_ApplicationDeactivated();
				void Window_Suspend();
				void Window_Resume();
				void Window_Paint();

				/** Run one frame, which includes one render and X updates  */
				void Tick();

				D3D9RenderWindow* m_target;
				GraphicsDeviceManager* m_graphicsDeviceManager;
				GameWindow* m_gameWindow;
				GameClock* m_gameClock;

				int m_maxSkipFrameCount = 10;
				float m_maxElapsedTime = 0.5f;
				float m_totalGameTime = 0;
				float m_accumulatedElapsedGameTime = 0;
				float m_lastFrameElapsedGameTime = 0;
				float m_lastFrameElapsedRealTime = 0;
				
				float m_inactiveSleepTime = 20;
				int32 m_updatesSinceRunningSlowly1 = MAXINT32;
				int32 m_updatesSinceRunningSlowly2 = MAXINT32;
				int64 m_lastUpdateFrame = 0;
				float m_lastUpdateTime = 0;
				float m_fps = 0;

				bool m_forceElapsedTimeToZero = false;
				bool m_drawRunningSlowly = false;

				bool m_exiting = false;
				bool m_active = false;

			};
		}
	}

}

#endif