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
#ifndef COLORVALUE_H
#define COLORVALUE_H

#include "Common.h"

namespace Apoc3D
{
	namespace Math
	{
		/** Packed ARGB value, 8 bit per channel
		*/
		typedef uint ColorValue;
#define UNPACK_COLOR(color, r,g,b,a) a = (color>>24); r = (0xff & (color>>16)); g = (0xff & (color>>8)); b = (0xff & color);
#define PACK_COLOR(r,g,b,a) ( (static_cast<uint>(a)<<24) | (static_cast<uint>(r)<<16) | (static_cast<uint>(g)<<8) | (static_cast<uint>(b)) )

		static const ColorValue TransparentBlack = 0;
		static const ColorValue TransparentWhite = 0xffffff;
		static const ColorValue AliceBlue = 0xfff0f8ff;
		static const ColorValue AntiqueWhite = 0xfffaebd7;
		static const ColorValue Aqua = 0xff00ffff;
		static const ColorValue Aquamarine = 0xff7fffd4;
		static const ColorValue Azure = 0xfff0ffff;
		static const ColorValue Beige = 0xfff5f5dc;
		static const ColorValue Bisque = 0xffffe4c4;
		static const ColorValue Black = 0xff000000;
		static const ColorValue BlanchedAlmond = 0xffffebcd;
		static const ColorValue Blue = 0xff0000ff;
		static const ColorValue BlueViolet = 0xff8a2be2;
		static const ColorValue Brown = 0xffa52a2a;
		static const ColorValue BurlyWood = 0xffdeb887;
		static const ColorValue CadetBlue = 0xff5f9ea0;
		static const ColorValue Chartreuse = 0xff7fff00;
		static const ColorValue Chocolate = 0xffd2691e;
		static const ColorValue Coral = 0xffff7f50;
		static const ColorValue CornflowerBlue = 0xff6495ed;
		static const ColorValue Cornsilk = 0xfffff8dc;
		static const ColorValue Crimson = 0xffdc143c;
		static const ColorValue Cyan = 0xff00ffff;
		static const ColorValue DarkBlue = 0xff00008b;
		static const ColorValue DarkCyan = 0xff008b8b;
		static const ColorValue DarkGoldenrod = 0xffb8860b;
		static const ColorValue DarkGray = 0xffa9a9a9;
		static const ColorValue DarkGreen = 0xff006400;
		static const ColorValue DarkKhaki = 0xffbdb76b;
		static const ColorValue DarkMagenta = 0xff8b008b;
		static const ColorValue DarkOliveGreen = 0xff556b2f;
		static const ColorValue DarkOrange = 0xffff8c00;
		static const ColorValue DarkOrchid = 0xff9932cc;
		static const ColorValue DarkRed = 0xff8b0000;
		static const ColorValue DarkSalmon = 0xffe9967a;
		static const ColorValue DarkSeaGreen = 0xff8fbc8b;
		static const ColorValue DarkSlateBlue = 0xff483d8b;
		static const ColorValue DarkSlateGray = 0xff2f4f4f;
		static const ColorValue DarkTurquoise = 0xff00ced1;
		static const ColorValue DarkViolet = 0xff9400d3;
		static const ColorValue DeepPink = 0xffff1493;
		static const ColorValue DeepSkyBlue = 0xff00bfff;
		static const ColorValue DimGray = 0xff696969;
		static const ColorValue DodgerBlue = 0xff1e90ff;
		static const ColorValue Firebrick = 0xffb22222;
		static const ColorValue FloralWhite = 0xfffffaf0;
		static const ColorValue ForestGreen = 0xff228b22;
		static const ColorValue Fuchsia = 0xffff00ff;
		static const ColorValue Gainsboro = 0xffdcdcdc;
		static const ColorValue GhostWhite = 0xfff8f8ff;
		static const ColorValue Gold = 0xffffd700;
		static const ColorValue Goldenrod = 0xffdaa520;
		static const ColorValue Gray = 0xff808080;
		static const ColorValue Green = 0xff008000;
		static const ColorValue GreenYellow = 0xffadff2f;
		static const ColorValue Honeydew = 0xfff0fff0;
		static const ColorValue HotPink = 0xffff69b4;
		static const ColorValue IndianRed = 0xffcd5c5c;
		static const ColorValue Indigo = 0xff4b0082;
		static const ColorValue Ivory = 0xfffffff0;
		static const ColorValue Khaki = 0xfff0e68c;
		static const ColorValue Lavender = 0xffe6e6fa;
		static const ColorValue LavenderBlush = 0xfffff0f5;
		static const ColorValue LawnGreen = 0xff7cfc00;
		static const ColorValue LemonChiffon = 0xfffffacd;
		static const ColorValue LightBlue = 0xffadd8e6;
		static const ColorValue LightCoral = 0xfff08080;
		static const ColorValue LightCyan = 0xffe0ffff;
		static const ColorValue LightGoldenrodYellow = 0xfffafad2;
		static const ColorValue LightGreen = 0xff90ee90;
		static const ColorValue LightGray = 0xffd3d3d3;
		static const ColorValue LightPink = 0xffffb6c1;
		static const ColorValue LightSalmon = 0xffffa07a;
		static const ColorValue LightSeaGreen = 0xff20b2aa;
		static const ColorValue LightSkyBlue = 0xff87cefa;
		static const ColorValue LightSlateGray = 0xff778899;
		static const ColorValue LightSteelBlue = 0xffb0c4de;
		static const ColorValue LightYellow = 0xffffffe0;
		static const ColorValue Lime = 0xff00ff00;
		static const ColorValue LimeGreen = 0xff32cd32;
		static const ColorValue Linen = 0xfffaf0e6;
		static const ColorValue Magenta = 0xffff00ff;
		static const ColorValue Maroon = 0xff800000;
		static const ColorValue MediumAquamarine = 0xff66cdaa;
		static const ColorValue MediumBlue = 0xff0000cd;
		static const ColorValue MediumOrchid = 0xffba55d3;
		static const ColorValue MediumPurple = 0xff9370db;
		static const ColorValue MediumSeaGreen = 0xff3cb371;
		static const ColorValue MediumSlateBlue = 0xff7b68ee;
		static const ColorValue MediumSpringGreen = 0xff00fa9a;
		static const ColorValue MediumTurquoise = 0xff48d1cc;
		static const ColorValue MediumVioletRed = 0xffc71585;
		static const ColorValue MidnightBlue = 0xff191970;
		static const ColorValue MintCream = 0xfff5fffa;
		static const ColorValue MistyRose = 0xffffe4e1;
		static const ColorValue Moccasin = 0xffffe4b5;
		static const ColorValue NavajoWhite = 0xffffdead;
		static const ColorValue Navy = 0xff000080;
		static const ColorValue OldLace = 0xfffdf5e6;
		static const ColorValue Olive = 0xff808000;
		static const ColorValue OliveDrab = 0xff6b8e23;
		static const ColorValue Orange = 0xffffa500;
		static const ColorValue OrangeRed = 0xffff4500;
		static const ColorValue Orchid = 0xffda70d6;
		static const ColorValue PaleGoldenrod = 0xffeee8aa;
		static const ColorValue PaleGreen = 0xff98fb98;
		static const ColorValue PaleTurquoise = 0xffafeeee;
		static const ColorValue PaleVioletRed = 0xffdb7093;
		static const ColorValue PapayaWhip = 0xffffefd5;
		static const ColorValue PeachPuff = 0xffffdab9;
		static const ColorValue Peru = 0xffcd853f;
		static const ColorValue Pink = 0xffffc0cb;
		static const ColorValue Plum = 0xffdda0dd;
		static const ColorValue PowderBlue = 0xffb0e0e6;
		static const ColorValue Purple = 0xff800080;
		static const ColorValue Red = 0xffff0000;
		static const ColorValue RosyBrown = 0xffbc8f8f;
		static const ColorValue RoyalBlue = 0xff4169e1;
		static const ColorValue SaddleBrown = 0xff8b4513;
		static const ColorValue Salmon = 0xfffa8072;
		static const ColorValue SandyBrown = 0xfff4a460;
		static const ColorValue SeaGreen = 0xff2e8b57;
		static const ColorValue SeaShell = 0xfffff5ee;
		static const ColorValue Sienna = 0xffa0522d;
		static const ColorValue Silver = 0xffc0c0c0;
		static const ColorValue SkyBlue = 0xff87ceeb;
		static const ColorValue SlateBlue = 0xff6a5acd;
		static const ColorValue SlateGray = 0xff708090;
		static const ColorValue Snow = 0xfffffafa;
		static const ColorValue SpringGreen = 0xff00ff7f;
		static const ColorValue SteelBlue = 0xff4682b4;
		static const ColorValue Tan = 0xffd2b48c;
		static const ColorValue Teal = 0xff008080;
		static const ColorValue Thistle = 0xffd8bfd8;
		static const ColorValue Tomato = 0xffff6347;
		static const ColorValue Turquoise = 0xff40e0d0;
		static const ColorValue Violet = 0xffee82ee;
		static const ColorValue Wheat = 0xfff5deb3;
		static const ColorValue White = 0xffffffff;
		static const ColorValue WhiteSmoke = 0xfff5f5f5;
		static const ColorValue Yellow = 0xffffff00;
		static const ColorValue YellowGreen = 0xff9acd32;
	}
}

#endif