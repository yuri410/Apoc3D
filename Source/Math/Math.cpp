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
#include "Vector.h"
#include "Color.h"
#include "Point.h"
#include "Rectangle.h"
#include "Common.h"

namespace Apoc3D
{
	namespace Math
	{
		class FieldInitializer
		{
		public:
			FieldInitializer()
			{
				uint Data2[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
				memcpy((void*)&_MASKSIGN_, Data2, sizeof(Data2));
				
			}

		} Initializer;

		inline Vector VecLoad(float x, float y, float z, float w)
		{
			float vec[4] = {x,y,z,w};
			return _mm_load_ps(&vec[0]);
		};

		const Vector ZeroVec = VecLoad(0);
		const Vector UnitXVec = VecLoad(1,0,0,0);
		const Vector UnitYVec = VecLoad(0,1,0,0);
		const Vector UnitZVec = VecLoad(0,0,1,0);

		const Point Point::Zero = Point(0,0);
		const Rectangle Rectangle::Empty = Rectangle(0,0,0,0);
		const RectangleF RectangleF::Empty = RectangleF(0,0,0,0);
	}
}