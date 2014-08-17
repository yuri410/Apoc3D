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
			: m_index(0)
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
			else
			{
				m_seed = seed;
				int32 holdRand = seed;

				for (int32 i=0;i<16;i++)
				{
					m_state[i] = (((holdRand = holdRand * 214013L + 2531011L) >> 16) & 0x7fff);
				}
			}
		}

		void Random::Reset()
		{
			SetSeed(m_seed, false);
			m_index = 0;
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