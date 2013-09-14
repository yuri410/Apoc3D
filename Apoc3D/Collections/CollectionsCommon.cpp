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
#include "CollectionsCommon.h"

#include "apoc3d/Core/Resource.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Vector.h"
#include <unordered_map>

using namespace std;
using namespace Apoc3D::Utility;

const int SmallPrimeTable[72] = { 
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
		
		bool ResourceEqualityComparer::Equals(const LPResource& x, const LPResource& y) const
		{
			const void* a = x;
			const void* b = y;
			return a==b;
		}
		int64 ResourceEqualityComparer::GetHashCode(const LPResource& obj) const
		{
			const void* s = obj;
			return static_cast<int64>(reinterpret_cast<uintptr>(s));
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool PointerEqualityComparer::Equals(const PtrVoid& x, const PtrVoid& y) const { return x==y; }
		int64 PointerEqualityComparer::GetHashCode(const PtrVoid& obj) const
		{
			return static_cast<int64>(reinterpret_cast<uintptr>(obj));
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool WCharEqualityComparer::Equals(const wchar_t& x, const wchar_t& y) const { return x==y; }
		int64 WCharEqualityComparer::GetHashCode(const wchar_t& obj) const
		{
			return static_cast<int64>(obj);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		std::unordered_map<string, int>::hasher stlStringHasher;

		bool stlstringEqualityComparer::Equals(const string& x, const string& y) const { return x==y; }
		int64 stlstringEqualityComparer::GetHashCode(const string& obj) const
		{
			return stlStringHasher(obj);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool Uint64EqualityComparer::Equals(const uint64& x, const uint64& y) const { return x==y; }
		int64 Uint64EqualityComparer::GetHashCode(const uint64& obj) const
		{
			return static_cast<int64>(obj);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool Uint32EqualityComparer::Equals(const uint32& x, const uint32& y) const { return x==y; }
		int64 Uint32EqualityComparer::GetHashCode(const uint32& obj) const { return obj; }

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool Int32EqualityComparer::Equals(const int32& x, const int32& y) const { return x==y; }
		int64 Int32EqualityComparer::GetHashCode(const int32& obj) const { return obj; }

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool StringEqualityComparer::Equals(const String& x, const String& y) const { return x==y; }
		int64 StringEqualityComparer::GetHashCode(const String& obj) const
		{
			return StringUtils::GetHashCode(obj);
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool PointEqualityComparer::Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y) const { return x==y; }
		int64 PointEqualityComparer::GetHashCode(const Apoc3D::Math::Point& obj) const { return obj.X ^ obj.Y; }

		bool SizeEqualityComparer::Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y) const { return x==y; }
		int64 SizeEqualityComparer::GetHashCode(const Apoc3D::Math::Size& obj) const { return obj.Width ^ obj.Height; }

		bool RectangleEqualityComparer::Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y) const { return x==y; }
		int64 RectangleEqualityComparer::GetHashCode(const Apoc3D::Math::Rectangle& obj) const { return obj.X ^ obj.Y ^ obj.Width ^ obj.Height; }

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		bool PointFEqualityComparer::Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y) const { return x==y; }
		int64 PointFEqualityComparer::GetHashCode(const Apoc3D::Math::PointF& obj) const { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		bool Vector2EqualityComparer::Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y) const { return x==y; }
		int64 Vector2EqualityComparer::GetHashCode(const Apoc3D::Math::Vector2& obj) const { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		bool Vector3EqualityComparer::Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y) const { return x==y; }
		int64 Vector3EqualityComparer::GetHashCode(const Apoc3D::Math::Vector3& obj) const { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z; }
		
		bool Vector4EqualityComparer::Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y) const { return x==y; }
		int64 Vector4EqualityComparer::GetHashCode(const Apoc3D::Math::Vector4& obj) const { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z ^ *(const int32*)&obj.W; }
	}
}