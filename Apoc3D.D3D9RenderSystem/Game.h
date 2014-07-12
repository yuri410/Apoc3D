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
				/** When derived, this method should be overrided to initialize the graphics device,
				 *  with the specific parameters and settings provided.
				 *  As GraphicsDeviceManager has creates the device, Game::LoadContent and Game::Initialize
				 *  will be called.
				 */
				virtual void Create(const RenderParameters& params);
				
				/** This method will ask the GraphicsDeviceManager to release resources. And 
				 *  will cause the Game::UnloadContent to be called.
				 */
				virtual void Release();

				CancellableEventHandler* eventFrameStart() { return &m_eFrameStart; }
				EventHandler* eventFrameEnd() { return &m_eFrameEnd; }		

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_graphicsDeviceManager; }
				GameWindow* getWindow() const { return m_gameWindow; }
				D3DDevice* getDevice() const;
				bool getIsExiting() const { return m_exiting; }
				bool getIsActive() const { return m_active; }

				virtual void Initialize() = 0;
				virtual void LoadContent() = 0;
				virtual void OnDeviceLost() = 0;
				virtual void UnloadContent() = 0;
				virtual void OnDeviceReset() = 0;
				/**
				 * This should be overrided to draw a frame
				 */
				virtual void Render(const GameTime* const time) = 0;
				/**
				 * This should be overrided to allow the game to run logic such as updating the world,
				 *  checking for collisions, gathering input, playing audio and etc.
				 */
				virtual void Update(const GameTime* const time) = 0;
				/**
				 * Enters the main loop. 
				 */
				void Run();
				void Exit();


			protected:

				Game(const String& name, IDirect3D9* d3d9);
				virtual ~Game(void);
				
				virtual bool OnFrameStart();
				virtual void OnFrameEnd();

			private:

				void DrawFrame(const GameTime* const time);

				void Window_ApplicationActivated();
				void Window_ApplicationDeactivated();
				void Window_Suspend();
				void Window_Resume();
				void Window_Paint();
				/**
				 *  Run one frame, which includes one render and X updates
				 */
				void Tick();

				GraphicsDeviceManager* m_graphicsDeviceManager;
				GameWindow* m_gameWindow;

				int m_maxSkipFrameCount;
				GameClock* m_gameClock;
				float m_maxElapsedTime;
				float m_totalGameTime;
				float m_accumulatedElapsedGameTime;
				float m_lastFrameElapsedGameTime;
				float m_lastFrameElapsedRealTime;
				float m_targetElapsedTime;
				float m_inactiveSleepTime;
				int32 m_updatesSinceRunningSlowly1;
				int32 m_updatesSinceRunningSlowly2;
				bool m_forceElapsedTimeToZero;
				bool m_drawRunningSlowly;
				int64 m_lastUpdateFrame;
				float m_lastUpdateTime;
				float m_fps;

				bool m_exiting;
				bool m_active;


				CancellableEventHandler m_eFrameStart;
				EventHandler m_eFrameEnd;


			};
		}
	}

}

#endif