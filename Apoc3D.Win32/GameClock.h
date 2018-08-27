#pragma once
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#ifndef APOC3D_WIN32_GAMECLOCK_H
#define APOC3D_WIN32_GAMECLOCK_H

#include "Win32Common.h"

namespace Apoc3D
{
	namespace Win32
	{
		/** A clock used to measure time with good accuracy */
		class GameClock
		{
		public:
			GameClock() { Reset(); }
			~GameClock() { }

			void Reset();

			/** Called every frame to get the time difference */
			void Step();

			double getElapsedTime() const { return m_elapsedTime; }
			double getCurrentTime() const { return m_currentTimeBase + m_currentTime; }

		private:
			bool CounterToTimeSpan(uint64 counter, uint64 base, double* result) const;

			uint64 m_baseCounter;
			uint64 m_lastCounter;

			uint64 m_frequency;

			double m_currentTimeBase;
			double m_currentTime;

			double m_elapsedTime;

			bool m_lastRealTimeValid;

		};
	}
};
#endif