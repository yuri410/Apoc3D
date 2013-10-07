#pragma once
#ifndef APOC3D_BOUNDINGBOX_H
#define APOC3D_BOUNDINGBOX_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
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

#include "Vector.h"

#include "MathCommon.h"

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
					return Vector3(Minimum.X, Maximum.Y, Maximum.Z);
				case 1:
					return Vector3(Maximum.X, Maximum.Y, Maximum.Z);
				case 2:
					return Vector3(Maximum.X, Minimum.Y, Maximum.Z);
				case 3:
					return Vector3(Minimum.X, Minimum.Y, Maximum.Z);
				case 4:
					return Vector3(Minimum.X, Maximum.Y, Minimum.Z);
				case 5:
					return Vector3(Maximum.X, Maximum.Y, Minimum.Z);
				case 6:
					return Vector3(Maximum.X, Minimum.Y, Minimum.Z);
				case 7:
					return Vector3(Minimum.X, Minimum.Y, Minimum.Z);
				}
				return Vector3::Zero;
			}

			PlaneIntersectionType Intersects(const Plane& plane) const
			{
				return Intersects(*this, plane);
			}
			
			/**
			 *  Determines whether the box contains the specified point.
			 */
			bool Contains(Vector3 vector) const
			{
				if (Vector3::IsGreaterEqual(vector, Minimum) && 
					Vector3::IsLessEqual(vector, Maximum))
				{
					return true;
				}
				return false;
			}
			/**
			 *  Determines whether the box contains the specified box.
			 */
			ContainmentType Contains(const BoundingBox& box) const
			{
				if (Maximum.X < box.Minimum.X || Minimum.X > box.Maximum.X)
				{
					return CONTAIN_Disjoint;
				}
				if (Maximum.Y < box.Minimum.Y || Minimum.Y > box.Maximum.Y)
				{
					return CONTAIN_Disjoint;
				}
				if (Maximum.Z < box.Minimum.Z || Minimum.Z > box.Maximum.Z)
				{
					return CONTAIN_Disjoint;
				}
				if (Vector3::IsLessEqual(Minimum, box.Minimum) && 
					Vector3::IsLessEqual(box.Maximum, Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Intersects;
			}
			
			/** 
			 *  Determines whether the box contains the specified sphere.
			 */
			ContainmentType Contains(const BoundingSphere& sphere);
			
			bool operator==(const BoundingBox &other) const
			{
				return other.Minimum == Minimum && other.Maximum == Maximum; 
			}
			bool operator!=(const BoundingBox &other) const { return !(*this == other); }


			/**
			 *  Constructs a BoundingBox that fully contains the given points.
			 */
			static void CreateFromPoints(BoundingBox& res, const Vector3* points, int count)
			{
				Vector3 minv(FLT_MAX);
				Vector3 maxv(-FLT_MAX);

				for (int i = 0; i < count; i++)
				{
					minv = Vector3::Minimize(minv, points[i]);
					maxv = Vector3::Maximize(maxv, points[i]);
				}

				res = BoundingBox(minv, maxv);
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
				res.Minimum = Vector3::Minimize(box1.Minimum, box2.Minimum);
				res.Maximum = Vector3::Maximize(box1.Maximum, box2.Maximum);				
			}
			/**
			 *  Finds the intersection between a plane and a box.
			 */
			static PlaneIntersectionType Intersects(const BoundingBox& box, const Plane& plane);
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
				if (box1.Maximum.X < box2.Minimum.X || box1.Minimum.X > box2.Maximum.X)
					return false;

				if (box1.Maximum.Y < box2.Minimum.Y || box1.Minimum.Y > box2.Maximum.Y)
					return false;

				return (box1.Maximum.Z >= box2.Minimum.Z && box1.Minimum.Z <= box2.Maximum.Z);
			}
		};
	}
}

#endif