#pragma once
#ifndef APOC3D_STRINGUTILS_H
#define APOC3D_STRINGUTILS_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Utility
	{
		class APAPI StringUtils
		{
		public:
			static const String Empty;

			static std::string toString(const String& str);
			static std::string toString(const wchar_t* str);
			static String toWString(const std::string& str);
			static String toWString(const char* str);

			static bool ParseBool(const String& val);

			static uint16 ParseUInt16(const String& val);
			static uint32 ParseUInt32(const String& val);
			static uint32 ParseUInt32Hex(const String& val);
			static uint64 ParseUInt64(const String& val);
			static uint64 ParseUInt64Bin(const String& val);
			static int16 ParseInt16(const String& val);
			static int32 ParseInt32(const String& val);
			static int64 ParseInt64(const String& val);
			static float ParseSingle(const String& val);
			static double ParseDouble(const String& val);
			static String ToString(int16 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0))
			{
				return ToString(static_cast<int64>(val), width, fill, flags);
			}
			static String ToString(int32 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0))
			{
				return ToString(static_cast<int64>(val), width, fill, flags);
			}
			static String ToString(int64 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0));
			static String ToString(float val, unsigned short precision = 10, 
				unsigned short width = 0, char fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(std::ios::fixed) );
			static String ToString(const wchar_t* val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0));

			static String ToString(uint16 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0))
			{
				return ToString(static_cast<uint64>(val), width, fill, flags);
			}
			static String ToString(uint32 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0))
			{
				return ToString(static_cast<uint64>(val), width, fill, flags);
			}
			static String ToStringHex(uint32 val, unsigned short width=0);
			static String ToStringHex(uint64 val, unsigned short width=0);
			static String ToStringBin(uint64 val);

			static String ToString(uint64 val, 
				unsigned short width=0, wchar_t fill=' ', std::ios::fmtflags flags= std::ios::fmtflags(0));

			static String ToString(bool val);

			static void Trim(String& str, const String& delims = L" \t\r");
			static void TrimLeft(String& str, const String& delims = L" \t\r");
			static void TrimRight(String& str, const String& delims = L" \t\r");
			static void StringUtils::Split(const String& str, Apoc3D::Collections::List<String>& results, const String& delims = L" ");

			static bool StartsWidth(const String& str, const String& v, bool caseInsensitive = false);
			static bool EndsWidth(const String& str, const String& v, bool caseInsensitive = false);

			static void ToLowerCase(String& str);
			static void ToUpperCase(String& str);

			static uint GetHashCode(const String& str)
			{
				// This may read "outside" buffer. 
				// Not really for String because it is null terminated having an extra wchar.
				uint even = 0x15051505;
				uint odd = even;
				const uint* data = reinterpret_cast<const uint*>(str.c_str());
				for (int i = str.size(); i > 0; i -= 4)
				{
					even = ((even << 5) + even + (even >> 0x1b)) ^ data[0];
					if (i <= 2)
					{
						break;
					}
					odd = ((odd << 5) + odd + (odd >> 0x1b)) ^ data[1];
					data += 2;
				}
				return even + odd * 0x5d588b65;
			}
		};
	}
}


#endif