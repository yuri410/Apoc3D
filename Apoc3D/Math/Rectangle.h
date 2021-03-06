#pragma once
#ifndef APOC3D_RECTANGLE_H
#define APOC3D_RECTANGLE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "Point.h"

namespace Apoc3D
{
	namespace Math
	{
		template <typename T>
		class RectBase
		{
			static const bool IsFloat = std::is_floating_point<T>::value;
		public:
			using PointType = typename std::conditional<IsFloat, PointF, Point>::type;

			T X = 0;				/** Specifies the x-coordinate of the rectangle. */
			T Y = 0;				/** Specifies the y-coordinate of the rectangle. */
			T Width = 0;			/** Specifies the width of the rectangle. */
			T Height = 0;			/** Specifies the height of the rectangle. */

			T getLeft() const { return X; }					/** Returns the x-coordinate of the left side of the rectangle. */
			T getRight() const { return X + Width; }		/** Returns the x-coordinate of the right side of the rectangle. */
			T getTop() const { return Y; }					/** Returns the y-coordinate of the top of the rectangle. */
			T getBottom() const { return Y + Height; }		/** Returns the y-coordinate of the bottom of the rectangle. */

			T getCenterX() const { return X + Width / 2; }
			T getCenterY() const { return Y + Height / 2; }

			bool IsEmpty() const { return (Width == 0) && (Height == 0) && (X == 0) && (Y == 0); }

			PointType getCenter() const { return{ getCenterX(), getCenterY() }; }			/** Gets the Point that specifies the center of the rectangle. */
			PointType getCenterLeft() const { return{ X, getCenterY() }; }
			PointType getCenterRight() const { return{ X + Width, getCenterY() }; }

			PointType getTopLeft() const { return{ X, Y }; }
			PointType getTopRight() const { return{ X + Width, Y }; }
			PointType getTopCenter() const { return{ getCenterX(), Y }; }

			PointType getBottomLeft() const { return{ X, Y + Height }; }
			PointType getBottomRight() const { return{ X + Width, Y + Height }; }
			PointType getBottomCenter() const { return{ getCenterX(), Y + Height }; }

			PointType getPosition() const { return{ X, Y }; }
			void setPosition(PointType pt) { X = pt.X; Y = pt.Y; }
			void setPosition(T x, T y) { X = x; Y = y; }

			PointType getSize() const { return{ Width, Height }; }


			float getAspectRatio() const { return static_cast<float>(Width) / Height; }


			/** Changes the position of the Rectangle. */
			void Offset(PointType amount)
			{
				X += amount.X; Y += amount.Y;
			}

			/** Changes the position of the Rectangle. */
			void Offset(T offsetX, T offsetY)
			{
				X += offsetX; Y += offsetY;
			}

			/** Pushes the edges of the Rectangle out by the horizontal and vertical values specified. */
			void Inflate(T horizontalAmount, T verticalAmount)
			{
				X -= horizontalAmount;
				Y -= verticalAmount;
				Width += horizontalAmount * 2;
				Height += verticalAmount * 2;
			}
			void InflateVert(T top, T bottom)
			{
				Y -= top;
				Height += top + bottom;
			}
			void InflateHorizonal(T left, T right)
			{
				X -= left;
				Width += left + right;
			}


			/** Determines whether this Rectangle contains a specified point
			 *  represented by its x- and y-coordinates.
			 */
			bool Contains(T x, T y) const
			{
				return (X <= x) && x < (X + Width) && (Y <= y) && y < (Y + Height);
			}

			/** Determines whether this Rectangle contains a specified Point. */
			bool Contains(PointType value) const
			{
				return (X <= value.X) &&
					(value.X < X + Width) &&
					(Y <= value.Y) &&
					(value.Y < Y + Height);
			}

			/** Determines whether this Rectangle entirely contains a specified Rectangle. */
			bool Contains(const RectBase& value) const
			{
				return (X <= value.X) &&
					(value.X + value.Width <= X + Width) &&
					(Y <= value.Y) &&
					(value.Y + value.Height <= Y + Height);
			}

			/** Determines whether a specified Rectangle intersects with this Rectangle. */
			bool Intersects(const RectBase& value) const
			{
				return (value.X < X + Width) &&
					(X < value.X + value.Width) &&
					(value.Y < Y + Height) &&
					(Y < value.Y + value.Height);
			}


		protected:
			RectBase() { }
			RectBase(T x, T y, T w, T h)
				: X(x), Y(y), Width(w), Height(h) { }

		};

		class APAPI RectangleF : public RectBase<float>
		{
		public:
			RectangleF() { }
			RectangleF(const PointF& pos, const PointF& size)
				: RectBase(pos.X, pos.Y, size.X, size.Y) { }

			RectangleF(float x, float y, float width, float height)
				: RectBase(x, y, width, height) { }

			RectangleF GetCenterRegion(const PointF& sz) const;

			RectangleF Offsetted(PointF ofs) const;
			RectangleF Offsetted(float x, float y) const;

			bool operator==(const RectangleF& other) const;
			bool operator!=(const RectangleF& other) const;

			static RectangleF Intersect(const RectangleF& a, const RectangleF& b);
			static RectangleF Union(const RectangleF& a, const RectangleF& b);

			static RectangleF Lerp(const RectangleF& r1, const RectangleF& r2, float amount);

			const static RectangleF Empty;
		};

		class APAPI Rectangle : public RectBase<int32>
		{
		public:
			Rectangle() { }
			
			Rectangle(const Point& pos, const Point& size)
				: RectBase(pos.X, pos.Y, size.X, size.Y) { }

			Rectangle(int x, int y, int width, int height)
				: RectBase(x, y, width, height) { }

			Rectangle GetCenterRegion(const Point& sz) const;
			Rectangle Offsetted(Point ofs) const;
			Rectangle Offsetted(int32 x, int32 y) const;

			void SplitVert(int32 ypos, Rectangle& top, Rectangle& bottom);
			void SplitHorizontal(int32 xpos, Rectangle& left, Rectangle& right);

			void DivideTo9Regions(int32 xpad, int32 ypad, Rectangle(&result)[9]) const { DivideTo9Regions(xpad, xpad, ypad, ypad, result); }

			void DivideTo9Regions(int32 left, int32 right, int32 top, int32 bottom, Rectangle(&result)[9]) const;
			void DivideTo3RegionsX(int32 left, int32 right, Rectangle(&result)[3]) const;
			void DivideTo3RegionsY(int32 top, int32 bottom, Rectangle(&result)[3]) const;

			operator RectangleF() const;

			bool operator==(const Rectangle& other) const;
			bool operator!=(const Rectangle& other) const;

			/** Creates a Rectangle defining the area where one rectangle overlaps another rectangle. */
			static Rectangle Intersect(const Rectangle &a, const Rectangle &b);

			/** Creates a new Rectangle that exactly contains two other rectangles. */
			static Rectangle Union(const Rectangle &a, const Rectangle &b);


			const static Rectangle Empty;
		};

		
	}
}
#endif