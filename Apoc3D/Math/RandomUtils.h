#pragma once
#ifndef APOC3D_RANDOMUTILS_H
#define APOC3D_RANDOMUTILS_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"
#include "apoc3d/EventDelegate.h"

namespace Apoc3D
{
	namespace Math
	{
		//typedef EventDelegate1<int&> SeedChangedEventHandler;

		class APAPI Random
		{
		public:
			Random();
			Random(int seed) : m_index(0) { SetSeed(seed); }
			~Random() { }

			int32 getSeed() const { return m_seed; }
			void SetSeed(int32 seed);

			int32 Next() { return RawSample(); }

			int32 Next(int32 max)
			{
				assert(max>=0);
				return static_cast<int32>(Sample() * max);
			}

			int32 Next(int32 minValue, int32 maxValue)
			{
				assert(minValue<=maxValue);
				int32 range = maxValue - minValue;
				return static_cast<int32>(Sample() * range) + minValue;
			}
			float NextFloat() { return Sample(); }

		private:
			int32 m_state[16];
			int32 m_index;
			int32 m_seed;

			int32 RawSample()
			{
				unsigned long a, b, c, d;
				a = m_state[m_index];
				c = m_state[(m_index+13)&15];
				b = a^c^(a<<16)^(c<<15);
				c = m_state[(m_index+9)&15];
				c ^= (c>>11);
				a = m_state[m_index] = b^c;
				d = a^((a<<5)&0xDA442D20UL);
				m_index = (m_index + 15)&15;
				a = m_state[m_index];
				m_state[m_index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
				return static_cast<int32>(m_state[m_index] & 0x7fffffffUL);
			}
			float Sample()
			{
				return RawSample() / 2147483647.0f;
			}
		};


		class APAPI Randomizer
		{
		public:
			static int32 Next() { return m_randomizer.Next(); }
			static int32 Next(int32 max) { return m_randomizer.Next(max); }
			static int32 Next(int32 minValue, int32 maxValue) { return m_randomizer.Next(minValue, maxValue); }

			static float NextFloat() { return m_randomizer.NextFloat(); }
			static float NextFloat(float minValue, float maxValue)
			{
				if (minValue >= maxValue)
					return minValue;
				return minValue + (maxValue - minValue) * m_randomizer.NextFloat(); 
			}
			static float NextFloat(const float* ranges) { return NextFloat(ranges[0], ranges[1]); }

			static int Choose(const float* p, int count)
			{
				float total = 0;
				for (int i = 0; i < count; i++)
				{
					total += p[i];
				}

				float rnd = NextFloat() * total;

				float cmp = 0;
				for (int i = 0; i < count; i++)
				{
					cmp += p[i];
					if (rnd < cmp)
					{
						return i;
					}
				}
				return 0;
			}

			static int32 getSeed() { return m_randomizer.getSeed(); }

		private:
			static Random m_randomizer;

			Randomizer() {}
			~Randomizer() {}
		};

	}

}
#endif