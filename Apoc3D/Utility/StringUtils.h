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
#include "apoc3d/Utility/Hash.h"

namespace Apoc3D
{
	namespace Utility
	{
		class APAPI StringUtils
		{
		public:
			static const String Empty;

			static std::string toPlatformNarrowString(const String& str);
			static std::string toPlatformNarrowString(const wchar_t* str);
			static String toPlatformWideString(const std::string& str);
			static String toPlatformWideString(const char* str);


			static std::string toASCIINarrowString(const String& str);
			static String toASCIIWideString(const std::string& str);
			

			static String UTF8toUTF16(const std::string& utf8);
			static std::string UTF16toUTF8(const String& utf16);

			static String32 UTF8toUTF32(const std::string& utf8);
			static std::string UTF32toUTF8(const String32& utf32);

			static String32 UTF16toUTF32(const String& utf16);
			static String UTF32toUTF16(const String32& utf32);




			static bool ParseBool(const String& val);
			static uint16 ParseUInt16(const String& val);
			static uint32 ParseUInt32(const String& val);
			static uint32 ParseUInt32Hex(const String& val);
			static uint64 ParseUInt64(const String& val);
			static uint64 ParseUInt64Hex(const String& val);
			static uint64 ParseUInt64Bin(const String& val);
			static int16 ParseInt16(const String& val);
			static int32 ParseInt32(const String& val);
			static int64 ParseInt64(const String& val);
			static float ParseSingle(const String& val);
			static double ParseDouble(const String& val);

			static bool ParseBool(const std::string& val);
			static uint16 ParseUInt16(const std::string& val);
			static uint32 ParseUInt32(const std::string& val);
			static uint32 ParseUInt32Hex(const std::string& val);
			static uint64 ParseUInt64(const std::string& val);
			static uint64 ParseUInt64Hex(const std::string& val);
			static uint64 ParseUInt64Bin(const std::string& val);
			static int16 ParseInt16(const std::string& val);
			static int32 ParseInt32(const std::string& val);
			static int64 ParseInt64(const std::string& val);
			static float ParseSingle(const std::string& val);
			static double ParseDouble(const std::string& val);



			static String IntToString(int16 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static String IntToString(int32 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static String IntToString(int64 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static String SingleToString(float val, uint16 precision = 6, uint16 width = 0, wchar_t fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(std::ios::fixed) );
			
			static String UIntToString(uint16 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static String UIntToString(uint32 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static String UIntToString(uint64 val, uint16 width = 0, wchar_t fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

			static String UIntToStringHex(uint32 val, uint16 width = 0);
			static String UIntToStringHex(uint64 val, uint16 width = 0);
			static String UIntToStringBin(uint64 val);

			static String BoolToString(bool val);



			static std::string IntToNarrowString(int16 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static std::string IntToNarrowString(int32 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static std::string IntToNarrowString(int64 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static std::string SingleToNarrowString(float val, uint16 precision = 6, uint16 width = 0, char fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(std::ios::fixed) );
			
			static std::string UIntToNarrowString(uint16 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static std::string UIntToNarrowString(uint32 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			static std::string UIntToNarrowString(uint64 val, uint16 width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

			static std::string UIntToNarrowStringHex(uint32 val, uint16 width = 0);
			static std::string UIntToNarrowStringHex(uint64 val, uint16 width = 0);
			static std::string UIntToNarrowStringBin(uint64 val);

			static std::string BoolToNarrowString(bool val);




			static void Trim(String& str, const String& delims = L" \t\r");
			static void TrimLeft(String& str, const String& delims = L" \t\r");
			static void TrimRight(String& str, const String& delims = L" \t\r");
			static void Split(const String& str, Apoc3D::Collections::List<String>& results, const String& delims = L" ");
			static void Split(const std::string& str, Apoc3D::Collections::List<std::string>& results, const std::string& delims = " ");

			static bool StartsWith(const String& str, const String& v, bool caseInsensitive = false);
			static bool EndsWith(const String& str, const String& v, bool caseInsensitive = false);

			static void ToLowerCase(String& str);
			static void ToUpperCase(String& str);

			static uint GetHashCode(const String& str)
			{
				return MurmurHash(str.c_str(), sizeof(wchar_t)*str.length());
			}
		};
	}
}


#endif