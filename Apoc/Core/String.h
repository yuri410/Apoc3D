#ifndef STRING_H
#define STRING_H

#pragma once

#include <string>

namespace Apoc
{
	namespace Core 
	{
		class String
		{
		private:
			std::wstring str;

		public:
			static const int NPOS = -1;

			static std::wstring stringToWstring( const std::string & source)
			{
				char * de = setlocale( LC_ALL, "chs" );
				std::size_t size = source.size() +1;
				wchar_t * wptr = new wchar_t[ size ];
				memset( wptr , 0 , size*sizeof(wchar_t) );
				size = mbstowcs( wptr , source.c_str() ,size );
				std::wstring res= wptr;
				delete [] wptr;
				return res;
			}

			static std::string wstringToString( const std::wstring & source )
			{
				char * de = setlocale( LC_ALL, "chs" );
				std::size_t size = source.size() +1;
				char * ptr = new char[ size*2 ];
				memset( ptr , 0 , size*2 );
				size = ::wcstombs( ptr , source.c_str() ,size*2 );
				std::string res = ptr;
				delete [] ptr;

				return res;
			}
			

			String(const std::string &str)
			{
				this->str = stringToWstring(str);
			}
			String(const std::wstring &str)
			{
				this->str = str;
			}


			String(const wchar_t * s)
			{
				str = std::wstring(s);
			}

			String(wchar_t ch, int count)
			{
				str.assign(count, ch);
			}
			String(void)
			{
				str = L"";
			}
			~String(void){}

			int Length() { return str.length(); }

			void Trim()
			{
				static const std::wstring delims = L" \t\r";
				
				str.erase(str.find_last_not_of(delims)+1); // trim right
				str.erase(0, str.find_first_not_of(delims)); // trim left
			}

			void TrimLeft()
			{
				static const std::wstring delims = L" \t\r";
				
				str.erase(0, str.find_first_not_of(delims)); // trim left
			}
			void TrimRight()
			{
				static const std::wstring delims = L" \t\r";
				
				str.erase(str.find_last_not_of(delims)+1); // trim right
			}
			
			String& operator =(const wchar_t* value)
			{
				str.assign(value);
				return *this;
			}
			
			String& operator +=(const wchar_t* value)
			{
				str.append(value);
				return *this;
			}

			wchar_t operator [](int index) { return str[index]; }

			int IndexOf(const wchar_t * value)
			{
				return str.find(value);
			}
			int IndexOf(const wchar_t * value, int startIndex)
			{
				return str.find(value, startIndex);
			}
			int IndexOf(const wchar_t * value, int startIndex, int count)
			{
				return str.find(value, startIndex, count);
			}

			int LastIndexOf(const wchar_t * value)
			{
				return str.rfind(value);
			}
			int LastIndexOf(const wchar_t * value, int startIndex)
			{
				return str.rfind(value, startIndex);
			}
			int LastIndexOf(const wchar_t * value, int startIndex, int count)
			{
				return str.rfind(value, startIndex, count);
			}
			
			void Insert(int startIndex, const wchar_t * value)
			{
				str.insert(startIndex, value);
			}


			String SubString(int startIndex) const
			{
				return String(str.substr(startIndex));
			}
			String SubString(int startIndex, int count) const
			{
				return String(str.substr(startIndex, count));
			}
		};
	}
}
#endif