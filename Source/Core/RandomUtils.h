/*
-----------------------------------------------------------------------------
This source file is part of labtd

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
#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H
#include "Common.h"

#pragma once

namespace Apoc3D
{
	namespace Core
	{
		class Random
		{
		private:
			int32 m_inext;
			int32 m_inextp;
			//int32 m_mbig;
			//int32 m_mseed;
			//int32 m_mz;
			int32 m_seedArray[0x38];

			int32 InternalSample()
			{
				int inext = m_inext;
				int inextp = m_inextp;
				if (++inext >= 0x38)
				{
					inext = 1;
				}
				if (++inextp >= 0x38)
				{
					inextp = 1;
				}
				int num = m_seedArray[inext] - m_seedArray[inextp];
				if (num < 0)
				{
					num += 0x7fffffff;
				}
				m_seedArray[inext] = num;
				m_inext = inext;
				m_inextp = inextp;
				return num;
			}
			float GetSampleForLargeRange()
			{
				int num = InternalSample();
				if ((((InternalSample() % 2) == 0) ? 1 : 0) != 0)
				{
					num = -num;
				}
				float num2 = static_cast<float>(num);
				num2 += 2147483646.0f;
				return (num2 / 4294967293.f);
			}
			float Sample()
			{
				return (InternalSample() * 4.6566128752457969E-10f);
			}
		public:
			Random()
				//: m_mbig(0), m_mseed(0), m_mz(0)
			{
				const int32 seed = static_cast<int32>(time(0));
				//m_seedArray = new int[0x38];
				int num2 = 0x9a4ec86 - abs(seed);
				m_seedArray[0x37] = num2;
				int num3 = 1;
				for (int i = 1; i < 0x37; i++)
				{
					int index = (0x15 * i) % 0x37;
					m_seedArray[index] = num3;
					num3 = num2 - num3;
					if (num3 < 0)
					{
						num3 += 0x7fffffff;
					}
					num2 = m_seedArray[index];
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
			~Random()
			{
				//delete[] m_seedArray;
			}

			int32 Next()
			{
				return InternalSample();
			}
			int32 Next(int32 max)
			{
				assert(max>=0);
				return static_cast<int32>(Sample() * max);
			}
			int32 Next(int32 minValue, int32 maxValue)
			{
				assert(minValue<=maxValue);

				long num = maxValue - minValue;
				if (num <= 0x7fffffffL)
				{
					return static_cast<int32>(Sample() * num) + minValue;
				}
				return static_cast<int32>(GetSampleForLargeRange() * num) + minValue;
			}
			float NextFloat()
			{
				return Sample();
			}

		};

		class Randomizer
		{
		private:
			static Random m_randomizer;

			Randomizer() {}
			~Randomizer() {}
		public:
			static int32 Next() { return m_randomizer.Next(); }
			static int32 Next(int32 max) { return m_randomizer.Next(max); }
			static int32 Next(int32 minValue, int32 maxValue) { return m_randomizer.Next(minValue, maxValue); }

			static float NextFloat() { return m_randomizer.NextFloat(); }
		};

	}

}
#endif