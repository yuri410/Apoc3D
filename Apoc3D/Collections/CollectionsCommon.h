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


namespace Apoc3D
{
	namespace Collections
	{

		/** 
		 *  Defines methods to support the comparison of objects for equality.
		 */
		template<typename T>
		struct EqualityComparerBase
		{
			/** 
			 *  Determines whether the specified objects are equal.
			 */
			static bool Equals(const T& x, const T& y) { return x == y; }
		};

		template<typename T>
		struct EqualityComparer : public EqualityComparerBase<T>
		{
		};

		template <typename T>
		struct EqualityComparer<T*> : public EqualityComparerBase<T*>
		{
			/** 
			 *  Returns a hash code for the specified object.
			 */
			static int32 GetHashCode(T* const& obj)
			{
				if (sizeof(T*) == sizeof(int32))
					return static_cast<int32>(reinterpret_cast<uintptr>(obj));
				
				int64 av = static_cast<int64>(reinterpret_cast<uintptr>(obj));
				return (int32)(av ^ (av >> 32));
			}
		};



		template <>
		struct EqualityComparer<uint64> : public EqualityComparerBase<uint64>
		{
			static int32 GetHashCode(const uint64& obj) { return (int32)(obj ^ (obj >> 32)); }
		};

		template <>
		struct EqualityComparer<uint32> : public EqualityComparerBase<uint32>
		{
			static int32 GetHashCode(const uint32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<int32> : public EqualityComparerBase<int32>
		{
			static int32 GetHashCode(const int32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<wchar_t> : public EqualityComparerBase<wchar_t>
		{
			static int32 GetHashCode(const wchar_t& obj) { return obj; }
		};

		template <>
		struct APAPI EqualityComparer<std::string> : public EqualityComparerBase<std::string>
		{
			static int32 GetHashCode(const std::string& obj);
		};

		template <>
		struct APAPI EqualityComparer<String> : public EqualityComparerBase<String>
		{
			static int32 GetHashCode(const String& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Rectangle> : public EqualityComparerBase<Apoc3D::Math::Rectangle>
		{
			static int32 GetHashCode(const Apoc3D::Math::Rectangle& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Size> : public EqualityComparerBase<Apoc3D::Math::Size>
		{
			static int32 GetHashCode(const Apoc3D::Math::Size& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Point> : public EqualityComparerBase<Apoc3D::Math::Point>
		{
			static int32 GetHashCode(const Apoc3D::Math::Point& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::PointF> : public EqualityComparerBase<Apoc3D::Math::PointF>
		{
			static int32 GetHashCode(const Apoc3D::Math::PointF& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector2> : public EqualityComparerBase<Apoc3D::Math::Vector2>
		{
			static int32 GetHashCode(const Apoc3D::Math::Vector2& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector3> : public EqualityComparerBase<Apoc3D::Math::Vector3>
		{
			static int32 GetHashCode(const Apoc3D::Math::Vector3& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector4> : public EqualityComparerBase<Apoc3D::Math::Vector4>
		{
			static int32 GetHashCode(const Apoc3D::Math::Vector4& obj);
		};

		template <typename T>
		int32 OrderComparer(const T& a, const T& b)
		{
			if (a < b) return -1;
			return (a > b) ? 1 : 0;
		}

		template <typename T, typename S>
		struct KeyPairValue
		{
			T Key;
			S Value;

			KeyPairValue() { }
			KeyPairValue(const T& key, const S& value) 
				: Key(key), Value(value) { }

			KeyPairValue(KeyPairValue&& other)
				: Key(std::move(other.Key)), Value(std::move(other.Value)) { }

			KeyPairValue& operator =(KeyPairValue&& other)
			{
				if (this != &other)
				{
					Key = std::move(other.Key);
					Value = std::move(other.Value);
				}
				return *this;
			}
		};

		namespace Utils
		{
			int APAPI GetHashTableSize(int min);
			bool APAPI IsPrime(int candidate);

			template <typename T>
			struct _ToString
			{
				static String Invoke(const T& item) { return L""; }
			};

			template <typename T>
			String ToString(const T& item) { return _ToString<T>::Invoke(item); }

			

			template <typename A>
			struct HashMapEntry
			{
				int32 hashCode;
				int32 next;
				A data;
			};

			template <typename A, typename B>
			struct HashMapEntryPair : public HashMapEntry < A >
			{
				B value;
			};

		};

	}
}

#endif