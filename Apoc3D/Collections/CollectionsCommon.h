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
		struct EqualityComparer
		{
			/** 
			 *  Determines whether the specified objects are equal.
			 */
			static bool Equals(const T& x, const T& y);
			/** 
			 *  Returns a hash code for the specified object.
			 */
			static int64 GetHashCode(const T& obj);
		};

		template <typename T>
		struct EqualityComparer<T*>
		{
			static bool Equals(T* const& x, T* const& y) { return x==y; }
			static int64 GetHashCode(T* const& obj) { return static_cast<int64>(reinterpret_cast<uintptr>(obj)); }
		};

		template <>
		struct EqualityComparer<void*>
		{
			static bool Equals(void* const& x, void* const& y) { return x==y; }
			static int64 GetHashCode(void* const& obj) { return static_cast<int64>(reinterpret_cast<uintptr>(obj)); }
		};


		template <>
		struct EqualityComparer<uint64>
		{
			static bool Equals(const uint64& x, const uint64& y) { return x == y; }
			static int64 GetHashCode(const uint64& obj) { return static_cast<int64>(obj); }
		};

		template <>
		struct EqualityComparer<uint32>
		{
			static bool Equals(const uint32& x, const uint32& y) { return x == y; }
			static int64 GetHashCode(const uint32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<int32>
		{
			static bool Equals(const int32& x, const int32& y) { return x == y; }
			static int64 GetHashCode(const int32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<wchar_t>
		{
			static bool Equals(const wchar_t& x, const wchar_t& y) { return x == y; }
			static int64 GetHashCode(const wchar_t& obj) { return static_cast<int64>(obj); }
		};

		template <>
		struct APAPI EqualityComparer<std::string>
		{
			static bool Equals(const std::string& x, const std::string& y);
			static int64 GetHashCode(const std::string& obj);
		};

		template <>
		struct APAPI EqualityComparer<String>
		{
			static bool Equals(const String& x, const String& y);
			static int64 GetHashCode(const String& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Rectangle>
		{
			static bool Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y);
			static int64 GetHashCode(const Apoc3D::Math::Rectangle& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Size>
		{
			static bool Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y);
			static int64 GetHashCode(const Apoc3D::Math::Size& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Point>
		{
			static bool Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y);
			static int64 GetHashCode(const Apoc3D::Math::Point& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::PointF>
		{
			static bool Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y);
			static int64 GetHashCode(const Apoc3D::Math::PointF& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector2>
		{
			static bool Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y);
			static int64 GetHashCode(const Apoc3D::Math::Vector2& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector3>
		{
			static bool Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y);
			static int64 GetHashCode(const Apoc3D::Math::Vector3& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector4>
		{
			static bool Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y);
			static int64 GetHashCode(const Apoc3D::Math::Vector4& obj);
		};

		template <typename T>
		int32 OrderComparer(const T& a, const T& b)
		{
			if (a < b) return -1;
			return (a > b) ? 1 : 0;
		}

		template <typename T, typename S>
		struct KeyValuePair
		{
			T Key;
			S Value;

			KeyValuePair() { }
			KeyValuePair(T key, S val) 
				: Key(key), Value(val) { }
		};

		namespace HashHelpers
		{
			int APAPI GetPrime(int min);
			bool APAPI IsPrime(int candidate);

			template <typename T>
			struct _ToString
			{
				static String Invoke(const T& item) { return L""; }
			};

			template <typename T>
			String ToString(const T& item) { return _ToString<T>::Invoke(item); }

			/*template<> String ToString<int16>(const int16& v);
			template<> String ToString<int32>(const int32& v);
			template<> String ToString<int64>(const int64& v);

			template<> String ToString<uint16>(const uint16& v);
			template<> String ToString<uint32>(const uint32& v);
			template<> String ToString<uint64>(const uint64& v);
			*/
		};

	}
}

#endif