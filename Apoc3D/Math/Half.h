/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#ifndef APOC3D_HALF_H
#define APOC3D_HALF_H

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Math
	{
		typedef ushort Half;

		static Half FloatToHalf(float value)
		{
			uint num5 = reinterpret_cast<uint&>(value);
			uint num3 = (uint)((num5 & 0x80000000) >> 0x10);
			uint num = num5 & 0x7fffffff;
			if (num > 0x47ffefff)
			{
				return (ushort)(num3 | 0x7fff);
			}
			if (num < 0x38800000)
			{
				uint num6 = (num & 0x7fffff) | 0x800000;
				int num4 = 0x71 - ((int)(num >> 0x17));
				num = (num4 > 0x1f) ? 0x0 : (num6 >> num4);
				return (ushort)(num3 | (((num + 0xfff) + ((num >> 0xd) & 0x1)) >> 0xd));
			}
			return (ushort)(num3 | ((((num + 0xc8000000) + 0xfff) + ((num >> 0xd) & 0x1)) >> 0xd));

		}
		static float HalfToFloat(Half value)
		{
			uint num3;
			if ((value & 0xffff7c00) == 0)
			{
				if ((value & 0x3ff) != 0)
				{
					uint num2 = 0xfffffff2;
					uint num = (uint)(value & 0x3ff);
					while ((num & 0x400) == 0)
					{
						num2--;
						num = num << 0x1;
					}
					num &= 0xfffffbff;
					num3 = (((uint)value & 0x8000) << 0x10) | ((num2 + 0x7f) << 0x17) | (num << 0xd);
				}
				else
				{
					num3 = (uint)((value & 0x8000) << 0x10);
				}
			}
			else
			{
				num3 = (uint)((((value & 0x8000) << 0x10) | (((value >> 0xa) & 0x1f) - 0xf + 0x7f) << 0x17) | ((value & 0x3ff) << 0xd));
			}
			return reinterpret_cast<const float&>(num3);
		}

	}
}
#endif