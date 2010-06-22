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

#pragma once

#include "Common.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	class _Export Game
	{
	private:
		HINSTANCE m_inst;

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

		//bool m_exiting;
		bool m_active;

		GraphicsDeviceManager* m_graphicsDeviceManager;
		GameWindow* m_gameWindow;

		DelegateEvent<CancellableEventHandler> m_eFrameStart;
		DelegateEvent<EventHandler> m_eFrameEnd;

		void DrawFrame();

		void Window_ApplicationActivated();
		void Window_ApplicationDeactivated();
		void Window_Suspend();
		void Window_Resume();
		void Window_Paint();

	protected:
		
		virtual bool OnFrameStart();
		virtual void OnFrameEnd();

	public:
		DelegateEvent<CancellableEventHandler> &eventFrameStart() const { return m_eFrameStart; }
		const DelegateEvent<EventHandler> &eventFrameEnd() const { return m_eFrameEnd; }

		GraphicsDeviceManager* getGraphicsDeviceManager() const { return m_graphicsDeviceManager; }
		GameWindow* getWindow() const { return m_gameWindow; }
		//bool getIsExiting() const { return m_exiting; }
		bool getIsActive() const { return m_active; }

		virtual void Initialize() = 0;
		virtual void LoadContent() = 0;
		virtual void UnloadContent() = 0;
		virtual void Render() = 0;
		virtual void Update(const GameTime* time) = 0;
		void Run();
		void Tick();

		Game(HINSTANCE instance, int nCmdShow, const wchar_t* const &name);
		~Game(void);
	};

}

#endif