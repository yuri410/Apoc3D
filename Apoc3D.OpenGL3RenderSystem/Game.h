#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GAME_H
#define GAME_H

#include "GL3Common.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;
using namespace fastdelegate;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
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
				

				Win32GameWindow* getWindow() const { return m_gameWindow; }

				bool getIsExiting() const { return m_exiting; }
				bool getIsActive() const { return m_active; }

				virtual void Initialize() = 0;
				virtual void LoadContent() = 0;
				virtual void UnloadContent() = 0;
				/** This should be overrided to draw a frame */
				virtual void Render(const GameTime* const time) = 0;
				/** This should be overrided to allow the game to run logic such as updating the world,
				 *  checking for collisions, gathering input, playing audio and etc.
				 */
				virtual void Update(const GameTime* const time) = 0;
				void Run();
				void Tick();
				void Exit();

				CancellableEventHandler eventFrameStart;
				EventHandler eventFrameEnd;

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


				void DrawFrame(const GameTime* const time);

				void Window_ApplicationActivated();
				void Window_ApplicationDeactivated();
				void Window_Suspend();
				void Window_Resume();
				void Window_Paint();

			protected:

				Game(GL3DeviceContext* devCont, const String& name);
				virtual ~Game();
				virtual bool OnFrameStart();
				virtual void OnFrameEnd();


			};
		}
	}

}

#endif