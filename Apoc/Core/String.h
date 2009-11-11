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
			String(const char* s)
			{
				
			}

			String(const wchar_t * s)
			{
				str = std::wstring(s);
			}

			String(void)
			{

			}
			~String(void){}

			void Trim()
			{

			}

			void TrimLeft()
			{

			}
			void TrimRight()
			{

			}
			
		};
	}
}
#endif