#pragma once
#ifndef APOC3D_POINT_H
#define APOC3D_POINT_H

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


#include "MathCommon.h"

namespace Apoc3D
{
	namespace Math
	{
		class Size
		{
		public:
			int Width;
			int Height;
			Size() : Width(0), Height(0) { }
			Size(int w, int h) : Width(w), Height(h) { }

			bool operator==(const Size &other) const
			{
				return (Width == other.Width) && (Height == other.Height);	
			}
			bool operator!=(const Size &other) const { return !(*this == other); }

			const static Size Zero;
		};

		class APAPI Point
		{
		public:
			int X;
			int Y;

			Point() : X(0), Y(0) { X = Y = 0; }
			Point(int x, int y) { X = x; Y = y; }


			bool operator==(const Point &other) const { return (X == other.X) && (Y == other.Y); }
			bool operator!=(const Point &other) const { return !(*this == other); }

			Point operator+(const Point &other) const { return Point(X + other.X, Y + other.Y); }
			Point operator-(const Point &other) const { return Point(X - other.X, Y - other.Y); }
			

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


			bool operator==(const PointF &other) const { return (X == other.X) && (Y == other.Y); }
			bool operator!=(const PointF &other) const { return !(*this == other); }

			PointF operator+(const PointF &other) const { return PointF(X + other.X, Y + other.Y); }
			PointF operator-(const PointF &other) const { return PointF(X - other.X, Y - other.Y); }

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