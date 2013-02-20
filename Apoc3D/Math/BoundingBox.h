#pragma once
#ifndef APOC3D_BOUNDINGBOX_H
#define APOC3D_BOUNDINGBOX_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Games
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "apoc3d/Common.h"

#include "Vector.h"
#include "Plane.h"
#include "MathCommon.h"

using namespace std;

namespace Apoc3D
{
	namespace Math
	{
		class APAPI BoundingBox
		{
		public:
			/**
			 *  The highest corner of the box.
			 */
			Vector3 Maximum;

			/**
			 *  The lowest corner of the box.
			 */
			Vector3 Minimum;

			BoundingBox() { }
			BoundingBox(Vector3 minimum, Vector3 maximum)
			{
				Minimum = minimum;
				Maximum = maximum;
			}

			Vector3 GetCorner(int index) const
			{
				switch (index)
				{
				case 0:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Maximum));
				case 1:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Maximum));
				case 2:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Maximum));
				case 3:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Maximum));
				case 4:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Minimum));
				case 5:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Minimum));
				case 6:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Minimum));
				case 7:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Minimum));
				}
				return Vector3Utils::Zero;
			}

			PlaneIntersectionType Intersects(const Plane& plane) const
			{
				return Intersects(*this, plane);
			}
			ContainmentType Contains(Vector3 vector) const
			{
				if (Vector3Utils::IsGreaterEqual(vector, Minimum) && 
					Vector3Utils::IsLessEqual(vector, Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Disjoint;
			}
			ContainmentType Contains(const BoundingBox& box) const
			{
				if (_V3X(Maximum) < _V3X(box.Minimum) || _V3X(Minimum) > _V3X(box.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (_V3Y(Maximum) < _V3Y(box.Minimum) || _V3Y(Minimum) > _V3Y(box.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (_V3Z(Maximum) < _V3Z(box.Minimum) || _V3Z(Minimum) > _V3Z(box.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (Vector3Utils::IsLessEqual(Minimum, box.Minimum) && 
					Vector3Utils::IsLessEqual(box.Maximum, Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Intersects;
			}

			/**
			 *  Determines whether the box contains the specified point.
			 */
			static ContainmentType Contains(const BoundingBox& box, Vector3 vector)
			{
				if (Vector3Utils::IsGreaterEqual(vector, box.Minimum) && 
					Vector3Utils::IsLessEqual(vector, box.Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Disjoint;
			}

			/** 
			 *  Determines whether the box contains the specified sphere.
			 */
			static ContainmentType Contains(const BoundingBox& box, const BoundingSphere& sphere);
			
			/**
			 *  Determines whether the box contains the specified box.
			 */
			static ContainmentType Contains(const BoundingBox& box1, const BoundingBox& box2)
			{
				if (_V3X(box1.Maximum) < _V3X(box2.Minimum) || _V3X(box1.Minimum) > _V3X(box2.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (_V3Y(box1.Maximum) < _V3Y(box2.Minimum) || _V3Y(box1.Minimum) > _V3Y(box2.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (_V3Z(box1.Maximum) < _V3Z(box2.Minimum) || _V3Z(box1.Minimum) > _V3Z(box2.Maximum))
				{
					return CONTAIN_Disjoint;
				}
				if (Vector3Utils::IsLessEqual(box1.Minimum, box2.Minimum) && 
					Vector3Utils::IsLessEqual(box2.Maximum, box1.Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Intersects;
			}
			/**
			 *  Constructs a BoundingBox that fully contains the given points.
			 */
			static void CreateFromPoints(BoundingBox& res, const Vector3* points, int count)
			{
				Vector3 min = Vector3Utils::LDVector(FLT_MAX);
				Vector3 max = Vector3Utils::LDVector(FLT_MIN);

				for (int i = 0; i < count; i++)
				{
					min = Vector3Utils::Minimize(min, points[i]);
					max = Vector3Utils::Maximize(max, points[i]);
				}

				res = BoundingBox(min, max);
			}
			/**
			 *  Constructs a BoundingBox from a given sphere.
			 */
			static void CreateFromSphere(BoundingBox& res, const BoundingSphere& sphere);
			/**
			 *  Constructs a BoundingBox that is the as large as the total combined area of the two specified boxes.
			 */
			static void Merge(BoundingBox& res, const BoundingBox& box1, const BoundingBox& box2)
			{				
				res.Minimum = Vector3Utils::Minimize(box1.Minimum, box2.Minimum);
				res.Maximum = Vector3Utils::Maximize(box1.Maximum, box2.Maximum);				
			}
			/**
			 *  Finds the intersection between a plane and a box.
			 */
			static PlaneIntersectionType Intersects(const BoundingBox& box, const Plane& plane)
			{
				return Plane::Intersects(plane, box);
			}
			static bool Intersects(const BoundingBox& box, const Ray& ray, float& distance);

			/**
			 *  Determines whether a box intersects the specified object.
			 */
			static bool Intersects(const BoundingBox& box, const BoundingSphere& sphere);

			/**
			 *  Determines whether a box intersects the specified object.
			 */
			static bool Intersects(const BoundingBox& box1, const BoundingBox& box2)
			{
				if (_V3X(box1.Maximum) < _V3X(box2.Minimum) || _V3X(box1.Minimum) > _V3X(box2.Maximum))
					return false;

				if (_V3Y(box1.Maximum) < _V3Y(box2.Minimum) || _V3Y(box1.Minimum) > _V3Y(box2.Maximum))
					return false;

				return (_V3Z(box1.Maximum) >= _V3Z(box2.Minimum) && _V3Z(box1.Minimum) <= _V3Z(box2.Maximum));
			}
		};
	}
}

#endif