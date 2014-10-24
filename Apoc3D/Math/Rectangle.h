#pragma once
#ifndef APOC3D_RECTANGLE_H
#define APOC3D_RECTANGLE_H

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

#include "Point.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI RectangleF
		{
		public:
			float X;
			float Y;
			float Width;
			float Height;

			RectangleF() : X(0), Y(0), Width(0), Height(0) { }
			RectangleF(const PointF& pos, const PointF& size)
				: X(pos.X), Y(pos.Y), Width(size.X), Height(size.Y) { }

			RectangleF(float x, float y, float width, float height)
				: X(x), Y(y), Width(width), Height(height) { }

			float getLeft() const { return X; }
			float getRight() const { return X + Width; }
			float getTop() const { return Y; }
			float getBottom() const { return Y + Height; }

			PointF getCenter() const { return PointF(X + Width / 2, Y + Height / 2); }

			PointF getTopLeft() const { return PointF(X, Y); }
			PointF getTopRight() const { return PointF(X + Width, Y); }

			PointF getBottomLeft() const { return PointF(X, Y + Height); }
			PointF getBottomRight() const { return PointF(X + Width, Y + Height); }

			PointF getPosition() const { return PointF(X, Y); }
			void setPosition(const PointF& pt) { X = pt.X; Y = pt.Y; }

			PointF getSize() const { return PointF(Width, Height); }

			bool IsEmpty() const { return (Width == 0) && (Height == 0) && (X == 0) && (Y == 0); }


			void Offset(const PointF &amount)
			{
				X += amount.X; Y += amount.Y;
			}
			void Offset(float offsetX, float offsetY)
			{
				X += offsetX; Y += offsetY;
			}
			void Inflate(float horizontalAmount, float verticalAmount)
			{
				X -= horizontalAmount;
				Y -= verticalAmount;
				Width += horizontalAmount * 2;
				Height += verticalAmount * 2;
			}
			bool Contains(float x, float y) const
			{
				return (X <= x) && x < (X + Width) && (Y <= y) && y < (Y + Height);
			}
			bool Contains(const PointF &value) const
			{
				return (X <= value.X) &&
					(value.X < X + Width) &&
					(Y <= value.Y) &&
					(value.Y < Y + Height);
			}
			bool Contains(const RectangleF &value) const
			{
				return (X <= value.X) &&
					((value.X + value.Width) <=
					(X + Width)) &&
					(Y <= value.Y) &&
					((value.Y + value.Height) <= (Y + Height));
			}
			bool Intersects(const RectangleF &value) const
			{
				return (value.X < (X + Width)) &&
					(X < (value.X + value.Width)) &&
					(value.Y < (Y + Height)) &&
					(Y < (value.Y + value.Height));
			}

			RectangleF GetCenterRegion(const Point& sz) const
			{
				return RectangleF(getCenter() - sz / 2, sz);
			}

			static RectangleF Intersect(const RectangleF &a, const RectangleF &b);
			static RectangleF Union(const RectangleF &a, const RectangleF &b);

			bool operator==(const RectangleF &other) const
			{
				return (X  == other.X) && (Y == other.Y) && (Width == other.Width) && (Height == other.Height);	
			}
			bool operator!=(const RectangleF &other) const { return !(*this == other); }

			const static RectangleF Empty;
		};

		class APAPI Rectangle
		{
		public:
			int X;				/** Specifies the x-coordinate of the rectangle. */
			int Y;				/** Specifies the y-coordinate of the rectangle. */
			int Width;			/** Specifies the width of the rectangle. */
			int Height;			/** Specifies the height of the rectangle. */

			Rectangle() : X(0), Y(0), Width(0), Height(0) { }
			
			Rectangle(const Point& pos, const Point& size)
				: X(pos.X), Y(pos.Y), Width(size.X), Height(size.Y) { }

			Rectangle(int x, int y, int width, int height)
				: X(x), Y(y), Width(width), Height(height) { }

			/** Returns the x-coordinate of the left side of the rectangle. */
			int getLeft() const { return X; }

			/** Returns the x-coordinate of the right side of the rectangle. */
			int getRight() const { return X + Width; }

			/** Returns the y-coordinate of the top of the rectangle. */
			int getTop() const { return Y; }

			/** Returns the y-coordinate of the bottom of the rectangle. */
			int getBottom() const { return Y + Height; }

			/** Gets the Point that specifies the center of the rectangle. */
			Point getCenter() const { return Point(X + Width / 2, Y + Height / 2); }

			Point getTopLeft() const { return Point(X, Y); }
			Point getTopRight() const { return Point(X + Width, Y); }

			Point getBottomLeft() const { return Point(X, Y + Height); }
			Point getBottomRight() const { return Point(X + Width, Y + Height); }

			Point getPosition() const { return Point(X, Y); }
			void setPosition(const Point& pt) { X = pt.X; Y = pt.Y; }

			Point getSize() const { return Point(Width, Height); }

			float getAspectRatio() const { return static_cast<float>(Width) / Height; }

			bool IsEmpty() const { return (Width == 0) && (Height == 0) && (X == 0) && (Y == 0); }

			/** Changes the position of the Rectangle. */
			void Offset(const Point &amount)
			{
				X += amount.X; Y += amount.Y;
			}

			/** Changes the position of the Rectangle. */
			void Offset(int offsetX, int offsetY)
			{
				X += offsetX; Y += offsetY;
			}

			/** Pushes the edges of the Rectangle out by the horizontal and vertical values specified. */
			void Inflate(int horizontalAmount, int verticalAmount)
			{
				X -= horizontalAmount;
				Y -= verticalAmount;
				Width += horizontalAmount * 2;
				Height += verticalAmount * 2;
			}

			/** Determines whether this Rectangle contains a specified point 
			    represented by its x- and y-coordinates.
			*/
			bool Contains(int x, int y) const
			{
				return (X <= x) && x < (X + Width) && (Y <= y) && y < (Y + Height);
			}

			/** Determines whether this Rectangle contains a specified Point. */
			bool Contains(const Point& value) const
			{
				return (X <= value.X) &&
					(value.X < X + Width) &&
					(Y <= value.Y) &&
					(value.Y < Y + Height);
			}

			/** Determines whether this Rectangle entirely contains a specified Rectangle. */
			bool Contains(const Rectangle& value) const
			{
				return (X <= value.X) &&
					((value.X + value.Width) <=
					(X + Width)) &&
					(Y <= value.Y) &&
					((value.Y + value.Height) <= (Y + Height));
			}

			/** Determines whether a specified Rectangle intersects with this Rectangle. */
			bool Intersects(const Rectangle& value) const
			{
				return (value.X < (X + Width)) &&
					(X < (value.X + value.Width)) &&
					(value.Y < (Y + Height)) &&
					(Y < (value.Y + value.Height));
			}

			Rectangle GetCenterRegion(const Point& sz) const
			{
				return Rectangle(getCenter() - sz / 2, sz);
			}

			/** Creates a Rectangle defining the area where one rectangle overlaps another rectangle. */
			static Rectangle Intersect(const Rectangle &a, const Rectangle &b);

			/** Creates a new Rectangle that exactly contains two other rectangles. */
			static Rectangle Union(const Rectangle &a, const Rectangle &b);

			operator RectangleF() const
			{
				return RectangleF(static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Width), static_cast<float>(Height)); 
			} 

			bool operator==(const Rectangle &other) const
			{
				return (X == other.X) && (Y == other.Y) && (Width == other.Width) && (Height == other.Height);	
			}
			bool operator!=(const Rectangle &other) const { return !(*this == other); }

			const static Rectangle Empty;
		};

		
	}
}
#endif