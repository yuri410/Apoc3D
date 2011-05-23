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


namespace Apoc3D
{
	namespace Math
	{
		/** Packed ARGB value, 8 bit per channel
		*/
		typedef uint ColorValue;
#define UNPACK_COLOR(color, r,g,b,a) a = (color>>24); r = (0xff & (color>>16)); g = (0xff & (color>>8)); b = (0xff & color);
#define PACK_COLOR(r,g,b,a) ( (static_cast<uint>(a)<<24) | (static_cast<uint>(r)<<16) | (static_cast<uint>(g)<<8) | (static_cast<uint>(b)) )
	}
}

#endif