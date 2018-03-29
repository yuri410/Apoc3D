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
#include "apoc3d/Collections/List.h"
#include "apoc3d/Meta/EventDelegate.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI Random
		{
		public:
			Random();
			Random(int32 seed) { SetSeed(seed, true); }
			~Random() { }

			int32 getSeed() const { return m_seed; }
			void SetSeed(int32 seed, bool reset);
			void Reset();

			int32 Next() { return RawSample(); }
			int32 NextInclusive(int32 max);
			int32 NextExclusive(int32 max);
			int32 Next(int32 minValue, int32 maxValue);

			int64 Next64();
			int64 Next64Inclusive(int64 max);
			int64 Next64Exclusive(int64 max);
			int64 Next64(int64 minValue, int64 maxValue);

			float NextFloat() { return Sample(); }
			double NextDouble() { return SampleD(); }
		private:
			uint32 m_state[16];
			int32 m_index = 0;
			int32 m_seed = 0;

			int32 RawSample()
			{
				// WELLRNG512
				uint32 a, b, c, d;
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
				return static_cast<int32>(m_state[m_index] & 0x7fffffffU);
			}
			float Sample() { return RawSample() * (1.0f / 2147483647.0f); }
			double SampleD() { return RawSample() * (1.0 / 2147483647.0); }
		};


		class APAPI Randomizer
		{
		public:
#if _DEBUG
#define RANDOMIZER_CHECKTHREAD CheckThreadSafety()
#else
#define RANDOMIZER_CHECKTHREAD
#endif
			static bool NextBool() { return (Next() & 1) != 0; }

			static int32 Next() { RANDOMIZER_CHECKTHREAD; return m_randomizer.Next();  }
			static int32 NextInclusive(int32 max) { RANDOMIZER_CHECKTHREAD; return m_randomizer.NextInclusive(max); }
			static int32 NextExclusive(int32 max) { RANDOMIZER_CHECKTHREAD; return m_randomizer.NextExclusive(max); }
			static int32 Next(int32 minValue, int32 maxValue) { RANDOMIZER_CHECKTHREAD; return m_randomizer.Next(minValue, maxValue); }

			static float NextFloat() { RANDOMIZER_CHECKTHREAD; return m_randomizer.NextFloat();  }
			static float NextFloat(float minValue, float maxValue)
			{
				RANDOMIZER_CHECKTHREAD;
				if (minValue >= maxValue)
					return minValue;
				return minValue + (maxValue - minValue) * m_randomizer.NextFloat(); 
			}
			static float NextFloat(const float* ranges) { return NextFloat(ranges[0], ranges[1]);  }


			template <int32 N>
			static int32 Choose(const float(&p)[N]) { return Choose(p, N); }

			static int32 Choose(const Apoc3D::Collections::List<float>& lst) { return Choose(lst.getElements(), lst.getCount()); }

			static int32 Choose(const float* p, int32 count)
			{
				RANDOMIZER_CHECKTHREAD;

				float total = 0;
				for (int32 i = 0; i < count; i++)
				{
					total += p[i];
				}

				float rnd = NextFloat() * total;

				float cmp = 0;
				for (int32 i = 0; i < count; i++)
				{
					cmp += p[i];
					if (rnd < cmp)
					{
						return i;
					}
				}
				return 0;
			}
			

			template <typename T, int32 N>
			static void Shuffle(T (&lst)[N]) { Shuffle(lst, N); }

			template <typename T>
			static void Shuffle(Apoc3D::Collections::List<T>& lst) { Shuffle(lst.getElements(), lst.getCount()); }

			template <typename T>
			static void Shuffle(T* arr, int32 count)
			{
				RANDOMIZER_CHECKTHREAD;

				for (int32 i = count - 1; i > 0; i--)
				{
					std::swap(arr[i], arr[NextInclusive(i)]);
				}
			}

			static int32 getSeed() { return m_randomizer.getSeed(); }
			static void setSeed(int32 seed, bool reset) { RANDOMIZER_CHECKTHREAD; m_randomizer.SetSeed(seed, reset); }
			static void Reset() { RANDOMIZER_CHECKTHREAD; m_randomizer.Reset(); }
		private:
			static Random m_randomizer;

#if _DEBUG
			static void CheckThreadSafety();
			static uint32 m_existingThreadID;
#endif

			Randomizer() {}
			~Randomizer() {}

#undef RANDOMIZER_CHECKTHREAD
		};

	}

}
#endif