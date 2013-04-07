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
#define minf(x, y) (x<y ? x : y)
#define maxf(x, y) (x>y ? x : y)
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

		//static const float MaxFloat = std::numeric_limits<float>::max();
		//static const float MinFloat = std::numeric_limits<float>::min();
		//static const float POSINF_Float = std::numeric_limits<float>::infinity();
		//static const float NEGINF_Float = -std::numeric_limits<float>::infinity();
		//static const float MaxFloat;
		//static const float MinFloat;
		//static const float POSINF_Float;
		//static const float NEGINF_Float;

		const float PI = float( 4.0 * atan( 1.0 ) );
		const float Two_PI = PI * 2;
		const float Half_PI = float(PI * 0.5);
		const float Root2 = 1.41421f;
		const float Root3 = 1.73205f;

#define ToRadian(x) (x * (Apoc3D::Math::PI/180.0f))
#define ToDegree(x) (x * (180.0f/Apoc3D::Math::PI))

		static inline float Sign(float value)
		{
			if (value>0)
				return 1.0f;
			return value<0 ? -1.0f : 0.0f;
		}
		static inline float Lerp(float a, float b, float amount)
		{
			return a + (b-a)*amount;
		}
		static inline float Saturate(float v)
		{
			if (v>1)
				v=1;
			return (v<0) ? 0:v;
		}
		static inline float Clamp(float v, float _min, float _max)
		{
			if (v > _max)
				v = _max;
			return (v < _min) ? _min : v;
		}
		static inline int Clamp(int v, int _min, int _max)
		{
			if (v > _max)
				v = _max;
			return (v<_min) ? _min : v;
		}
	}
}

#endif