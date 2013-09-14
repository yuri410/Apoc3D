#pragma once

#ifndef APOC3D_COLLECTIONSCOMMON_H
#define APOC3D_COLLECTIONSCOMMON_H

/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

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

#include "apoc3d/Common.h"

using namespace Apoc3D::Core;

extern const int SmallPrimeTable[];

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
			virtual bool Equals(const PtrVoid& x, const PtrVoid& y) const;
			virtual int64 GetHashCode(const PtrVoid& obj) const;
		};

		class APAPI Uint64EqualityComparer : public IEqualityComparer<uint64>
		{
			virtual bool Equals(const uint64& x, const uint64& y) const;
			virtual int64 GetHashCode(const uint64& obj) const;
		};

		class APAPI Uint32EqualityComparer : public IEqualityComparer<uint32>
		{
			virtual bool Equals(const uint32& x, const uint32& y) const;
			virtual int64 GetHashCode(const uint32& obj) const;
		};

		class APAPI Int32EqualityComparer : public IEqualityComparer<int32>
		{
		public:
			virtual bool Equals(const int32& x, const int32& y) const;
			virtual int64 GetHashCode(const int32& obj) const;
		};

		class APAPI WCharEqualityComparer : public IEqualityComparer<wchar_t>
		{
			virtual bool Equals(const wchar_t& x, const wchar_t& y) const;
			virtual int64 GetHashCode(const wchar_t& obj) const;
		};

		typedef Resource* LPResource;
		class APAPI ResourceEqualityComparer : public IEqualityComparer<LPResource>
		{
			virtual bool Equals(const LPResource& x, const LPResource& y) const;
			virtual int64 GetHashCode(const LPResource& obj) const;
		};

		class APAPI stlstringEqualityComparer : public IEqualityComparer<std::string>
		{
			virtual bool Equals(const std::string& x, const std::string& y) const;
			virtual int64 GetHashCode(const std::string& obj) const;
		};

		class APAPI StringEqualityComparer : public IEqualityComparer<String>
		{
			virtual bool Equals(const String& x, const String& y) const;
			virtual int64 GetHashCode(const String& obj) const;
		};

		class APAPI RectangleEqualityComparer : public IEqualityComparer<Apoc3D::Math::Rectangle>
		{
			virtual bool Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Rectangle& obj) const;
		};

		class APAPI SizeEqualityComparer : public IEqualityComparer<Apoc3D::Math::Size>
		{
			virtual bool Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Size& obj) const;
		};
		class APAPI PointEqualityComparer : public IEqualityComparer<Apoc3D::Math::Point>
		{
			virtual bool Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Point& obj) const;
		};

		class APAPI PointFEqualityComparer : public IEqualityComparer<Apoc3D::Math::PointF>
		{
			virtual bool Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::PointF& obj) const;
		};
		class APAPI Vector2EqualityComparer : public IEqualityComparer<Apoc3D::Math::Vector2>
		{
			virtual bool Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Vector2& obj) const;
		};
		class APAPI Vector3EqualityComparer : public IEqualityComparer<Apoc3D::Math::Vector3>
		{
			virtual bool Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Vector3& obj) const;
		};
		class APAPI Vector4EqualityComparer : public IEqualityComparer<Apoc3D::Math::Vector4>
		{
			virtual bool Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y) const;
			virtual int64 GetHashCode(const Apoc3D::Math::Vector4& obj) const;
		};

		template<> const IEqualityComparer<Resource*>*
			IBuiltInEqualityComparer<Resource*>::Default = new ResourceEqualityComparer();

		template<> const IEqualityComparer<PtrVoid>*
			IBuiltInEqualityComparer<PtrVoid>::Default = new PointerEqualityComparer();

		template<> const IEqualityComparer<wchar_t>*
			IBuiltInEqualityComparer<wchar_t>::Default = new WCharEqualityComparer();

		template<> const IEqualityComparer<std::string>* 
			IBuiltInEqualityComparer<std::string>::Default = new stlstringEqualityComparer();

		template<> const IEqualityComparer<uint64>*
			IBuiltInEqualityComparer<uint64>::Default = new Uint64EqualityComparer();

		template<> const IEqualityComparer<uint32>*
			IBuiltInEqualityComparer<uint32>::Default = new Uint32EqualityComparer();

		template<> const IEqualityComparer<int32>* 
			IBuiltInEqualityComparer<int32>::Default = new Int32EqualityComparer();

		template<> const IEqualityComparer<String>*
			IBuiltInEqualityComparer<String>::Default = new StringEqualityComparer();

		template<> const IEqualityComparer<Apoc3D::Math::Point>*
			IBuiltInEqualityComparer<Apoc3D::Math::Point>::Default = new PointEqualityComparer();
		template<> const IEqualityComparer<Apoc3D::Math::Size>*
			IBuiltInEqualityComparer<Apoc3D::Math::Size>::Default = new SizeEqualityComparer();
		template<> const IEqualityComparer<Apoc3D::Math::Rectangle>*
			IBuiltInEqualityComparer<Apoc3D::Math::Rectangle>::Default = new RectangleEqualityComparer();


		template<> const IEqualityComparer<Apoc3D::Math::PointF>*
			IBuiltInEqualityComparer<Apoc3D::Math::PointF>::Default = new PointFEqualityComparer();
		template<> const IEqualityComparer<Apoc3D::Math::Vector2>*
			IBuiltInEqualityComparer<Apoc3D::Math::Vector2>::Default = new Vector2EqualityComparer();
		template<> const IEqualityComparer<Apoc3D::Math::Vector3>*
			IBuiltInEqualityComparer<Apoc3D::Math::Vector3>::Default = new Vector3EqualityComparer();
		template<> const IEqualityComparer<Apoc3D::Math::Vector4>*
			IBuiltInEqualityComparer<Apoc3D::Math::Vector4>::Default = new Vector4EqualityComparer();


		class APAPI HashHelpers
		{
		public:
			//static const int primes[72];

			static int GetPrime(int min)
			{
				for (int i = 0; i < 72; i++)
				{
					int val = SmallPrimeTable[i];
					if (val >= min)
					{
						return val;
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
				int root = (int)sqrtf((float)candidate);
				for (int i = 3; i <= root; i += 2)
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