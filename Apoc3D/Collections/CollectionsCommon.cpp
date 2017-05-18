/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "CollectionsCommon.h"

#include "apoc3d/Core/Resource.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/Hash.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Math/Color.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Collections
	{
		//////////////////////////////////////////////////////////////////////////

		bool EqualityComparer<std::string>::Equals(const std::string& x, const std::string& y) { return x == y; }
		int32 EqualityComparer<std::string>::GetHashCode(const std::string& obj)
		{
			FNVHash32 fnv;
			fnv.Accumulate(obj.c_str(), obj.size());
			return static_cast<int32>(fnv.getResult());
		}

		bool EqualityComparer<String>::Equals(const String& x, const String& y) { return x == y; }
		int32 EqualityComparer<String>::GetHashCode(const String& obj) { return StringUtils::GetHashCode(obj); }
		
		//////////////////////////////////////////////////////////////////////////

		bool EqualityComparer<Apoc3D::Math::Rectangle>::Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Rectangle>::GetHashCode(const Apoc3D::Math::Rectangle& obj) { return obj.X ^ obj.Y ^ obj.Width ^ obj.Height; }

		bool EqualityComparer<Apoc3D::Math::Size>::Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Size>::GetHashCode(const Apoc3D::Math::Size& obj) { return obj.Width ^ obj.Height; }

		bool EqualityComparer<Apoc3D::Math::Point>::Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Point>::GetHashCode(const Apoc3D::Math::Point& obj) { return obj.X ^ obj.Y; }
		
		bool EqualityComparer<Apoc3D::Math::PointF>::Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::PointF>::GetHashCode(const Apoc3D::Math::PointF& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		//////////////////////////////////////////////////////////////////////////
		
		bool EqualityComparer<Apoc3D::Math::Vector2>::Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Vector2>::GetHashCode(const Apoc3D::Math::Vector2& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		bool EqualityComparer<Apoc3D::Math::Vector3>::Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Vector3>::GetHashCode(const Apoc3D::Math::Vector3& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z; }
		
		bool EqualityComparer<Apoc3D::Math::Vector4>::Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Vector4>::GetHashCode(const Apoc3D::Math::Vector4& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z ^ *(const int32*)&obj.W; }

		bool EqualityComparer<Apoc3D::Math::Color4>::Equals(const Apoc3D::Math::Color4& x, const Apoc3D::Math::Color4& y) { return x == y; }
		int32 EqualityComparer<Apoc3D::Math::Color4>::GetHashCode(const Apoc3D::Math::Color4& obj) { return *(const int32*)&obj.Red ^ *(const int32*)&obj.Green ^ *(const int32*)&obj.Blue ^ *(const int32*)&obj.Alpha; }

		//////////////////////////////////////////////////////////////////////////


		bool NStringEqualityComparerNoCase::Equals(const std::string& x, const std::string& y) { return StringUtils::EqualsNoCase(x,y); }
		int32 NStringEqualityComparerNoCase::GetHashCode(const std::string& obj)
		{
			FNVHash32 fnv;
			for (char c : obj)
			{
				c = StringUtils::ToLowerCase(c);
				fnv.Accumulate(&c, sizeof(c));
			}

			return static_cast<int32>(fnv.getResult());
		}
		bool StringEqualityComparerNoCase::Equals(const String& x, const String& y) { return StringUtils::EqualsNoCase(x, y); }
		int32 StringEqualityComparerNoCase::GetHashCode(const String& obj)
		{
			FNVHash32 fnv;
			for (wchar_t c : obj)
			{
				c = StringUtils::ToLowerCase(c);
				fnv.Accumulate(&c, sizeof(c));
			}
			return static_cast<int32>(fnv.getResult());
		}

		namespace Utils
		{
			const int32 Int32PrimeSizes[] =
			{
				7, 11, 17, 23, 29, 37, 53, 71, 97, 127, 167, 223, 293, 383, 499, 653, 853, 1109,
				1447, 1889, 2459, 3203, 4177, 5431, 7069, 9199, 11959, 15551, 20219, 26293, 34183,
				44449, 57787, 75133, 97673, 126989, 165089, 214631, 279023, 362741, 471571,
				613049, 796967, 1036067, 1346899, 1750979, 2276293, 2959183, 3846943, 5001049, 6501367,
				8451791, 10987343, 14283559, 18568637, 24139231, 31381043, 40795357, 53033969,
				68944181, 89627443, 116515687, 151470409, 196911553
			};

			int32 GetHashTableSize(int32 min)
			{
				for (int32 val : Int32PrimeSizes)
				{
					if (val >= min)
					{
						return val;
					}
				}
				return min;
			}


			template<> String _ToString<std::string>::Invoke(const std::string& str) { return StringUtils::toPlatformWideString(str); }
			template<> String _ToString<const char*>::Invoke(const char* const& str) { return StringUtils::toPlatformWideString(str); }
			template<> String _ToString<const wchar_t*>::Invoke(const wchar_t* const& str) { return str; }

			template<> String _ToString<int16>::Invoke(const int16& v) { return StringUtils::IntToString(v); }
			template<> String _ToString<int32>::Invoke(const int32& v) { return StringUtils::IntToString(v); }
			template<> String _ToString<int64>::Invoke(const int64& v) { return StringUtils::IntToString(v); }

			template<> String _ToString<uint16>::Invoke(const uint16& v) { return StringUtils::UIntToString(v); }
			template<> String _ToString<uint32>::Invoke(const uint32& v) { return StringUtils::UIntToString(v); }
			template<> String _ToString<uint64>::Invoke(const uint64& v) { return StringUtils::UIntToString(v); }
			

			//template<> String ToString<std::string>(const std::string& str) { return StringUtils::toPlatformWideString(str); }

			/*template<> String ToString<int16>(const int16& v) { return StringUtils::IntToString(v); }
			template<> String ToString<int32>(const int32& v) { return StringUtils::IntToString(v); }
			template<> String ToString<int64>(const int64& v) { return StringUtils::IntToString(v); }

			template<> String ToString<uint16>(const uint16& v) { return StringUtils::UIntToString(v); }
			template<> String ToString<uint32>(const uint32& v) { return StringUtils::UIntToString(v); }
			template<> String ToString<uint64>(const uint64& v) { return StringUtils::UIntToString(v); }
			*/
		}
	}
}