#include "Rectangle.h"
#include "Math.h"

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

		RectangleF RectangleF::Intersect(const RectangleF& a, const RectangleF& b) { return IntersectRect(a, b); }
		RectangleF RectangleF::Union(const RectangleF& a, const RectangleF& b) { return UnionRect(a, b); }

		RectangleF RectangleF::Lerp(const RectangleF& r1, const RectangleF& r2, float amount)
		{
			return{ Math::Lerp(r1.X, r2.X, amount),
				Math::Lerp(r1.Y, r2.Y, amount),
				Math::Lerp(r1.Width, r2.Width, amount),
				Math::Lerp(r1.Height, r2.Height, amount) };
		}


		Rectangle Rectangle::Intersect(const Rectangle& a, const Rectangle& b) { return IntersectRect(a, b); }
		Rectangle Rectangle::Union(const Rectangle& a, const Rectangle& b) { return UnionRect(a, b); }

	}
}