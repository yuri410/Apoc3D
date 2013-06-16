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
#ifndef APOC3D_D3D9_GAMECLOCK_H
#define APOC3D_D3D9_GAMECLOCK_H

#include "D3D9Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/**
			 *  A clock used to measure time with good accuracy
			 */
			class GameClock
			{
			public:
				GameClock(void)
				{
					Reset();
				}

				~GameClock(void)
				{
				}


				void Reset()
				{
					m_timeLostToSuspension = 0;
					m_suspendStartTime = 0;
					m_suspendCount = 0;
					m_baseRealTime = 0;
					m_lastRealTime = 0;


					m_currentTimeBase = 0;
					m_currentTimeOffset = 0;
					m_baseRealTime = GetCounter();
					m_lastRealTimeValid = false;
					m_frequency = GetFrequency();
				}
				/**
				 *  Suspends the clock.
				 */
				void Suspend()
				{
					m_suspendCount++;
					if (m_suspendCount == 1)
						m_suspendStartTime = GetCounter();
				}

				/* 
				 * Resumes a previously suspended clock.
				 */
				void Resume()
				{
					m_suspendCount--;
					if (m_suspendCount <= 0)
					{
						m_timeLostToSuspension += GetCounter() - m_suspendStartTime;
						m_suspendStartTime = 0;
					}
				}
				/**
				 *  Called every frame to get the time difference
				 */
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

				float getElapsedTime() const { return m_elapsedTime; }
				float getElapsedAdjustedTime() const { return m_elapsedAdjustedTime; }
				float getCurrentTime() const { return m_currentTimeBase + m_currentTimeOffset; }
			private:
				
				bool CounterToTimeSpan(uint64 counter, uint64 base, float* result) const
				{
					// this checks overflow
					if (base > counter)
						return false;
					*result = (counter - base) / (float)m_frequency;
					return true;
				}
				uint64 GetCounter()
				{
					LARGE_INTEGER counter;
					BOOL res = QueryPerformanceCounter(&counter);
					assert(res);
					return counter.QuadPart;
				}

				uint64 GetFrequency()
				{
					LARGE_INTEGER freq;
					BOOL res = QueryPerformanceFrequency(&freq); 
					assert(res);

					return freq.QuadPart;
				}

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
			};
		}
	}
};
#endif