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

namespace Apoc3D
{
	namespace Math
	{
		Random Randomizer::m_randomizer;

		Random::Random()
		{
			const int32 seed = static_cast<int32>(time(0));
			SetSeed(seed);
		}

		void Random::SetSeed(int32 seed)
		{
			int seed0 = 0x9a4ec86 - abs(seed);
			m_seedArray[0x37] = seed0;
			int incr = 1;
			for (int i = 1; i < 0x37; i++)
			{
				int index = (0x15 * i) % 0x37;
				m_seedArray[index] = incr;
				incr = seed0 - incr;
				if (incr < 0)
				{
					incr += 0x7fffffff;
				}
				seed0 = m_seedArray[index];
			}
			for (int j = 1; j < 5; j++)
			{
				for (int k = 1; k < 0x38; k++)
				{
					m_seedArray[k] -= m_seedArray[1 + ((k + 30) % 0x37)];
					if (m_seedArray[k] < 0)
					{
						m_seedArray[k] += 0x7fffffff;
					}
				}
			}
			m_inext = 0;
			m_inextp = 0x15;
		}
	}
}