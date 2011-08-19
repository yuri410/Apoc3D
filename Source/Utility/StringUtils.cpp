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
#include "StringUtils.h"
#include <strstream>

using namespace std;

namespace Apoc3D
{
	namespace Utility
	{
		const String StringUtils::Empty;

		string StringUtils::toString(const String& str)
		{
			char* buffer = new char[str.length()+1];
			buffer[str.length()] = 0;
			wcstombs(buffer, str.c_str(), str.length());
			string result = buffer;
			delete[] buffer;
			return result;
		}
		String StringUtils::toWString(const string& str)
		{
			wchar_t* buffer = new wchar_t[str.length()+1];
			buffer[str.length()] = 0;
			mbstowcs(buffer, str.c_str(), str.length());
			String result = buffer;
			delete[] buffer;
			return result;
		}

		bool StringUtils::ParseBool(const String& val)
		{
			wistringstream str(val);
			str.setf(ios::boolalpha);
			str.imbue(locale::classic());

			bool ret;
			str >> ret;
			return ret;
		}
		uint16 StringUtils::ParseUInt16(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint16 ret = 0;
			str >> ret;

			return ret;
		}
		uint32 StringUtils::ParseUInt32(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint32 ret = 0;
			str >> ret;

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

			return ret;
		}
		uint64 StringUtils::ParseUInt64(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			uint64 ret = 0;
			str >> ret;

			return ret;
		}
		int16 StringUtils::ParseInt16(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int16 ret = 0;
			str >> ret;
			
			return ret;
		}
		int32 StringUtils::ParseInt32(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int32 ret = 0;
			str >> ret;

			return ret;
		}
		int64 StringUtils::ParseInt64(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			int64 ret = 0;
			str >> ret;

			return ret;
		}
		float StringUtils::ParseSingle(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			
			float ret = 0;
			str >> ret;

			return ret;
		}
		double StringUtils::ParseDouble(const String& val)
		{
			wistringstream str(val);
			str.imbue(locale::classic());
			double ret = 0;
			str >> ret;

			return ret;
		}

		String StringUtils::ToString(bool val)
		{
			wostringstream stream;
			stream.setf(ios::boolalpha);
			stream << val;
			return stream.str();
		}
		String StringUtils::ToString(const wchar_t* val, 
			unsigned short width, wchar_t fill, std::ios::fmtflags flags)
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

		String StringUtils::ToString(int64 val, 
			unsigned short width, wchar_t fill, std::ios::fmtflags flags)
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
		String StringUtils::ToString(uint64 val, 
			unsigned short width, wchar_t fill, std::ios::fmtflags flags)
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
		String StringUtils::ToString(float val, 
			unsigned short precision, unsigned short width, char fill, std::ios::fmtflags flags)
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
		vector<String> StringUtils::Split(const String& str, const String& delims, int32 preserve)
		{
			std::vector<String> ret;
		
			ret.reserve(preserve ? preserve : 4);

			unsigned int numSplits = 0;

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
					ret.push_back( str.substr(start) );
					break;
				}
				else
				{
					// Copy up to delimiter
					ret.push_back( str.substr(start, pos - start) );
					start = pos + 1;
				}
				// parse up to next real data
				start = str.find_first_not_of(delims, start);
				++numSplits;

			} while (pos != String::npos);

			return ret;
		}

		bool StringUtils::StartsWidth(const String& str, const String& v, bool caseInsensitive)
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

		bool StringUtils::EndsWidth(const String& str, const String& v, bool caseInsensitive)
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
			std::transform(
				str.begin(),
				str.end(),
				str.begin(),
				tolower);
		}
		void StringUtils::ToUpperCase(String& str)
		{
			std::transform(
				str.begin(),
				str.end(),
				str.begin(),
				toupper);
		}
	}
}