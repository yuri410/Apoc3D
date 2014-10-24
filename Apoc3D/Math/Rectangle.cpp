#include "Rectangle.h"
#include "Math.h"

namespace Apoc3D
{
	namespace Math
	{
		RectangleF RectangleF::Intersect(const RectangleF &a, const RectangleF &b)
		{
			RectangleF rectangle;
			float abrp_x = a.X + a.Width;
			float abrp_y = a.Y + a.Height;

			float bbrp_x = b.X + b.Width;
			float bbrp_y = b.Y + b.Height;
			float maxX = Max(a.X, b.X);
			float maxY = Max(a.Y, b.Y);
			float minRight = Min(abrp_x, bbrp_x);
			float minBottom = Min(abrp_y, bbrp_y);
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
		RectangleF RectangleF::Union(const RectangleF &a, const RectangleF &b)
		{
			RectangleF result;
			float abrp_x = a.X + a.Width;
			float abrp_y = a.Y + a.Height;

			float bbrp_x = b.X + b.Width;
			float bbrp_y = b.Y + b.Height;
			result.X = Min(a.X, b.X);
			result.Y = Min(a.Y, b.Y);
			result.Width = Max(abrp_x, bbrp_x) - result.X;
			result.Height = Max(abrp_y, bbrp_y) - result.Y;
			return result;
		}


		Rectangle Rectangle::Intersect(const Rectangle &a, const Rectangle &b)
		{
			Rectangle rectangle;
			int abrp_x = a.X + a.Width;
			int abrp_y = a.Y + a.Height;

			int bbrp_x = b.X + b.Width;
			int bbrp_y = b.Y + b.Height;
			int maxX = Max(a.X, b.X);
			int maxY = Max(a.Y, b.Y);
			int minRight = Min(abrp_x, bbrp_x);
			int minBottom = Min(abrp_y, bbrp_y);
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

		Rectangle Rectangle::Union(const Rectangle &a, const Rectangle &b)
		{
			Rectangle result;
			int abrp_x = a.X + a.Width;
			int abrp_y = a.Y + a.Height;

			int bbrp_x = b.X + b.Width;
			int bbrp_y = b.Y + b.Height;
			result.X = Min(a.X, b.X);
			result.Y = Min(a.Y, b.Y);
			result.Width = Max(abrp_x, bbrp_x) - result.X;
			result.Height = Max(abrp_y, bbrp_y) - result.Y;
			return result;
		}

	}
}