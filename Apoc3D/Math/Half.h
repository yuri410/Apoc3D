#pragma once
#ifndef APOC3D_HALF_H
#define APOC3D_HALF_H

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

namespace Apoc3D
{
	namespace Math
	{
		typedef ushort Half;

		static Half FloatToHalf(float value)
		{
			uint bits = reinterpret_cast<uint&>(value);
			uint signBit = (uint)((bits & 0x80000000) >> 16);
			uint absolute = bits & 0x7fffffffU;
			if (absolute > 0x47ffefffU)  // too big(131039.992188)
				return (ushort)(signBit | 0x7fff);

			if (absolute < 0x38800000U)  // too small(0.000061)
			{
				// denorm
				uint fraction = (absolute & 0x7fffffU) | 0x800000U;
				int shift = 113 - ((int)(absolute >> 23));
				absolute = (shift > 31) ? 0 : (fraction >> shift);
				return (ushort)(signBit | ((absolute + 0xfff + ((absolute >> 13) & 1)) >> 13));
			}
			return (ushort)(signBit | ((absolute + 0xc8000000U + 0xfff + ((absolute >> 13) & 1)) >> 13));
		}
		static float HalfToFloat(Half value)
		{
			uint bits;
			if ((value & 0xffff7c00) == 0)
			{
				if ((value & 0x3ff) != 0)
				{
					uint exponent = 0xfffffff2;
					uint fraction = (uint)(value & 0x3ff);
					while ((fraction & 0x400) == 0)
					{
						exponent--;
						fraction <<= 1;
					}
					fraction &= 0xfffffbff;
					bits = (((uint)value & 0x8000) << 16) | ((exponent + 127) << 23) | (fraction << 13);
				}
				else
				{
					bits = (uint)((value & 0x8000) << 16);
				}
			}
			else
			{
				bits = (uint)(((value & 0x8000) << 16) | ((((value >> 10) & 31) + 112) << 23) | ((value & 0x3ff) << 13));
			}
			return reinterpret_cast<const float&>(bits);
		}

	}
}
#endif