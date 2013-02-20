#pragma once

#ifndef APOC3D_COLLECTIONSCOMMON_H
#define APOC3D_COLLECTIONSCOMMON_H

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

#include "apoc3d/Common.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Core;

using namespace std;

static const int SmallPrimeTable[72] = { 
	3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 
	293, 353, 431, 521, 631, 761, 919, 1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 
	5839, 7013, 8419, 10103, 12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 
	108631, 130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263, 1674319, 
	2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
};

namespace Apoc3D
{
	namespace Collections
	{
		/** 
		 *  Defines methods to support the comparison of objects for equality.
		 */
		template<typename T>
		class IEqualityComparer
		{
		public:
			IEqualityComparer() { }
			virtual ~IEqualityComparer() { }

			//static const IEqualityComparer* Default;
			/** 
			 *  Determines whether the specified objects are equal.
			 */
			virtual bool Equals(const T& x, const T& y) const = 0;
			/** 
			 *  Returns a hash code for the specified object.
			 */
			virtual int64 GetHashCode(const T& obj) const = 0;
		};

		template<typename T>
		class IBuiltInEqualityComparer
		{
		public:
			static const IEqualityComparer<T>* Default;
		};

		typedef void* PtrVoid;
		class APAPI PointerEqualityComparer : public IEqualityComparer<PtrVoid>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<PtrVoid> { };

			virtual bool Equals(const PtrVoid& x, const PtrVoid& y) const;
			virtual int64 GetHashCode(const PtrVoid& obj) const;
		};

		class APAPI Uint64EqualityComparer : public IEqualityComparer<uint64>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<uint64> { };

			virtual bool Equals(const uint64& x, const uint64& y) const;
			virtual int64 GetHashCode(const uint64& obj) const;
		};

		class APAPI Uint32EqualityComparer : public IEqualityComparer<uint32>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<uint32> { };

			virtual bool Equals(const uint32& x, const uint32& y) const;
			virtual int64 GetHashCode(const uint32& obj) const;
		};

		class APAPI Int32EqualityComparer : public IEqualityComparer<int32>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<int32> { };

			virtual bool Equals(const int32& x, const int32& y) const;
			virtual int64 GetHashCode(const int32& obj) const;
		};

		class APAPI WCharEqualityComparer : public IEqualityComparer<wchar_t>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<wchar_t> { };

			virtual bool Equals(const wchar_t& x, const wchar_t& y) const;
			virtual int64 GetHashCode(const wchar_t& obj) const;
		};

		typedef Resource* LPResource;
		class APAPI ResourceEqualityComparer : public IEqualityComparer<LPResource>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<LPResource> { };

			virtual bool Equals(const LPResource& x, const LPResource& y) const;
			virtual int64 GetHashCode(const LPResource& obj) const;
		};

		class APAPI stlstringEqualityComparer : public IEqualityComparer<string>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<string> { };

			virtual bool Equals(const string& x, const string& y) const;
			virtual int64 GetHashCode(const string& obj) const;

		private:
			unordered_map<string, string> m_hasher;
		};

		class APAPI StringEuqlityComparer : public IEqualityComparer<String>
		{
		public:
			class BuiltIn : public IBuiltInEqualityComparer<String> { };

			virtual bool Equals(const String& x, const String& y) const;
			virtual int64 GetHashCode(const String& obj) const;
		};


		template<> const IEqualityComparer<Resource*>*
			IBuiltInEqualityComparer<Resource*>::Default = new ResourceEqualityComparer();

		template<> const IEqualityComparer<PtrVoid>*
			IBuiltInEqualityComparer<PtrVoid>::Default = new PointerEqualityComparer();

		template<> const IEqualityComparer<wchar_t>*
			IBuiltInEqualityComparer<wchar_t>::Default = new WCharEqualityComparer();

		template<> const IEqualityComparer<string>* 
			IBuiltInEqualityComparer<string>::Default = new stlstringEqualityComparer();

		template<> const IEqualityComparer<uint64>*
			IBuiltInEqualityComparer<uint64>::Default = new Uint64EqualityComparer();

		template<> const IEqualityComparer<uint32>*
			IBuiltInEqualityComparer<uint32>::Default = new Uint32EqualityComparer();

		template<> const IEqualityComparer<int32>* 
			IBuiltInEqualityComparer<int32>::Default = new Int32EqualityComparer();

		template<> const IEqualityComparer<String>*
			IBuiltInEqualityComparer<String>::Default = new StringEuqlityComparer();


		class APAPI HashHelpers
		{
		public:
			//static const int primes[72];

			static int GetPrime(int min)
			{
				for (int i = 0; i < 72; i++)
				{
					int num2 = SmallPrimeTable[i];
					if (num2 >= min)
					{
						return num2;
					}
				}
				for (int j = min | 1; j < 2147483647; j += 2)
				{
					if (IsPrime(j))
					{
						return j;
					}
				}
				return min;
			}

			static bool IsPrime(int candidate)
			{
				if ((candidate & 1) == 0)
				{
					return (candidate == 2);
				}
				int num = (int)sqrtf((float)candidate);
				for (int i = 3; i <= num; i += 2)
				{
					if ((candidate % i) == 0)
					{
						return false;
					}
				}
				return true;
			}
		};

	}
}

#endif