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

#include "RandomUtils.h"

#include <ctime>

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

#if _WIN32
#include <Windows.h>
#endif

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Math
	{
		Random Randomizer::m_randomizer;

		Random::Random()
		{
			const int32 seed = static_cast<int32>(time(0));
			SetSeed(seed, true);
		}

		void Random::SetSeed(int32 seed, bool reset)
		{
			if (reset)
			{
				Reset();
			}
			
			m_seed = seed;
			int32 holdRand = seed;

			for (uint32& s : m_state)
			{
				s = (((holdRand = holdRand * 214013L + 2531011L) >> 16) & 0x7fff);
			}
		}

		void Random::Reset()
		{
			ZeroArray(m_state);
			m_seed = 0;
			m_index = 0;
		}

		int32 Random::NextInclusive(int32 max)
		{
			assert(max >= 0);
			int32 result = static_cast<int32>(SampleD() * (max + 1));
			if (result > max)
				result = max; // this might happen if processor float point precision is tuned down
			return result;
		}

		int32 Random::NextExclusive(int32 max)
		{
			if (--max <= 0)
				return 0;
			return NextInclusive(max);
		}

		int32 Random::Next(int32 minValue, int32 maxValue)
		{
			return (int32)Next64(minValue, maxValue);
		}

		int64 Random::Next64()
		{
			uint64 a = (uint64)RawSample();
			uint64 b = (uint64)RawSample();
			return (int64)(a << 32 | b);
		}

		int64 Random::Next64Inclusive(int64 max)
		{
			assert(max >= 0);
			int64 result = static_cast<int64>(SampleD() * (max + 1));
			if (result > max)
				result = max; // this might happen if processor float point precision is tuned down
			return result;
		}

		int64 Random::Next64Exclusive(int64 max)
		{
			if (--max <= 0)
				return 0;
			return Next64Inclusive(max);
		}

		int64 Random::Next64(int64 minValue, int64 maxValue)
		{
			assert(minValue <= maxValue);
			int64 range = maxValue - minValue;
			return static_cast<int64>(SampleD() * range) + minValue;
		}


#if _DEBUG
		uint32 Randomizer::m_existingThreadID = 0;

		void Randomizer::CheckThreadSafety()
		{
			uint32 id = GetCurrentThreadId();

			if (id != m_existingThreadID)
			{
				if (m_existingThreadID == 0)
				{
					m_existingThreadID = id;
				}
				else
				{
					ApocLog(LOG_System, L"[Randomizer] Called from thread " + StringUtils::UIntToString(id) + L". Unsafe!", LOGLVL_Warning);

#if _WIN32
					DebugBreak();
#endif
				}
			}
		}
#endif


	}
}