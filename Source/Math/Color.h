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
#ifndef COLOR_H
#define COLOR_H

#include "Common.h"
#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
#pragma pack(push, 16)
		class APOC3D_API Color4
		{
		public:
			union
			{
				struct  
				{
					/* the color's red component.
					*/
					float Red;

					/* the color's green component.
					*/
					float Green;

					/* the color's blue component.
					*/
					float Blue;

					/* the color's alpha component.
					*/
					float Alpha;
				};
				Vector vector;
			};
			
			Color4() { vector = VecLoad(0.0f); }
			Color4(int argb)
			{
				Alpha = static_cast<float>(((argb >> 24) & 0xff) / 255.0f);
				Red = static_cast<float>((float)((argb >> 16) & 0xff) / 255.0f);
				Green = static_cast<float>(((argb >> 8) & 0xff) / 255.0f);
				Blue = static_cast<float>((argb & 0xff) / 255.0f);
			}
			Color4(Vector3 color)
			{
				vector = color;				
			}
			Color4(int red, int green, int blue, int alpha)
			{
				Alpha = static_cast<float>(alpha / 255.0f);
				Red = static_cast<float>(red / 255.0f);
				Green = static_cast<float>(green / 255.0f);
				Blue = static_cast<float>(blue / 255.0f);
			}
			Color4(int red, int green, int blue)
			{
				Alpha = 1;
				Red = static_cast<float>(red / 255.0f);
				Green = static_cast<float>(green / 255.0f);
				Blue = static_cast<float>(blue / 255.0f);
			}
			Color4(float red, float green, float blue)
			{
				Alpha = 1.0f;
				Red = red;
				Green = green;
				Blue = blue;
			}
			Color4(float red, float green, float blue, float alpha)
			{
				Alpha = alpha;
				Red = red;
				Green = green;
				Blue = blue;
			}

			/* Converts the color into a packed integer.
			*/
			uint ToArgb()
			{
				uint a = static_cast<uint>(Alpha * 255.0f);
				uint r = static_cast<uint>(Red * 255.0f);
				uint g = static_cast<uint>(Green * 255.0f);
				uint b = static_cast<uint>(Blue * 255.0f);
				uint result = b;
				result += g << 8;
				result += r << 16;
				result += a << 24;
				return result;
			}

			/* Converts the color into a vector.
			*/
			Vector3 ToVector()
			{
				const float buffer[4]= { Red, Green, Blue, Alpha };
				return VecLoad(buffer);
			}

			/* Adds two colors.
			*/
			static void Add(const Color4 &color1, const Color4 &color2, Color4& result)
			{
				result = Color4(VecAdd(color1.vector, color2.vector));
			}

			/* Adds two colors.
			*/
			static Color4 Add(const Color4 &color1, const Color4 &color2)
			{
				return Color4(VecAdd(color1.vector, color2.vector));
			}

			
			/* Subtracts two colors.
			*/
			static Color4 Subtract(const Color4 &color1, const Color4 &color2)
			{
				return Color4(VecSub(color1.vector, color2.vector));
			}

			/* Subtracts two colors.
			*/
			static Color4 Subtract(const Color4 &color1, const Color4 &color2, Color4& result)
			{
				result = Color4(VecSub(color1.vector, color2.vector));
			}

			/* Modulates two colors.
			*/
			static void Modulate(const Color4 &color1, const Color4 &color2, Color4& result)
			{
				result = VecMul(color1.vector, color2.vector);
			}
			/* Modulates two colors.
			*/
			static Color4 Modulate(const Color4 &color1, const Color4 &color2)
			{
				return VecMul(color1.vector, color2.vector);
			}

			/* Performs a linear interpolation between two colors.
			*/
			static Color4 Lerp(const Color4 &color1, const Color4 &color2, float amount)
			{
				Vector3 b = VecSub(color2.vector, color1.vector);
				b = VecMul(b, amount);
				b = VecAdd(color1.vector, b);
				return b;				
			}
			/* Performs a linear interpolation between two colors.
			*/
			static Color4 Lerp(const Color4 &color1, const Color4 &color2, float amount, Color4& result)
			{
				Vector3 b = VecSub(color2.vector, color1.vector);
				b = VecMul(b, amount);
				b = VecAdd(color1.vector, b);
				result = Color4(b);
			}
			/* Negates a color.
			*/
			static void Negate(const Color4 &color, Color4& result)
			{
				static const Vector3 one = VecLoad(1);
				result = Color4(VecSub(one, color.vector));				
			}
			
			/* Scales a color by the specified amount.
			*/
			static void Scale(const Color4 &color, float scale, Color4& result)
			{
				result = Color4(VecMul(color.vector, scale));
			}
			/* Scales a color by the specified amount.
			*/
			static Color4 Scale(const Color4 &color, float scale)
			{
				return Color4(VecMul(color.vector, scale));
			}
			/* Adjusts the contrast of a color.
			*/
			static void AdjustContrast(const Color4 &color, float contrast, Color4& result)
			{
				static const Vector3 half = VecLoad(0.5f);

				Vector3 t = VecSub(color.vector, half);
				t = VecMul(t, contrast);
				t = VecAdd(t, half);

				result = Color4(t);
				result.Alpha = color.Alpha;
			}
			/* Adjusts the contrast of a color.
			*/
			static Color4 AdjustContrast(const Color4 &color, float contrast)
			{
				static const Vector3 half = VecLoad(0.5f);

				Vector3 t = VecSub(color.vector, half);
				t = VecMul(t, contrast);
				t = VecAdd(t, half);

				Color4 result = Color4(t);
				result.Alpha = color.Alpha;
				return result;
			}
			/* Adjusts the saturation of a color.
			*/
			static void AdjustSaturation(const Color4 &color, float saturation, Color4& result)
			{
				static const float buffer[4] = {0.2125f, 0.7154f, 0.0721f, 0};
				static const Vector3 hue = VecLoad(buffer);
			
				Vector3 grey = Vec3Dot2(hue, color.vector);

				Vector3 c = VecSub(color.vector, grey);
				c = VecMul(c, saturation);
				c = VecAdd(c, grey);

				result = Color4(c);
				result.Alpha = color.Alpha;
			}

			/* Adjusts the saturation of a color.
			*/
			static Color4 AdjustSaturation(const Color4 &color, float saturation)
			{
				static const float buffer[4] = {0.2125f, 0.7154f, 0.0721f, 0};
				static const Vector3 hue = VecLoad(buffer);

				Vector3 grey = Vec3Dot2(hue, color.vector);

				Vector3 c = VecSub(color.vector, grey);
				c = VecMul(c, saturation);
				c = VecAdd(c, grey);

				Color4 result = Color4(c);
				result.Alpha = color.Alpha;
				return result;
			}


		};
#pragma pack(pop)

	}
}

#endif
