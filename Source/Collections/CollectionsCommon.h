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
#ifndef COLLECTIONSCOMMON_H
#define COLLECTIONSCOMMON_H

#include "Common.h"

using namespace Apoc3D::Core;

using namespace std;

namespace Apoc3D
{
	namespace Collections
	{
		/** Defines methods to support the comparison of objects for equality.
		*/
		template<typename T>
		class APAPI IEqualityComparer
		{
		public:
			static const IEqualityComparer* Default;
			/** Determines whether the specified objects are equal.
			*/
			virtual bool Equals(const T& x, const T& y) const = 0;
			/** Returns a hash code for the specified object.
			*/
			virtual int64 GetHashCode(const T& obj) const = 0;
		};

		class Uint32EqualityComparer : public IEqualityComparer<uint32>
		{
		public:
			virtual bool Equals(const uint32& x, const uint32& y) const;

			virtual int64 GetHashCode(const uint32& obj) const;
		};

		class WCharEqualityComparer : public IEqualityComparer<wchar_t>
		{
		public:
			virtual bool Equals(const wchar_t& x, const wchar_t& y) const;

			virtual int64 GetHashCode(const wchar_t& obj) const;
		};

		typedef Resource* LPResource;
		class ResourceEqualityComparer : public IEqualityComparer<LPResource>
		{
		public:
			virtual bool Equals(const LPResource& x, const LPResource& y) const;
			
			virtual int64 GetHashCode(const LPResource& obj) const;
		};

		class stlstringEqualityComparer : public IEqualityComparer<string>
		{
		private:
			unordered_map<string, string> m_hasher;
		public:
			virtual bool Equals(const string& x, const string& y) const;

			virtual int64 GetHashCode(const string& obj) const;
		};


		class HashHelpers
		{
		public:
			static const int primes[72];

			static int GetPrime(int min)
			{
				for (int i = 0; i < 72; i++)
				{
					int num2 = primes[i];
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