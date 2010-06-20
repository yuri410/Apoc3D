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
		uint64 m_baseRealTime;
        uint64 m_lastRealTime;
        bool m_lastRealTimeValid;
        uint32 m_suspendCount;
        uint64 m_suspendStartTime;
        uint64 m_timeLostToSuspension;
        float m_currentTimeBase;
        float m_currentTimeOffset;

		float m_elapsedTime;
		float m_elapsedAdjustedTime;

		uint64 m_frequency;
	public:
		float getElapsedTime() const { return m_elapsedTime; }
		float getElapsedAdjustedTime() const { return m_elapsedAdjustedTime; }
		float getCurrentTime() const { return m_currentTimeBase + m_currentTimeOffset; }

		uint64 GetCounter()
		{
			LARGE_INTEGER counter;
			assert(!QueryPerformanceCounter(&counter));
			return counter.QuadPart;
		}

		uint64 GetFrequency()
		{
			LARGE_INTEGER freq;
			assert(!QueryPerformanceFrequency(&freq));

			return freq.QuadPart;
		}

		bool CounterToTimeSpan(uint64 counter, uint64 base, float* result) const
		{
			if (base > counter)
				return false;
			*result = (counter - base) / (float)m_frequency;
			return true;
		}

        void Reset()
        {
            m_currentTimeBase = 0;
            m_currentTimeOffset = 0;
            m_baseRealTime = GetCounter();
            m_lastRealTimeValid = false;
			m_frequency = GetFrequency();
        }

        void Suspend()
        {
            m_suspendCount++;
            if (m_suspendCount == 1)
                m_suspendStartTime = GetCounter();
        }

        /// <summary>
        /// Resumes a previously suspended clock.
        /// </summary>
        void Resume()
        {
            m_suspendCount--;
            if (m_suspendCount <= 0)
            {
                m_timeLostToSuspension += GetCounter() - m_suspendStartTime;
                m_suspendStartTime = 0;
            }
        }


        void Step()
        {
			uint64 counter = GetCounter();
			

            if (!m_lastRealTimeValid)
            {
                m_lastRealTime = counter;
                m_lastRealTimeValid = true;
            }

            

			if (!CounterToTimeSpan(counter, m_baseRealTime, &m_currentTimeOffset))
			{
				// update the base value and try again to adjust for overflow
				m_currentTimeBase += m_currentTimeOffset;
				m_baseRealTime = m_lastRealTime;

				if (!CounterToTimeSpan(counter, m_baseRealTime, &m_currentTimeOffset))
				{
					m_baseRealTime = counter;
					m_currentTimeOffset = 0;
				}
			}
            
			if (!CounterToTimeSpan(counter, m_lastRealTime, &m_elapsedTime))
			{
				m_elapsedTime = 0;
			}

			if (CounterToTimeSpan(counter, (m_lastRealTime + m_timeLostToSuspension), &m_elapsedAdjustedTime))
			{
				m_timeLostToSuspension = 0;
			}
			else
			{
				m_elapsedAdjustedTime = 0;
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