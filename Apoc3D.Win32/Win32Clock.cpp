#include "Win32Clock.h"

namespace Apoc3D
{
	namespace Win32
	{
		static uint64 GetCounter()
		{
			LARGE_INTEGER counter;
			BOOL res = QueryPerformanceCounter(&counter);
			assert(res);
			return counter.QuadPart;
		}

		static uint64 GetFrequency()
		{
			LARGE_INTEGER freq;
			BOOL res = QueryPerformanceFrequency(&freq);
			assert(res);

			return freq.QuadPart;
		}

		//////////////////////////////////////////////////////////////////////////

		void Win32Clock::Reset()
		{
			m_baseCounter = 0;
			m_lastCounter = 0;

			m_elapsedTime = 0;

			m_currentTimeBase = 0;
			m_currentTime = 0;
			m_baseCounter = GetCounter();
			m_lastRealTimeValid = false;
			m_frequency = GetFrequency();
		}

		void Win32Clock::Step()
		{
			uint64 counter = GetCounter();

			if (!m_lastRealTimeValid)
			{
				m_lastCounter = counter;
				m_lastRealTimeValid = true;
			}

			if (!CounterToTimeSpan(counter, m_baseCounter, &m_currentTime))
			{
				// update the base value and try again to adjust for overflow
				// overflow: counter < m_baseCounter

				// update base to last frame time/counter
				m_currentTimeBase += m_currentTime;
				m_baseCounter = m_lastCounter;

				// get current time off the updated base
				if (!CounterToTimeSpan(counter, m_baseCounter, &m_currentTime))
				{
					// if still overflow, set the base to current (at the cost of a small error)
					m_baseCounter = counter;
					m_currentTime = 0;
				}
			}

			if (!CounterToTimeSpan(counter, m_lastCounter, &m_elapsedTime))
			{
				m_elapsedTime = 0;
			}

			m_lastCounter = counter;
		}

		bool Win32Clock::CounterToTimeSpan(uint64 counter, uint64 base, double* result) const
		{
			// this checks overflow
			if (base > counter)
				return false;
			*result = (counter - base) / (double)m_frequency;
			return true;
		}
	}
}