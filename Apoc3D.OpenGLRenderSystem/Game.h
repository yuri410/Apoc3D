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
#ifndef GAME_H
#define GAME_H

#include "GL1Common.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;
using namespace fastdelegate;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class Game
			{
			public:
				virtual void Release();
				/** Creates RenderWindow and Game object are created.
				 *  When overrided, the derived class expected to
				 *  initialize the graphics device
				 */
				virtual void Create();

				GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_graphicsDeviceManager; }
				CancellableEventHandler* eventFrameStart() { return &m_eFrameStart; }
				EventHandler* eventFrameEnd() { return &m_eFrameEnd; }

				Win32GameWindow* getWindow() const { return m_gameWindow; }

				bool getIsExiting() const { return m_exiting; }
				bool getIsActive() const { return m_active; }

				virtual void Initialize() = 0;
				virtual void LoadContent() = 0;
				virtual void UnloadContent() = 0;
				/** This should be overrided to draw a frame
				 */
				virtual void Render(const GameTime* const time) = 0;
				/** This should be overrided to allow the game to run logic such as updating the world,
				 *  checking for collisions, gathering input, playing audio and etc.
				 */
				virtual void Update(const GameTime* const time) = 0;
				void Run();
				void Tick();
				void Exit();


			private:
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

				GraphicsDeviceManager* m_graphicsDeviceManager;
				Win32GameWindow* m_gameWindow;


				CancellableEventHandler m_eFrameStart;
				EventHandler m_eFrameEnd;

				void DrawFrame(const GameTime* const time);

				void Window_ApplicationActivated();
				void Window_ApplicationDeactivated();
				void Window_Suspend();
				void Window_Resume();
				void Window_Paint();

			protected:

				Game(GL1DeviceContent* devCont, const wchar_t* const &name);
				virtual ~Game(void);
				virtual bool OnFrameStart();
				virtual void OnFrameEnd();


			};
		}
	}

}

#endif