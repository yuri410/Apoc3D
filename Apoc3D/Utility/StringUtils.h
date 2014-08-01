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
		namespace StringUtils
		{
			APAPI extern const String Empty;

			APAPI std::string toPlatformNarrowString(const String& str);
			APAPI std::string toPlatformNarrowString(const wchar_t* str);
			APAPI String toPlatformWideString(const std::string& str);
			APAPI String toPlatformWideString(const char* str);


			APAPI std::string toASCIINarrowString(const String& str);
			APAPI String toASCIIWideString(const std::string& str);
			

			APAPI String UTF8toUTF16(const std::string& utf8);
			APAPI std::string UTF16toUTF8(const String& utf16);

			APAPI String32 UTF8toUTF32(const std::string& utf8);
			APAPI std::string UTF32toUTF8(const String32& utf32);

			APAPI String32 UTF16toUTF32(const String& utf16);
			APAPI String UTF32toUTF16(const String32& utf32);




			APAPI bool ParseBool(const String& val);
			APAPI uint16 ParseUInt16(const String& val);
			APAPI uint32 ParseUInt32(const String& val);
			APAPI uint32 ParseUInt32Hex(const String& val);
			APAPI uint64 ParseUInt64(const String& val);
			APAPI uint64 ParseUInt64Hex(const String& val);
			APAPI uint64 ParseUInt64Bin(const String& val);
			APAPI int16 ParseInt16(const String& val);
			APAPI int32 ParseInt32(const String& val);
			APAPI int64 ParseInt64(const String& val);
			APAPI float ParseSingle(const String& val);
			APAPI double ParseDouble(const String& val);

			APAPI bool ParseBool(const std::string& val);
			APAPI uint16 ParseUInt16(const std::string& val);
			APAPI uint32 ParseUInt32(const std::string& val);
			APAPI uint32 ParseUInt32Hex(const std::string& val);
			APAPI uint64 ParseUInt64(const std::string& val);
			APAPI uint64 ParseUInt64Hex(const std::string& val);
			APAPI uint64 ParseUInt64Bin(const std::string& val);
			APAPI int16 ParseInt16(const std::string& val);
			APAPI int32 ParseInt32(const std::string& val);
			APAPI int64 ParseInt64(const std::string& val);
			APAPI float ParseSingle(const std::string& val);
			APAPI double ParseDouble(const std::string& val);



			APAPI String IntToString(int16 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI String IntToString(int32 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI String IntToString(int64 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI String SingleToString(float val, uint16 precision = 6, uint16 width = 0, wchar_t fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(std::ios::fixed) );
			
			APAPI String UIntToString(uint16 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI String UIntToString(uint32 val, uint16 width=0, wchar_t fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI String UIntToString(uint64 val, uint16 width = 0, wchar_t fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

			APAPI String UIntToStringHex(uint32 val, uint16 width = 0);
			APAPI String UIntToStringHex(uint64 val, uint16 width = 0);
			APAPI String UIntToStringBin(uint64 val);

			APAPI String BoolToString(bool val);



			APAPI std::string IntToNarrowString(int16 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI std::string IntToNarrowString(int32 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI std::string IntToNarrowString(int64 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI std::string SingleToNarrowString(float val, uint16 precision = 6, uint16 width = 0, char fill = ' ', 
				std::ios::fmtflags flags = std::ios::fmtflags(std::ios::fixed) );
			
			APAPI std::string UIntToNarrowString(uint16 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI std::string UIntToNarrowString(uint32 val, uint16 width=0, char fill=' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
			APAPI std::string UIntToNarrowString(uint64 val, uint16 width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));

			APAPI std::string UIntToNarrowStringHex(uint32 val, uint16 width = 0);
			APAPI std::string UIntToNarrowStringHex(uint64 val, uint16 width = 0);
			APAPI std::string UIntToNarrowStringBin(uint64 val);

			APAPI std::string BoolToNarrowString(bool val);


			APAPI bool EqualsNoCase(const String& a, const String& b);
			APAPI bool EqualsNoCase(const std::string& a, const std::string& b);

			APAPI bool Match(const String& str, const String& pattern);


			APAPI void Trim(String& str, const String& delims = L" \t\r");
			APAPI void TrimLeft(String& str, const String& delims = L" \t\r");
			APAPI void TrimRight(String& str, const String& delims = L" \t\r");
			APAPI void Split(const String& str, Apoc3D::Collections::List<String>& results, const String& delims = L" ");
			APAPI void Split(const std::string& str, Apoc3D::Collections::List<std::string>& results, const std::string& delims = " ");

			APAPI int32 SplitParseSingles(const String& str, float* flts, int32 maxCount, const String& delims = L" ");
			APAPI void SplitParseSingles(const String& str, Apoc3D::Collections::List<float>& results, const String& delims = L" ");
			inline void SplitParseSinglesChecked(const String& str, float* flts, int32 maxCount, const String& delims = L" ")
			{
				int32 r = SplitParseSingles(str, flts, maxCount, delims); assert(r == maxCount);
			}
			template <int32 N>
			void SplitParseSinglesChecked(const String& str, float (&flts)[N], const String& delims = L" ") { SplitParseSinglesChecked(str, flts, N, delims); }

			APAPI int32 SplitParseSingles(const std::string& str, float* flts, int32 maxCount, const std::string& delims = " ");
			APAPI void SplitParseSingles(const std::string& str, Apoc3D::Collections::List<float>& results, const std::string& delims = " ");
			inline void SplitParseSinglesChecked(const std::string& str, float* flts, int32 maxCount, const std::string& delims = " ")
			{
				int32 r = SplitParseSingles(str, flts, maxCount, delims); assert(r == maxCount);
			}
			template <int32 N>
			void SplitParseSinglesChecked(const std::string& str, float (&flts)[N], const std::string& delims = " ") { SplitParseSinglesChecked(str, flts, N, delims); }


			APAPI int32 SplitParseInts(const String& str, int32* ints, int32 maxCount, const String& delims = L" ");
			APAPI void SplitParseInts(const String& str, Apoc3D::Collections::List<int32>& results, const String& delims = L" ");
			inline void SplitParseIntsChecked(const String& str, int32* ints, int32 maxCount, const String& delims = L" ")
			{
				int32 r = SplitParseInts(str, ints, maxCount, delims); assert(r == maxCount);
			}
			template <int32 N>
			void SplitParseIntsChecked(const String& str, int32 (&ints)[N], const String& delims = L" ") { SplitParseIntsChecked(str, ints, N, delims); }

			APAPI int32 SplitParseInts(const std::string& str, int32* ints, int32 maxCount, const std::string& delims = " ");
			APAPI void SplitParseInts(const std::string& str, Apoc3D::Collections::List<int32>& results, const std::string& delims = " ");
			inline void SplitParseIntsChecked(const std::string& str, int32* ints, int32 maxCount, const std::string& delims = " ")
			{
				int32 r = SplitParseInts(str, ints, maxCount, delims); assert(r == maxCount);
			}
			template <int32 N>
			void SplitParseIntsChecked(const std::string& str, int32 (&ints)[N], const std::string& delims = " ") { SplitParseIntsChecked(str, ints, N, delims); }


			APAPI bool StartsWith(const String& str, const String& v, bool caseInsensitive = false);
			APAPI bool EndsWith(const String& str, const String& v, bool caseInsensitive = false);
			APAPI bool StartsWith(const std::string& str, const std::string& v, bool caseInsensitive = false);
			APAPI bool EndsWith(const std::string& str, const std::string& v, bool caseInsensitive = false);

			APAPI void ToLowerCase(String& str);
			APAPI void ToUpperCase(String& str);

			

			inline uint GetHashCode(const String& str)
			{
				return MurmurHash(str.c_str(), sizeof(wchar_t)*str.length());
			}
		};
	}
}


#endif