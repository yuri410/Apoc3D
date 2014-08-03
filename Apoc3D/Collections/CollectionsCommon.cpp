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
#include "apoc3d/Utility/Hash.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Vector.h"

#include <unordered_map>

using namespace std;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Collections
	{
		//////////////////////////////////////////////////////////////////////////
		
		bool EqualityComparer<std::string>::Equals(const string& x, const string& y) { return x==y; }
		int64 EqualityComparer<std::string>::GetHashCode(const string& obj)
		{
			FNVHash32 fnv;
			fnv.Accumulate(obj.c_str(), obj.size());
			return fnv.GetResult();
		}

		bool EqualityComparer<String>::Equals(const String& x, const String& y) { return x==y; }
		int64 EqualityComparer<String>::GetHashCode(const String& obj) { return StringUtils::GetHashCode(obj); }
		
		//////////////////////////////////////////////////////////////////////////

		bool EqualityComparer<Apoc3D::Math::Rectangle>::Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Rectangle>::GetHashCode(const Apoc3D::Math::Rectangle& obj) { return obj.X ^ obj.Y ^ obj.Width ^ obj.Height; }

		bool EqualityComparer<Apoc3D::Math::Size>::Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Size>::GetHashCode(const Apoc3D::Math::Size& obj) { return obj.Width ^ obj.Height; }

		bool EqualityComparer<Apoc3D::Math::Point>::Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Point>::GetHashCode(const Apoc3D::Math::Point& obj) { return obj.X ^ obj.Y; }
		
		bool EqualityComparer<Apoc3D::Math::PointF>::Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::PointF>::GetHashCode(const Apoc3D::Math::PointF& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		//////////////////////////////////////////////////////////////////////////
		
		bool EqualityComparer<Apoc3D::Math::Vector2>::Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Vector2>::GetHashCode(const Apoc3D::Math::Vector2& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y; }

		bool EqualityComparer<Apoc3D::Math::Vector3>::Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Vector3>::GetHashCode(const Apoc3D::Math::Vector3& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z; }
		
		bool EqualityComparer<Apoc3D::Math::Vector4>::Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y) { return x==y; }
		int64 EqualityComparer<Apoc3D::Math::Vector4>::GetHashCode(const Apoc3D::Math::Vector4& obj) { return *(const int32*)&obj.X ^ *(const int32*)&obj.Y ^ *(const int32*)&obj.Z ^ *(const int32*)&obj.W; }

		namespace HashHelpers
		{
			const int SmallPrimeTable[72] = { 
				3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 
				293, 353, 431, 521, 631, 761, 919, 1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 
				5839, 7013, 8419, 10103, 12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 
				108631, 130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263, 1674319, 
				2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
			};

			int GetPrime(int min)
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

			bool IsPrime(int candidate)
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
		}
	}
}