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
#ifndef MATHCOMMON_H
#define MATHCOMMON_H

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

		const float MaxFloat = std::numeric_limits<float>::infinity();
		const float NEG_INFINITY = -std::numeric_limits<float>::infinity();
		const float PI = float( 4.0 * atan( 1.0 ) );
		const float Two_PI = PI * 2;
		const float Half_PI = float(PI * 0.5);

#define ToDegree(x) (x * (PI/180.0f))
#define ToRadian(x) (x * (180.0f/PI))

		float Sign(float value)
		{
			if (value>0)
				return 1;
			return value<0 ? -1 : 0;
		}
	}
}

#endif