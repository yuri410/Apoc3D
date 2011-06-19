/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

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
#include "CollectionsCommon.h"

#include "Core/Resource.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Collections
	{
		//const int HashHelpers::primes[72] = { 
		//	3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 
		//	293, 353, 431, 521, 631, 761, 919, 1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 
		//	5839, 7013, 8419, 10103, 12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 
		//	108631, 130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263, 1674319, 
		//	2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
		//};


		//template<typename T>
		//const IEqualityComparer<T> IEqualityComparer<T>::Default = 0;

		const IEqualityComparer<Resource*>* 
			ResourceEqualityComparer::BuiltIn::Default = new ResourceEqualityComparer();

		bool ResourceEqualityComparer::Equals(const LPResource& x, const LPResource& y) const
		{
			const void* a = x;
			const void* b = y;
			return a==b;
		}
		int64 ResourceEqualityComparer::GetHashCode(const LPResource& obj) const
		{
			const void* s = obj;
			return reinterpret_cast<int64>(s);
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		const IEqualityComparer<wchar_t>*
			WCharEqualityComparer::BuiltIn::Default = new WCharEqualityComparer();

		bool WCharEqualityComparer::Equals(const wchar_t& x, const wchar_t& y) const
		{
			return x==y;
		}
		int64 WCharEqualityComparer::GetHashCode(const wchar_t& obj) const
		{
			return static_cast<int64>(obj);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/



		const IEqualityComparer<string>* 
			stlstringEqualityComparer::BuiltIn::Default = new stlstringEqualityComparer();

		bool stlstringEqualityComparer::Equals(const string& x, const string& y) const
		{
			return x==y;
		}
		int64 stlstringEqualityComparer::GetHashCode(const string& obj) const
		{
			return m_hasher.hash_function()(obj);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		const IEqualityComparer<uint32>* 
			Uint32EqualityComparer::BuiltIn::Default = new Uint32EqualityComparer();

		bool Uint32EqualityComparer::Equals(const uint32& x, const uint32& y) const
		{
			return x==y;
		}
		int64 Uint32EqualityComparer::GetHashCode(const uint32& obj) const
		{
			return obj;
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		const IEqualityComparer<String>*
			StringEuqlityComparer::BuiltIn::Default = new StringEuqlityComparer();

		bool StringEuqlityComparer::Equals(const String& x, const String& y) const
		{
			return x==y;
		}
		int64 StringEuqlityComparer::GetHashCode(const String& obj) const
		{
			return StringUtils::GetHashCode(obj);
		}

	}
}