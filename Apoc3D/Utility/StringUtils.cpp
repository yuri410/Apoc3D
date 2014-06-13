/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

Copyright (c) 2009+ Tao Xin

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
#include "StringUtils.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Library/ConvertUTF.h"
#include <strstream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Utility
	{
		const String StringUtils::Empty;

		string StringUtils::toPlatformNarrowString(const String& str) { return toPlatformNarrowString(str.c_str()); }
		string StringUtils::toPlatformNarrowString(const wchar_t* str)
		{
			size_t bufSize = wcstombs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				char* buffer = new char[bufSize+1];
				buffer[bufSize] = 0;
				wcstombs(buffer, str, bufSize);

				string result = buffer;
				delete[] buffer;
				return result;
			}
			return string();
		}
		String StringUtils::toPlatformWideString(const string& str) { return toPlatformWideString(str.c_str()); }
		String StringUtils::toPlatformWideString(const char* str)
		{
			size_t bufSize = mbstowcs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				wchar_t* buffer = new wchar_t[bufSize+1];
				buffer[bufSize] = 0;
				mbstowcs(buffer, str, bufSize);

				String result = buffer;
				delete[] buffer;
				return result;
			}
			return Empty;
		}

		std::string StringUtils::toASCIINarrowString(const String& str) { return std::string(str.begin(), str.end()); }
		String StringUtils::toASCIIWideString(const std::string& str) { return String(str.begin(), str.end()); }



		String StringUtils::UTF8toUTF16(const std::string& utf8)
		{
			size_t utf16MaxLength = utf8.length()+1;
			UTF16* resultBuffer = new UTF16[utf16MaxLength];
			memset(resultBuffer, 0, sizeof(UTF16) * utf16MaxLength);

			const UTF8* sourceStart = (const UTF8*)utf8.c_str();
			const UTF8* sourceEnd = sourceStart + utf8.length();
			UTF16* targetStart = resultBuffer;
			UTF16* targetEnd = targetStart + utf16MaxLength;

			ConvertUTF8toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String result((const wchar_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		std::string StringUtils::UTF16toUTF8(const String& utf16)
		{
			size_t utf8MaxLength = 4 * utf16.size()+1;
			UTF8* byteBuffer = new UTF8[utf8MaxLength];
			memset(byteBuffer, 0, sizeof(UTF8) * utf8MaxLength);

			const UTF16* sourcestart = reinterpret_cast<const UTF16*>(utf16.c_str());
			const UTF16* sourceend = sourcestart + utf16.size();
			UTF8* targetstart = byteBuffer;
			UTF8* targetend = targetstart + utf8MaxLength;

			ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
			
			std::string result((const char*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}

		String32 StringUtils::UTF8toUTF32(const std::string& utf8)
		{
			size_t utf32MaxLength = utf8.length()+1;
			UTF32* resultBuffer = new UTF32[utf32MaxLength];
			memset(resultBuffer, 0, sizeof(UTF32) * utf32MaxLength);

			const UTF8* sourceStart = (const UTF8*)utf8.c_str();
			const UTF8* sourceEnd = sourceStart + utf8.length();
			UTF32* targetStart = resultBuffer;
			UTF32* targetEnd = targetStart + utf32MaxLength;

			ConvertUTF8toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String32 result((const char32_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		std::string StringUtils::UTF32toUTF8(const String32& utf32)
		{
			size_t utf8MaxLength = 4 * utf32.size()+1;
			UTF8* byteBuffer = new UTF8[utf8MaxLength];
			memset(byteBuffer, 0, sizeof(UTF8) * utf8MaxLength);

			const UTF32* sourcestart = reinterpret_cast<const UTF32*>(utf32.c_str());
			const UTF32* sourceend = sourcestart + utf32.size();
			UTF8* targetstart = byteBuffer;
			UTF8* targetend = targetstart + utf8MaxLength;

			ConvertUTF32toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);

			std::string result((const char*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}

		String32 StringUtils::UTF16toUTF32(const String& utf16)
		{
			size_t utf16MaxLength = utf16.length()+1;
			UTF32* resultBuffer = new UTF32[utf16MaxLength];
			memset(resultBuffer, 0, sizeof(UTF32) * utf16MaxLength);

			const UTF16* sourceStart = (const UTF16*)utf16.c_str();
			const UTF16* sourceEnd = sourceStart + utf16.length();
			UTF32* targetStart = resultBuffer;
			UTF32* targetEnd = targetStart + utf16MaxLength;

			ConvertUTF16toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String32 result((const char32_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		String StringUtils::UTF32toUTF16(const String32& utf32)
		{
			size_t utf16MaxLength = 2 * utf32.size()+1;
			UTF16* byteBuffer = new UTF16[utf16MaxLength];
			memset(byteBuffer, 0, sizeof(UTF16) * utf16MaxLength);

			const UTF32* sourcestart = reinterpret_cast<const UTF32*>(utf32.c_str());
			const UTF32* sourceend = sourcestart + utf32.size();
			UTF16* targetstart = byteBuffer;
			UTF16* targetend = targetstart + utf16MaxLength;

			ConvertUTF32toUTF16(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);

			String result((const wchar_t*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}



		//////////////////////////////////////////////////////////////////////////

		bool StringUtils::ParseBool(const String& val)
		{
			String v = val;
			ToLowerCase(v);
			return (StartsWith(v, L"true") || StartsWith(v, L"yes")
				|| StartsWith(v, L"1"));
		}
		uint16 StringUtils::ParseUInt16(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtils::ParseUInt32(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtils::ParseUInt32Hex(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			str.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			str.setf ( ios::showbase ); 
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64Hex(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			str.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			str.setf ( ios::showbase ); 
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64Bin(const String& val)
		{
			uint64 result = 0;

			for (size_t i=0;i<val.size();i++)
			{
				wchar_t ch = val[i];
				if (ch == ' ')
					continue;

				result <<=1;
				result |= (ch=='1') ? 1 : 0;
			}

			return result;
		}
		int16 StringUtils::ParseInt16(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int32 StringUtils::ParseInt32(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int64 StringUtils::ParseInt64(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		float StringUtils::ParseSingle(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			
			float ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		double StringUtils::ParseDouble(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			double ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}

		//////////////////////////////////////////////////////////////////////////

		bool startsWidth(const std::string& str, const std::string& v)
		{
			size_t len = str.length();
			size_t vlen = v.length();
			if (len<vlen || !vlen)
			{
				return false;
			}

			std::string startOfThis = str.substr(0, vlen);
			return (startOfThis == v);
		}

		bool StringUtils::ParseBool(const std::string& val)
		{
			std::string v = val;
			std::transform(v.begin(), v.end(), v.begin(), tolower);
			
			return (startsWidth(v, "true") || startsWidth(v, "yes")
				|| startsWidth(v, "1"));
		}
		uint16 StringUtils::ParseUInt16(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			uint16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtils::ParseUInt32(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtils::ParseUInt32Hex(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			str.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			str.setf ( ios::showbase ); 
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64Hex(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			str.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			str.setf ( ios::showbase ); 
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtils::ParseUInt64Bin(const std::string& val)
		{
			uint64 result = 0;

			for (size_t i=0;i<val.size();i++)
			{
				char ch = val[i];
				if (ch == ' ')
					continue;

				result <<=1;
				result |= (ch=='1') ? 1 : 0;
			}

			return result;
		}
		int16 StringUtils::ParseInt16(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			int16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int32 StringUtils::ParseInt32(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			int32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int64 StringUtils::ParseInt64(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			int64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		float StringUtils::ParseSingle(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());

			float ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		double StringUtils::ParseDouble(const std::string& val)
		{
			istringstream str(val);
			str.imbue(locale::classic());
			double ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}


		//////////////////////////////////////////////////////////////////////////
		
		String StringUtils::BoolToString(bool val)
		{
			wostringstream stream;
			stream.setf(ios::boolalpha);
			stream << val;
			return stream.str();
		}

		String StringUtils::IntToString(int64 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		String StringUtils::UIntToString(uint64 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		String StringUtils::SingleToString(float val, uint16 precision, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.precision(precision);
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());

			stream.setf(flags, ios::floatfield);
			stream << val;
			return stream.str();
		}

		String StringUtils::IntToString(int16 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			return IntToString((int64)val, width, fill, flags); 
		}
		String StringUtils::IntToString(int32 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			return IntToString((int64)val, width, fill, flags);
		}
		String StringUtils::UIntToString(uint16 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			return UIntToString((uint64)val, width, fill, flags);
		}
		String StringUtils::UIntToString(uint32 val, uint16 width, wchar_t fill, std::ios::fmtflags flags)
		{
			return UIntToString((uint64)val, width, fill, flags);
		}

		String StringUtils::UIntToStringHex(uint64 val, uint16 width/* =0 */)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill('0');
			stream.imbue(locale::classic());
			stream.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			stream.setf ( ios::showbase ); 
			stream << val;
			return stream.str();
		}
		String StringUtils::UIntToStringHex(uint32 val, uint16 width/* =0 */)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill('0');
			stream.imbue(locale::classic());
			stream.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			stream.setf ( ios::showbase ); 
			stream << val;
			return stream.str();
		}

		String StringUtils::UIntToStringBin(uint64 val)
		{
			//uint64 result = 0;
			String result(64, '0');

			for (size_t i=0;i<64;i++)
			{
				//if ((val & (1UL << i)))
				//{
				//	result[64 - i] = '1';
				//}
				val >>= 1;
				if (val & 1)
				{
					result[63-i] = '1';
				}
			}

			return result;
		}

		//////////////////////////////////////////////////////////////////////////


		std::string StringUtils::BoolToNarrowString(bool val)
		{
			ostringstream stream;
			stream.setf(ios::boolalpha);
			stream << val;
			return stream.str();
		}
		std::string StringUtils::IntToNarrowString(int64 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		std::string StringUtils::UIntToNarrowString(uint64 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		std::string StringUtils::SingleToNarrowString(float val, uint16 precision, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.precision(precision);
			stream.width(width);
			stream.fill(fill);
			stream.imbue(locale::classic());

			stream.setf(flags, ios::floatfield);
			stream << val;
			return stream.str();
		}

		std::string StringUtils::IntToNarrowString(int16 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return IntToNarrowString((int64)val, width, fill, flags); 
		}
		std::string StringUtils::IntToNarrowString(int32 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return IntToNarrowString((int64)val, width, fill, flags);
		}
		std::string StringUtils::UIntToNarrowString(uint16 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return UIntToNarrowString((uint64)val, width, fill, flags);
		}
		std::string StringUtils::UIntToNarrowString(uint32 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return UIntToNarrowString((uint64)val, width, fill, flags);
		}

		std::string StringUtils::UIntToNarrowStringHex(uint64 val, uint16 width/* =0 */)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill('0');
			stream.imbue(locale::classic());
			stream.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			stream.setf ( ios::showbase ); 
			stream << val;
			return stream.str();
		}
		std::string StringUtils::UIntToNarrowStringHex(uint32 val, uint16 width/* =0 */)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill('0');
			stream.imbue(locale::classic());
			stream.setf ( ios::hex, ios::basefield );       // set hex as the basefield
			stream.setf ( ios::showbase ); 
			stream << val;
			return stream.str();
		}

		std::string StringUtils::UIntToNarrowStringBin(uint64 val)
		{
			//uint64 result = 0;
			std::string result(64, '0');

			for (size_t i=0;i<64;i++)
			{
				//if ((val & (1UL << i)))
				//{
				//	result[64 - i] = '1';
				//}
				val >>= 1;
				if (val & 1)
				{
					result[63-i] = '1';
				}
			}

			return result;
		}



		//////////////////////////////////////////////////////////////////////////

		void StringUtils::Trim(String& str, const String& delims)
		{
			str.erase(str.find_last_not_of(delims)+1);
			str.erase(0, str.find_first_not_of(delims));
		}
		void StringUtils::TrimLeft(String& str, const String& delims)
		{
			str.erase(0, str.find_first_not_of(delims));
		}
		void StringUtils::TrimRight(String& str, const String& delims)
		{
			str.erase(str.find_last_not_of(delims)+1);
		}
		void StringUtils::Split(const String& str, List<String>& result, const String& delims)
		{
			assert(result.getCount() == 0);

			// Use STL methods 
			size_t start, pos;
			start = 0;
			do 
			{
				pos = str.find_first_of(delims, start);
				if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else if (pos == String::npos)
				{
					// Copy the rest of the string
					result.Add( str.substr(start) );
					break;
				}
				else
				{
					// Copy up to delimiter
					result.Add( str.substr(start, pos - start) );
					start = pos + 1;
				}
				// parse up to next real data
				start = str.find_first_not_of(delims, start);

			} while (pos != String::npos);
		}
		void StringUtils::Split(const std::string& str, List<std::string>& result, const std::string& delims)
		{
			assert(result.getCount() == 0);

			// Use STL methods 
			size_t start, pos;
			start = 0;
			do 
			{
				pos = str.find_first_of(delims, start);
				if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else if (pos == std::string::npos)
				{
					// Copy the rest of the string
					result.Add( str.substr(start) );
					break;
				}
				else
				{
					// Copy up to delimiter
					result.Add( str.substr(start, pos - start) );
					start = pos + 1;
				}
				// parse up to next real data
				start = str.find_first_not_of(delims, start);

			} while (pos != std::string::npos);
		}

		bool StringUtils::StartsWith(const String& str, const String& v, bool caseInsensitive)
		{
			size_t len = str.length();
			size_t vlen = v.length();
			if (len<vlen || !vlen)
			{
				return false;
			}

			String startOfThis = str.substr(0, vlen);
			if (caseInsensitive)
			{
				ToLowerCase(startOfThis);

				String newV = v;
				ToLowerCase(newV);
				return (startOfThis == newV);
			}
			return (startOfThis == v);
		}

		bool StringUtils::EndsWith(const String& str, const String& v, bool caseInsensitive)
		{
			size_t thisLen = str.length();
			size_t patternLen = v.length();
			if (thisLen < patternLen || patternLen == 0)
				return false;

			String endOfThis = str.substr(thisLen - patternLen, patternLen);
			if (caseInsensitive)
			{
				ToLowerCase(endOfThis);

				String newV = v;
				ToLowerCase(newV);

				return (endOfThis == newV);
			}
			return (endOfThis == v);
		}


		void StringUtils::ToLowerCase(String& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), tolower);
		}
		void StringUtils::ToUpperCase(String& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), toupper);
		}
	}
}