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
#define GetColorR(v) ((v>>16)&0xff)
#define GetColorG(v) ((v>>8)&0xff)
#define GetColorB(v) (v & 0xff)
#define GetColorA(v) ((v>>24)&0xff)
		static const ColorValue CV_TransparentBlack = 0;
		static const ColorValue CV_TransparentWhite = 0xffffff;
		static const ColorValue CV_AliceBlue = 0xfff0f8ff;
		static const ColorValue CV_AntiqueWhite = 0xfffaebd7;
		static const ColorValue CV_Aqua = 0xff00ffff;
		static const ColorValue CV_Aquamarine = 0xff7fffd4;
		static const ColorValue CV_Azure = 0xfff0ffff;
		static const ColorValue CV_Beige = 0xfff5f5dc;
		static const ColorValue CV_Bisque = 0xffffe4c4;
		static const ColorValue CV_Black = 0xff000000;
		static const ColorValue CV_BlanchedAlmond = 0xffffebcd;
		static const ColorValue CV_Blue = 0xff0000ff;
		static const ColorValue CV_BlueViolet = 0xff8a2be2;
		static const ColorValue CV_Brown = 0xffa52a2a;
		static const ColorValue CV_BurlyWood = 0xffdeb887;
		static const ColorValue CV_CadetBlue = 0xff5f9ea0;
		static const ColorValue CV_Chartreuse = 0xff7fff00;
		static const ColorValue CV_Chocolate = 0xffd2691e;
		static const ColorValue CV_Coral = 0xffff7f50;
		static const ColorValue CV_CornflowerBlue = 0xff6495ed;
		static const ColorValue CV_Cornsilk = 0xfffff8dc;
		static const ColorValue CV_Crimson = 0xffdc143c;
		static const ColorValue CV_Cyan = 0xff00ffff;
		static const ColorValue CV_DarkBlue = 0xff00008b;
		static const ColorValue CV_DarkCyan = 0xff008b8b;
		static const ColorValue CV_DarkGoldenrod = 0xffb8860b;
		static const ColorValue CV_DarkGray = 0xffa9a9a9;
		static const ColorValue CV_DarkGreen = 0xff006400;
		static const ColorValue CV_DarkKhaki = 0xffbdb76b;
		static const ColorValue CV_DarkMagenta = 0xff8b008b;
		static const ColorValue CV_DarkOliveGreen = 0xff556b2f;
		static const ColorValue CV_DarkOrange = 0xffff8c00;
		static const ColorValue CV_DarkOrchid = 0xff9932cc;
		static const ColorValue CV_DarkRed = 0xff8b0000;
		static const ColorValue CV_DarkSalmon = 0xffe9967a;
		static const ColorValue CV_DarkSeaGreen = 0xff8fbc8b;
		static const ColorValue CV_DarkSlateBlue = 0xff483d8b;
		static const ColorValue CV_DarkSlateGray = 0xff2f4f4f;
		static const ColorValue CV_DarkTurquoise = 0xff00ced1;
		static const ColorValue CV_DarkViolet = 0xff9400d3;
		static const ColorValue CV_DeepPink = 0xffff1493;
		static const ColorValue CV_DeepSkyBlue = 0xff00bfff;
		static const ColorValue CV_DimGray = 0xff696969;
		static const ColorValue CV_DodgerBlue = 0xff1e90ff;
		static const ColorValue CV_Firebrick = 0xffb22222;
		static const ColorValue CV_FloralWhite = 0xfffffaf0;
		static const ColorValue CV_ForestGreen = 0xff228b22;
		static const ColorValue CV_Fuchsia = 0xffff00ff;
		static const ColorValue CV_Gainsboro = 0xffdcdcdc;
		static const ColorValue CV_GhostWhite = 0xfff8f8ff;
		static const ColorValue CV_Gold = 0xffffd700;
		static const ColorValue CV_Goldenrod = 0xffdaa520;
		static const ColorValue CV_Gray = 0xff808080;
		static const ColorValue CV_Green = 0xff008000;
		static const ColorValue CV_GreenYellow = 0xffadff2f;
		static const ColorValue CV_Honeydew = 0xfff0fff0;
		static const ColorValue CV_HotPink = 0xffff69b4;
		static const ColorValue CV_IndianRed = 0xffcd5c5c;
		static const ColorValue CV_Indigo = 0xff4b0082;
		static const ColorValue CV_Ivory = 0xfffffff0;
		static const ColorValue CV_Khaki = 0xfff0e68c;
		static const ColorValue CV_Lavender = 0xffe6e6fa;
		static const ColorValue CV_LavenderBlush = 0xfffff0f5;
		static const ColorValue CV_LawnGreen = 0xff7cfc00;
		static const ColorValue CV_LemonChiffon = 0xfffffacd;
		static const ColorValue CV_LightBlue = 0xffadd8e6;
		static const ColorValue CV_LightCoral = 0xfff08080;
		static const ColorValue CV_LightCyan = 0xffe0ffff;
		static const ColorValue CV_LightGoldenrodYellow = 0xfffafad2;
		static const ColorValue CV_LightGreen = 0xff90ee90;
		static const ColorValue CV_LightGray = 0xffd3d3d3;
		static const ColorValue CV_LightPink = 0xffffb6c1;
		static const ColorValue CV_LightSalmon = 0xffffa07a;
		static const ColorValue CV_LightSeaGreen = 0xff20b2aa;
		static const ColorValue CV_LightSkyBlue = 0xff87cefa;
		static const ColorValue CV_LightSlateGray = 0xff778899;
		static const ColorValue CV_LightSteelBlue = 0xffb0c4de;
		static const ColorValue CV_LightYellow = 0xffffffe0;
		static const ColorValue CV_Lime = 0xff00ff00;
		static const ColorValue CV_LimeGreen = 0xff32cd32;
		static const ColorValue CV_Linen = 0xfffaf0e6;
		static const ColorValue CV_Magenta = 0xffff00ff;
		static const ColorValue CV_Maroon = 0xff800000;
		static const ColorValue CV_MediumAquamarine = 0xff66cdaa;
		static const ColorValue CV_MediumBlue = 0xff0000cd;
		static const ColorValue CV_MediumOrchid = 0xffba55d3;
		static const ColorValue CV_MediumPurple = 0xff9370db;
		static const ColorValue CV_MediumSeaGreen = 0xff3cb371;
		static const ColorValue CV_MediumSlateBlue = 0xff7b68ee;
		static const ColorValue CV_MediumSpringGreen = 0xff00fa9a;
		static const ColorValue CV_MediumTurquoise = 0xff48d1cc;
		static const ColorValue CV_MediumVioletRed = 0xffc71585;
		static const ColorValue CV_MidnightBlue = 0xff191970;
		static const ColorValue CV_MintCream = 0xfff5fffa;
		static const ColorValue CV_MistyRose = 0xffffe4e1;
		static const ColorValue CV_Moccasin = 0xffffe4b5;
		static const ColorValue CV_NavajoWhite = 0xffffdead;
		static const ColorValue CV_Navy = 0xff000080;
		static const ColorValue CV_OldLace = 0xfffdf5e6;
		static const ColorValue CV_Olive = 0xff808000;
		static const ColorValue CV_OliveDrab = 0xff6b8e23;
		static const ColorValue CV_Orange = 0xffffa500;
		static const ColorValue CV_OrangeRed = 0xffff4500;
		static const ColorValue CV_Orchid = 0xffda70d6;
		static const ColorValue CV_PaleGoldenrod = 0xffeee8aa;
		static const ColorValue CV_PaleGreen = 0xff98fb98;
		static const ColorValue CV_PaleTurquoise = 0xffafeeee;
		static const ColorValue CV_PaleVioletRed = 0xffdb7093;
		static const ColorValue CV_PapayaWhip = 0xffffefd5;
		static const ColorValue CV_PeachPuff = 0xffffdab9;
		static const ColorValue CV_Peru = 0xffcd853f;
		static const ColorValue CV_Pink = 0xffffc0cb;
		static const ColorValue CV_Plum = 0xffdda0dd;
		static const ColorValue CV_PowderBlue = 0xffb0e0e6;
		static const ColorValue CV_Purple = 0xff800080;
		static const ColorValue CV_Red = 0xffff0000;
		static const ColorValue CV_RosyBrown = 0xffbc8f8f;
		static const ColorValue CV_RoyalBlue = 0xff4169e1;
		static const ColorValue CV_SaddleBrown = 0xff8b4513;
		static const ColorValue CV_Salmon = 0xfffa8072;
		static const ColorValue CV_SandyBrown = 0xfff4a460;
		static const ColorValue CV_SeaGreen = 0xff2e8b57;
		static const ColorValue CV_SeaShell = 0xfffff5ee;
		static const ColorValue CV_Sienna = 0xffa0522d;
		static const ColorValue CV_Silver = 0xffc0c0c0;
		static const ColorValue CV_SkyBlue = 0xff87ceeb;
		static const ColorValue CV_SlateBlue = 0xff6a5acd;
		static const ColorValue CV_SlateGray = 0xff708090;
		static const ColorValue CV_Snow = 0xfffffafa;
		static const ColorValue CV_SpringGreen = 0xff00ff7f;
		static const ColorValue CV_SteelBlue = 0xff4682b4;
		static const ColorValue CV_Tan = 0xffd2b48c;
		static const ColorValue CV_Teal = 0xff008080;
		static const ColorValue CV_Thistle = 0xffd8bfd8;
		static const ColorValue CV_Tomato = 0xffff6347;
		static const ColorValue CV_Turquoise = 0xff40e0d0;
		static const ColorValue CV_Violet = 0xffee82ee;
		static const ColorValue CV_Wheat = 0xfff5deb3;
		static const ColorValue CV_White = 0xffffffff;
		static const ColorValue CV_WhiteSmoke = 0xfff5f5f5;
		static const ColorValue CV_Yellow = 0xffffff00;
		static const ColorValue CV_YellowGreen = 0xff9acd32;
	}
}

#endif