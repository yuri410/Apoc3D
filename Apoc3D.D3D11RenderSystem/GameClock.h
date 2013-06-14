#pragma once

#include "D3D11Common.h"

namespace rex
{
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

		bool CounterToTimeSpan(uint64 counter, uint64 base, float* result) const
		{
			if (base > counter)
				return false;
			*result = (counter - base) / (float)m_frequency;
			return true;
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