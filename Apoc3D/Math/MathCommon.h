#pragma once
#ifndef APOC3D_MATHCOMMON_H
#define APOC3D_MATHCOMMON_H

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
		enum ContainmentType
		{
			CONTAIN_Disjoint,
			CONTAIN_Contains,
			CONTAIN_Intersects
		};
		enum PlaneIntersectionType
		{
			PLANEIT_Front,
			PLANEIT_Back,
			PLANEIT_Intersecting
		};

		/*extern const float PI;
		extern const float Two_PI;
		extern const float Half_PI;
		extern const float Root2;
		extern const float Root3;*/

#define L_PIf 3.1415926535897932384626433832795f
#define L_Two_PIf 6.283185307179586476925286766559f
#define L_Half_PIf 1.5707963267948966192313216916398f
#define L_Root2f 1.4142135623730950488016887242097f
#define L_Root3f 1.7320508075688772935274463415059f

#define L_PId 3.1415926535897932384626433832795
#define L_Two_PId 6.283185307179586476925286766559
#define L_Half_PId 1.5707963267948966192313216916398
#define L_Root2d 1.4142135623730950488016887242097
#define L_Root3d 1.7320508075688772935274463415059

		const float PI = L_PIf;//float( 4.0 * atan( 1.0 ) );
		const float Two_PI = L_Two_PIf; // PI * 2
		const float Half_PI = L_Half_PIf;// float(PI * 0.5);
		const float Root2 = 1.41421f;
		const float Root3 = 1.73205f;

#define ToRadian(x) (x * (L_PIf/180.0f))
#define ToDegree(x) (x * (180.0f/L_PIf))
		
		inline float Frac(float value)
		{
			assert(value>=0);
			return value - floorf(value);
		}

		inline float Sign(float value)
		{
			if (value>0)
				return 1.0f;
			return value<0 ? -1.0f : 0.0f;
		}
		inline int32 Sign(int32 value)
		{
			if (value>0)
				return 1;
			return value<0 ? -1 : 0;
		}

		inline float Lerp(float a, float b, float amount)
		{
			return a + (b-a)*amount;
		}


		inline float Clamp(float v, float _min, float _max)
		{
			if (v > _max)
				return _max;
			return (v < _min) ? _min : v;
		}
		inline int32 Clamp(int32 v, int32 _min, int32 _max)
		{
			if (v > _max)
				return _max;
			return (v<_min) ? _min : v;
		}

		inline float Saturate(float v) 
		{
			const float t = v < 0 ? 0 : v;
			return t > 1 ? 1 : t;
		}

		inline int32 Max(int32 a, int32 b)
		{
			return (a > b) ? a : b;
		}
		inline int32 Min(int32 a, int32 b)
		{
			return (a < b) ? a : b;
		}

		inline float Max(float a, float b)
		{
			return (a > b) ? a : b;
		}
		inline float Min(float a, float b)
		{
			return (a < b) ? a : b;
		}

		inline int32 Round(float x)
		{
			return x>=0 ? (int32)(x+0.5) : (int32)(x-0.5);
		}



		inline int32 RoundUp2(int32 x)
		{
			assert(x>=0);
			return (x + 1) & ~0x01;
		}
		inline int32 RoundUp4(int32 x)
		{
			assert(x>=0);
			return (x + 3) & ~0x03;
		}
		inline int32 RoundUp8(int32 x)
		{
			assert(x>=0);
			return (x + 7) & ~0x07;
		}
		inline int32 RoundUp16(int32 x)
		{
			assert(x>=0);
			return (x + 15) & ~0x0f;
		}
		inline int32 Pow2RoundUp(int32 x)
		{
			if (x < 0)
				return 0;
			--x;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			return x+1;
		}
		inline bool IsPowerOfTwo(int32 x)
		{
			if (x <= 0)
				return false;

			return (x & (x - 1)) == 0;
		}

	}
}

#endif