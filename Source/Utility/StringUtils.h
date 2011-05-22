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
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#pragma once
#include "Common.h"

using namespace std;

namespace Apoc3D
{
	namespace Utility
	{

		class StringUtils
		{
		public:
			static const String Empty;

			static uint16 ParseUInt16(const String& val);
			static uint32 ParseUInt32(const String& val);
			static uint64 ParseUInt64(const String& val);
			static int16 ParseInt16(const String& val);
			static int32 ParseInt32(const String& val);
			static int64 ParseInt64(const String& val);
			static float ParseSingle(const String& val);
			static double ParseDouble(const String& val);
			static String ToString(int32 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0));
			static String ToString(float val, unsigned short precision = 2, 
				unsigned short width = 0, char fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(0) );
			static String ToString(const wchar_t* val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0));

			static void Trim(String& str, const String& delims = L" \t\r");
			static void TrimLeft(String& str, const String& delims = L" \t\r");
			static void TrimRight(String& str, const String& delims = L" \t\r");
			static vector<String> StringUtils::Split(const String& str, const String& delims = L" ", const int32 preserve = 4);

			static bool StartsWidth(const String& str, const String& v, bool lowerCase);
			static bool EndsWidth(const String& str, const String& v, bool lowerCase);

			static void ToLowerCase(String& str);
			static void ToUpperCase(String& str);
		};
	}
}


#endif