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
#ifndef RECTANGLE_H
#define RECTANGLE_H


#include "Point.h"

namespace Apoc3D
{
	namespace Math
	{
		class Rectangle
		{
		public:
			int X;
			int Y;
			int Width;
			int Height;

			int getLeft() const { return X; }
			int getRight() const { return X + Width; }
			int getTop() const { return Y; }
			int getBottom() const { return Y + Height; }

			Point getCenter() const { return Point(X + Width / 2, Y + Height / 2); }

			bool IsEmpty() const { return (Width == 0) && (Height == 0) && (X == 0) && (Y == 0); }

			Rectangle() { X=Y=Width=Height=0; }
			Rectangle(int x, int y, int width, int height)
			{
				X = x; Y = y;
				Width = width; Height = height;
			}

			void Offset(const Point &amount)
			{
				X += amount.X; Y += amount.Y;
			}
			void Offset(int offsetX, int offsetY)
			{
				X += offsetX; Y += offsetY;
			}
			void Inflate(int horizontalAmount, int verticalAmount)
			{
				X -= horizontalAmount;
				Y -= verticalAmount;
				Width += horizontalAmount * 2;
				Height += verticalAmount * 2;
			}
			bool Contains(int x, int y) const
			{
				return (X <= x) && x < (X + Width) && (Y <= y) && y < (Y + Height);
			}
			bool Contains(const Point &value) const
			{
				return (X <= value.X) &&
					(value.X < X + Width) &&
					(Y <= value.Y) &&
					(value.Y < Y + Height);
			}
			bool Contains(const Rectangle &value) const
			{
				return (X <= value.X) &&
					((value.X + value.Width) <=
					(X + Width)) &&
					(Y <= value.Y) &&
					((value.Y + value.Height) <= (Y + Height));
			}
			bool Intersects(const Rectangle &value) const
			{
				return (value.X < (X + Width)) &&
					(X < (value.X + value.Width)) &&
					(value.Y < (Y + Height)) &&
					(Y < (value.Y + value.Height));
			}

			static Rectangle Intersect(const Rectangle &a, const Rectangle &b)
			{
				Rectangle rectangle;
				int abrp_x = a.X + a.Width;
				int abrp_y = a.Y + a.Height;

				int bbrp_x = b.X + b.Width;
				int bbrp_y = b.Y + b.Height;
				int num2 = (a.X > b.X) ? a.X : b.X;
				int num = (a.Y > b.Y) ? a.Y : b.Y;
				int num4 = (abrp_x < bbrp_x) ? abrp_x : bbrp_x;
				int num3 = (abrp_y < bbrp_y) ? abrp_y : bbrp_y;
				if ((num4 > num2) && (num3 > num))
				{
					rectangle.X = num2;
					rectangle.Y = num;
					rectangle.Width = num4 - num2;
					rectangle.Height = num3 - num;
					return rectangle;
				}
				rectangle.X = 0;
				rectangle.Y = 0;
				rectangle.Width = 0;
				rectangle.Height = 0;
				return rectangle;
			}
			static Rectangle Union(const Rectangle &a, const Rectangle &b)
			{
				Rectangle result;
				int abrp_x = a.X + a.Width;
				int abrp_y = a.Y + a.Height;

				int bbrp_x = b.X + b.Width;
				int bbrp_y = b.Y + b.Height;
				result.X = (a.X < b.X) ? a.X : b.X;
				result.Y = (a.Y < b.Y) ? a.Y : b.Y;
				result.Width = ((abrp_x > bbrp_x) ? abrp_x : bbrp_x) - result.X;
				result.Height = ((abrp_y > bbrp_y) ? abrp_y : bbrp_y) - result.Y;
				return result;
			}

			friend static bool operator ==(Rectangle a, Rectangle b)
			{
				return (a.X  == b.X) && (b.X == b.Y) && (a.Width == b.Width) && (a.Height == b.Height);
			}
			friend static bool operator !=(Rectangle a, Rectangle b)
			{
				if ((a.X == b.X) && (a.Y == b.Y) && (a.Width == b.Width))
				{
					return (a.Height != b.Height);
				}
				return true;
			}

			const static Rectangle Empty;
		};

		class RectangleF
		{
		public:
			float X;
			float Y;
			float Width;
			float Height;

			float getLeft() const { return X; }
			float getRight() const { return X + Width; }
			float getTop() const { return Y; }
			float getBottom() const { return Y + Height; }

			//Point getCenter() { return Point(X + Width / 2, Y + Height / 2); }

			bool IsEmpty() const { return (Width == 0) && (Height == 0) && (X == 0) && (Y == 0); }

			RectangleF() { X=Y=Width=Height=0; }
			RectangleF(float x, float y, float width, float height)
			{
				X = x; Y = y;
				Width = width; Height = height;
			}

			void Offset(const Point &amount)
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
			bool Contains(const Point &value) const
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

			static RectangleF Intersect(const RectangleF &a, const RectangleF &b)
			{
				RectangleF rectangle;
				float abrp_x = a.X + a.Width;
				float abrp_y = a.Y + a.Height;

				float bbrp_x = b.X + b.Width;
				float bbrp_y = b.Y + b.Height;
				float num2 = (a.X > b.X) ? a.X : b.X;
				float num = (a.Y > b.Y) ? a.Y : b.Y;
				float num4 = (abrp_x < bbrp_x) ? abrp_x : bbrp_x;
				float num3 = (abrp_y < bbrp_y) ? abrp_y : bbrp_y;
				if ((num4 > num2) && (num3 > num))
				{
					rectangle.X = num2;
					rectangle.Y = num;
					rectangle.Width = num4 - num2;
					rectangle.Height = num3 - num;
					return rectangle;
				}
				rectangle.X = 0;
				rectangle.Y = 0;
				rectangle.Width = 0;
				rectangle.Height = 0;
				return rectangle;
			}
			static RectangleF Union(const RectangleF &a, const RectangleF &b)
			{
				RectangleF result;
				float abrp_x = a.X + a.Width;
				float abrp_y = a.Y + a.Height;

				float bbrp_x = b.X + b.Width;
				float bbrp_y = b.Y + b.Height;
				result.X = (a.X < b.X) ? a.X : b.X;
				result.Y = (a.Y < b.Y) ? a.Y : b.Y;
				result.Width = ((abrp_x > bbrp_x) ? abrp_x : bbrp_x) - result.X;
				result.Height = ((abrp_y > bbrp_y) ? abrp_y : bbrp_y) - result.Y;
				return result;
			}

			friend static bool operator ==(RectangleF a, RectangleF b)
			{
				return (a.X  == b.X) && (b.X == b.Y) && (a.Width == b.Width) && (a.Height == b.Height);
			}
			friend static bool operator !=(RectangleF a, RectangleF b)
			{
				if ((a.X == b.X) && (a.Y == b.Y) && (a.Width == b.Width))
				{
					return (a.Height != b.Height);
				}
				return true;
			}

			const static RectangleF Empty;
		};

	}
}
#endif