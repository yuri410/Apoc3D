#include "Rectangle.h"
#include "Math.h"
#include "apoc3d/UILib/UICommon.h"

namespace Apoc3D
{
	namespace Math
	{
		template <typename RectType>
		RectType IntersectRect(const RectType& a, const RectType& b)
		{
			using ET = decltype(a.X);

			RectType rectangle;
			ET abrp_x = a.X + a.Width;
			ET abrp_y = a.Y + a.Height;

			ET bbrp_x = b.X + b.Width;
			ET bbrp_y = b.Y + b.Height;
			ET maxX = Max(a.X, b.X);
			ET maxY = Max(a.Y, b.Y);
			ET minRight = Min(abrp_x, bbrp_x);
			ET minBottom = Min(abrp_y, bbrp_y);

			if (minRight > maxX && minBottom > maxY)
			{
				rectangle.X = maxX;
				rectangle.Y = maxY;
				rectangle.Width = minRight - maxX;
				rectangle.Height = minBottom - maxY;
				return rectangle;
			}
			return rectangle;
		}

		template <typename RectType>
		RectType UnionRect(const RectType& a, const RectType& b)
		{
			using ET = decltype(a.X);

			RectType result;
			ET abrp_x = a.X + a.Width;
			ET abrp_y = a.Y + a.Height;

			ET bbrp_x = b.X + b.Width;
			ET bbrp_y = b.Y + b.Height;
			result.X = Min(a.X, b.X);
			result.Y = Min(a.Y, b.Y);
			result.Width = Max(abrp_x, bbrp_x) - result.X;
			result.Height = Max(abrp_y, bbrp_y) - result.Y;
			return result;
		}

		//////////////////////////////////////////////////////////////////////////

		RectangleF RectangleF::GetCenterRegion(const PointF& sz) const
		{
			return{ getCenter() - sz / 2, sz };
		}

		RectangleF RectangleF::Offsetted(PointF ofs) const
		{
			RectangleF c = *this;
			c.Offset(ofs);
			return c;
		}

		RectangleF RectangleF::Offsetted(float x, float y) const
		{
			RectangleF c = *this;
			c.Offset(x, y);
			return c;
		}

		bool RectangleF::operator==(const RectangleF& other) const
		{
			return (X == other.X) && (Y == other.Y) && (Width == other.Width) && (Height == other.Height);
		}
		
		bool RectangleF::operator!=(const RectangleF& other) const
		{
			return !(*this == other);
		}

		RectangleF RectangleF::Intersect(const RectangleF& a, const RectangleF& b)
		{
			return IntersectRect(a, b);
		}

		RectangleF RectangleF::Union(const RectangleF& a, const RectangleF& b)
		{
			return UnionRect(a, b);
		}

		RectangleF RectangleF::Lerp(const RectangleF& r1, const RectangleF& r2, float amount)
		{
			return{ Math::Lerp(r1.X, r2.X, amount),
				Math::Lerp(r1.Y, r2.Y, amount),
				Math::Lerp(r1.Width, r2.Width, amount),
				Math::Lerp(r1.Height, r2.Height, amount) };
		}

		//////////////////////////////////////////////////////////////////////////

		Rectangle Rectangle::GetCenterRegion(const Point& sz) const
		{
			return{ getCenter() - sz / 2, sz };
		}

		Rectangle Rectangle::Offsetted(Point ofs) const
		{
			Rectangle c = *this;
			c.Offset(ofs);
			return c;
		}

		Rectangle Rectangle::Offsetted(int32 x, int32 y) const
		{
			Rectangle c = *this;
			c.Offset(x, y);
			return c;
		}

		Rectangle::operator RectangleF() const
		{
			return RectangleF(static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Width), static_cast<float>(Height));
		}

		bool Rectangle::operator==(const Rectangle& other) const
		{
			return (X == other.X) && (Y == other.Y) && (Width == other.Width) && (Height == other.Height);
		}

		bool Rectangle::operator!=(const Rectangle& other) const 
		{
			return !(*this == other); 
		}

		void Rectangle::SplitVert(int32 ypos, Rectangle& top, Rectangle& bottom)
		{
			top = *this;
			top.Height = ypos;

			bottom = *this;
			bottom.Y = Y + ypos;
			bottom.Height = Height - (ypos - Y);
		}

		void Rectangle::SplitHorizontal(int32 xpos, Rectangle& left, Rectangle& right)
		{
			left = *this;
			left.X = X;
			left.Width = xpos;

			right = *this;
			right.X = X + xpos;
			right.Width = Width - (xpos - X);
		}

		void Rectangle::DivideTo9Regions(int32 left, int32 right, int32 top, int32 bottom, Rectangle(&result)[9]) const
		{
			UI::ControlBounds bounds(left, right, top, bottom);

			result[1] = result[4] = result[7] =
				result[3] = result[5] = bounds.ShrinkRect(*this);

			result[1].Y = Y;
			result[1].Height = top;

			result[7].Y = result[4].getBottom();
			result[7].Height = bottom;

			result[3].X = X;
			result[3].Width = left;

			result[5].X = result[4].getRight();
			result[5].Width = right;

			result[0] = { getPosition(), { left, top } };
			result[2] = { result[1].getTopRight(), { right, top } };

			result[6] = { result[3].getBottomLeft(), { left, bottom } };
			result[8] = { result[4].getBottomRight(), { right, bottom } };
		}
		
		void Rectangle::DivideTo3RegionsX(int32 left, int32 right, Rectangle(&result)[3]) const
		{
			result[0] = { X, Y, left, Height };
			result[1] = { X + left, Y, Width - left - right, Height };
			result[2] = { X + Width - right, Y, right, Height };
		}

		void Rectangle::DivideTo3RegionsY(int32 top, int32 bottom, Rectangle(&result)[3]) const
		{
			result[0] = { X, Y, Width, top };
			result[1] = { X, Y + top, Width, Height - top - bottom };
			result[2] = { X, Y + Height - bottom, Width, bottom };
		}

		Rectangle Rectangle::Intersect(const Rectangle& a, const Rectangle& b)
		{
			return IntersectRect(a, b);
		}

		Rectangle Rectangle::Union(const Rectangle& a, const Rectangle& b)
		{
			return UnionRect(a, b);
		}
	}
}