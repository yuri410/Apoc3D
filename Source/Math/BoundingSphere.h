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
#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "Common.h"
#include "Vector.h"
#include "Plane.h"
#include "MathCommon.h"

namespace Apoc3D
{
	namespace Math
	{

		class APAPI BoundingSphere
		{
		public:
			/** Specifies the center point of the sphere.
			*/
			Vector3 Center;
			/** The radius of the sphere.
			*/
			float Radius;

			BoundingSphere() { }
			BoundingSphere(Vector3 center, float radius)
			{
				Center = center;
				Radius = radius;
			}

			/** Determines whether the sphere contains the specified box.
				@param
					sphere The sphere that will be checked for containment.
					box The box that will be checked for containment.
				@return
					A member of the ContainmentType enumeration indicating whether 
					the two objects intersect, are contained, or don't meet at all.
			*/
			static ContainmentType Contains(const BoundingSphere& sphere, const BoundingBox& box);

			/** Determines whether the sphere contains the specified sphere.			
				@param
					sphere The first sphere that will be checked for containment.
					sphere2 The second sphere that will be checked for containment.
				@returns
					A member of the ContainmentType enumeration indicating whether 
					the two objects intersect, are contained, or don't meet at all.
			*/
			static ContainmentType Contains(const BoundingSphere& sphere1, const BoundingSphere& sphere2)
			{
				float distance = Vector3Utils::Distance(sphere1.Center, sphere2.Center);
				
				float radius = sphere1.Radius;
				float radius2 = sphere2.Radius;

				if (radius + radius < distance)
					return CONTAIN_Disjoint;

				if (radius - radius2 < distance)
					return CONTAIN_Intersects;

				return CONTAIN_Contains;
			}

			/** Determines whether the sphere contains the specified point.
			*/
			static ContainmentType Contains(const BoundingSphere& sphere, Vector3 vector)
			{
				float distance = Vector3Utils::Distance(vector, sphere.Center);

				if (distance >= (sphere.Radius * sphere.Radius))
					return CONTAIN_Disjoint;

				return CONTAIN_Contains;
			}

			/** Constructs a BoundingSphere from a given box.
			*/
			static void CreateFromBox(BoundingSphere& res, const BoundingBox& box);

			/** Constructs a BoundingSphere that fully contains the given points.
			*/
			static void FromPoints(BoundingSphere& res, const Vector3* points, int count)
			{
				Vector3 center = Vector3Utils::Zero;
				for (int i = 0; i < count; i++)
				{
					center = Vector3Utils::Add(points[i], center);
				}
				center = Vector3Utils::Divide(center, static_cast<float>(count));

				float radius = 0;
				for (int i = 0; i < count; i++)
				{
					float dist = Vector3Utils::DistanceSquared(center, points[i]);
					if (dist > radius)
						radius = dist;
				}

				res.Center = center;
				res.Radius = sqrtf(radius);

			}

			/** Constructs a BoundingSphere that is the as large as the total combined area of the two specified spheres.
			*/
			static void Merge(BoundingSphere& res, const BoundingSphere& sphere1, const BoundingSphere& sphere2)
			{
				Vector3 difference = Vector3Utils::Subtract(sphere2.Center, sphere1.Center);

				float length = Vector3Utils::Length(difference);
				float radius = sphere1.Radius;
				float radius2 = sphere2.Radius;

				if (radius + radius2 >= length)
				{
					if (radius - radius2 >= length)
					{
						res = sphere1;
						return;
					}

					if (radius2 - radius >= length)
					{
						res = sphere2;
						return;
					}
				}

				Vector3 vector = Vector3Utils::Divide(difference, length);
				float minv = minf(-radius, length - radius2);
				float maxv = (maxf(radius, length + radius2) - minv) * 0.5f;

				vector = Vector3Utils::Multiply(vector, maxv + minv);
				res.Center = Vector3Utils::Add(sphere1.Center, vector);// + vector * (maxv + minv);
				res.Radius = maxv;
			}
			/** Determines whether a sphere intersects the specified object.
			*/
			static bool Intersects(const BoundingSphere& sphere, const BoundingBox& box);
			
			/** Determines whether a sphere intersects the specified object.
			*/
			static bool Intersects(const BoundingSphere& sphere1, const BoundingSphere& sphere2)
			{
				float distance = Vector3Utils::DistanceSquared(sphere1.Center, sphere2.Center);
				float radius = sphere1.Radius;
				float radius2 = sphere2.Radius;

				if ((radius * radius) + (2.0f * radius * radius2) + (radius2 * radius2) <= distance)
					return false;

				return true;
			}
			/** Determines whether a sphere intersects the specified object.
			*/
			static bool Intersects(const BoundingSphere& sphere, const Ray& ray, float& distance);


			static bool Intersects(const BoundingSphere& sphere, const Ray& ray, Vector3& p1);

			/* Finds the intersection between a plane and a sphere.
			*/
			static PlaneIntersectionType Intersects(const BoundingSphere& sphere, const Plane& plane)
			{
				return Plane::Intersects(plane, sphere);
			}
		};
	}
}

#endif