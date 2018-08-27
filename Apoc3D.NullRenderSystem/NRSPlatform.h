#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#ifndef NRSPLATFORM_H
#define NRSPLATFORM_H

#include "NRSCommon.h"
#include "Apoc3D/Math/Point.h"
#include <chrono>

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class GameClock
			{
			public:
				GameClock() { Reset(); }
				~GameClock() { }

				void Reset();

				/** Called every frame to get the time difference */
				void Step();

				double getElapsedTime() const { return m_elapsedTime; }
				double getCurrentTime() const { return m_currentTime; }

			private:

				std::chrono::high_resolution_clock::time_point m_baseTimePoint;
				std::chrono::high_resolution_clock::time_point m_curTimePoint;
				std::chrono::high_resolution_clock::time_point m_lastTimePoint;

				double m_currentTime;
				double m_elapsedTime;
			};

			/* Implements a specialized window for games and rendering. */
			class GameWindow
			{
			public:
				GameWindow(const String& wndTitle);
				~GameWindow();

				/** Creates a win32 window using the given parameters. */
				void Load(int32 width, int32 height, bool fixed);

				void Close();

				String getWindowTitle() const { return m_title; }
				void setWindowTitle(const String& txt);
				
				/** Gets the current client size */
				Size getCurrentSize() const { return m_size; }

				/** Gets the mouse wheel amount from window message */
				bool isClosed() const { return m_isClosed; }

				void SetVisible(bool v);

				void Minimize();
				void Restore();
				void Maximize();

			private:
				static GameWindow* ms_Window;

				static void SigIntHandler(int sig);

				bool m_isClosed = false;
				String m_title;
				Size m_size;
			};

		}
	}
}
#endif