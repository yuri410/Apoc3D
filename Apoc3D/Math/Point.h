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
#ifndef APOC3D_POINT_H
#define APOC3D_POINT_H

#include "Common.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI Size
		{
		public:
			int Width;
			int Height;
			Size() { Width =Height =0; }
			Size(int w, int h) { Width = w; Height = h; }

			friend static bool operator ==(const Size& a, const Size& b)
			{
				return (a.Width  == b.Width) && (a.Height == b.Height);
			}
			friend static bool operator !=(const Size& a, const Size& b)
			{
				return (a.Width != b.Width) || (a.Height != b.Height);
			}
			
		
			const static Size Zero;
		};

		class APAPI Point
		{
		public:
			int X;
			int Y;

			Point() { X = Y = 0; }
			Point(int x, int y) { X = x; Y = y; }

			friend static bool operator ==(const Point& a, const Point& b)
			{
				return (a.X  == b.X) && (a.Y == b.Y);
			}
			friend static bool operator !=(const Point& a, const Point& b)
			{
				return (a.X != b.X) || (a.Y != b.Y);
			}
			friend static Point operator +(const Point& a, const Point& b)
			{
				return Point(a.X + b.X, a.Y + b.Y);
			}
			friend static Point operator -(const Point& a, const Point& b)
			{
				return Point(a.X - b.X, a.Y - b.Y);
			}

			Point& operator +=(const Point& rhs)
			{
				X += rhs.X;
				Y += rhs.Y;
				return *this;
			}
			Point& operator -=(const Point& rhs)
			{
				X -= rhs.X;
				Y -= rhs.Y;
				return *this;
			}
			const static Point Zero;
		};
		class APAPI PointF
		{
		public:
			float X;
			float Y;

			PointF() { X = Y = 0; }
			PointF(float x, float y) { X = x; Y = y; }

			friend static bool operator ==(const PointF& a, const PointF& b)
			{
				return (a.X  == b.X) && (a.Y == b.Y);
			}
			friend static bool operator !=(const PointF& a, const PointF& b)
			{
				return (a.X != b.X) || (a.Y != b.Y);
			}
			friend static PointF operator +(const PointF& a, const PointF& b)
			{
				return PointF(a.X + b.X, a.Y + b.Y);
			}
			friend static PointF operator -(const PointF& a, const PointF& b)
			{
				return PointF(a.X - b.X, a.Y - b.Y);
			}
			PointF& operator +=(const PointF& rhs)
			{
				X += rhs.X;
				Y += rhs.Y;
				return *this;
			}
			PointF& operator -=(const PointF& rhs)
			{
				X -= rhs.X;
				Y -= rhs.Y;
				return *this;
			}

			const static PointF Zero;
		};
	}
}
#endif