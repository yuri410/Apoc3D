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
#ifndef GAMECLOCK_H
#define GAMECLOCK_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	class _Export GameClock
	{
	private:
		LARGE_INTEGER m_baseRealTime;
        LARGE_INTEGER m_lastRealTime;
        bool m_lastRealTimeValid;
        int32 m_suspendCount;
        LARGE_INTEGER m_suspendStartTime;
        LARGE_INTEGER m_timeLostToSuspension;
        float m_currentTimeBase;
        float m_currentTimeOffset;

	public:
        void Reset()
        {
            m_currentTimeBase = 0;
            m_currentTimeOffset = 0;
            m_baseRealTime = Stopwatch.GetTimestamp();
            m_lastRealTimeValid = false;
        }

        void Suspend()
        {
            m_suspendCount++;
            if (m_suspendCount == 1)
                m_suspendStartTime = Stopwatch.GetTimestamp();
        }

        /// <summary>
        /// Resumes a previously suspended clock.
        /// </summary>
        void Resume()
        {
            m_suspendCount--;
            if (m_suspendCount <= 0)
            {
                m_timeLostToSuspension += Stopwatch.GetTimestamp() - m_suspendStartTime;
                m_suspendStartTime = 0;
            }
        }

        void Step()
        {
            LARGE_INTEGER counter;
			QueryPerformanceCounter(&counter);//Stopwatch.GetTimestamp();

            if (!m_lastRealTimeValid)
            {
                m_lastRealTime = counter;
                m_lastRealTimeValid = true;
            }

            try
            {
                m_currentTimeOffset = CounterToTimeSpan(counter - m_baseRealTime);
            }
            catch (OverflowException)
            {
                // update the base value and try again to adjust for overflow
                m_currentTimeBase += m_currentTimeOffset;
                m_baseRealTime = m_lastRealTime;

                try
                {
                    // get the current offset
                    m_currentTimeOffset = CounterToTimeSpan(counter - m_baseRealTime);
                }
                catch (OverflowException)
                {
                    // account for overflow
                    m_baseRealTime = counter;
                    m_currentTimeOffset = 0;
                }
            }

            try
            {
                ElapsedTime = CounterToTimeSpan(counter - lastRealTime);
            }
            catch (OverflowException)
            {
                ElapsedTime = 0;
            }

            try
            {
                ElapsedAdjustedTime = CounterToTimeSpan(counter - (m_lastRealTime + m_timeLostToSuspension));
                m_timeLostToSuspension = 0;
            }
            catch (OverflowException)
            {
                ElapsedAdjustedTime = 0;
            }

            m_lastRealTime = counter;
        }

		GameClock(void)
		{
            Reset();
		}

		~GameClock(void)
		{
		}
	};
};
#endif