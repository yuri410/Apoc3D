/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "BoundingBox.h"

#include "Plane.h"

#include "BoundingSphere.h"
#include "Ray.h"

namespace Apoc3D
{
	namespace Math
	{
		ContainmentType BoundingBox::Contains(const BoundingSphere& sphere)
		{				
			Vector3 clamped = Vector3::Clamp(sphere.Center, Minimum, Maximum);

			float dist = Vector3::DistanceSquared(sphere.Center, clamped);
			const float radius = sphere.Radius;

			if (dist > (radius * radius))
				return ContainmentType::Disjoint;

			Vector3 rrr = Vector3::Set(radius);
			Vector3 boxmax_near = Maximum - rrr;
			Vector3 boxmin_near = Minimum + rrr;

			Vector3 ext = Maximum - Minimum;

			if (Vector3::IsLessEqual(boxmin_near, sphere.Center) && 
				Vector3::IsLessEqual(sphere.Center, boxmin_near) && 
				Vector3::IsGreater(ext, rrr))
			{
				return ContainmentType::Contains;
			}
				
			return ContainmentType::Intersects;
		}

		void BoundingBox::CreateFromSphere(BoundingBox& res, const BoundingSphere& sphere)
		{
			Vector3 r = Vector3::Set(sphere.Radius);
			res.Minimum = sphere.Center - r;
			res.Maximum = sphere.Center + r;
		}
		bool BoundingBox::Intersects(const BoundingBox& box, const Ray& ray, float& distance)
		{
			return Ray::Intersects(ray, box, distance);
		}

		bool BoundingBox::Intersects(const BoundingBox& box, const BoundingSphere& sphere)
		{
			Vector3 clamped = Vector3::Clamp(sphere.Center, box.Minimum, box.Maximum);
			float dist = Vector3::DistanceSquared(sphere.Center, clamped);

			return (dist <= (sphere.Radius * sphere.Radius));
		}

		PlaneIntersectionType BoundingBox::Intersects(const BoundingBox& box, const Plane& plane)
		{
			return Plane::Intersects(plane, box);
		}
	}
}