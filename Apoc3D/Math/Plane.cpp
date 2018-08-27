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

#include "Plane.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

#include "Quaternion.h"
#include "Ray.h"

namespace Apoc3D
{
	namespace Math
	{
		void Plane::ProjectLine(const Vector3& start, const Vector3& end, Vector3& ps, Vector3 pe) const
		{
			Plane pl = Plane::Normalize(*this);

			float prj = pl.Dot3(start);
			Vector3 normal(pl.X, pl.Y, pl.Z);

			ps = start - (normal * prj);

			prj = pl.Dot3(end);
			pe = start - (normal * prj);

		}

		Vector3 Plane::ProjectVector(const Vector3& vec) const
		{
			Plane pl = Plane::Normalize(*this);
			Vector3 normal(pl.X, pl.Y, pl.Z);

			Vector3 result = Vector3::Cross(vec, normal);
			result = Vector3::Cross(normal, result);
			return result;
		}

		PlaneIntersectionType Plane::Intersects(const Plane& plane, const BoundingBox& box)
		{
			Vector3 minv;
            Vector3 maxv;
            maxv.X = (plane.X >= 0.0f) ? box.Minimum.X : box.Maximum.X;
            maxv.Y = (plane.Y >= 0.0f) ? box.Minimum.Y : box.Maximum.Y;
            maxv.Z = (plane.Z >= 0.0f) ? box.Minimum.Z : box.Maximum.Z;
            minv.X = (plane.X >= 0.0f) ? box.Maximum.X : box.Minimum.X;
            minv.Y = (plane.Y >= 0.0f) ? box.Maximum.Y : box.Minimum.Y;
            minv.Z = (plane.Z >= 0.0f) ? box.Maximum.Z : box.Minimum.Z;

            float dot = plane.DotNormal(maxv);

            if (dot + plane.D > 0.0f)
				return PlaneIntersectionType::Front;

            dot = plane.DotNormal(minv);

            if (dot + plane.D < 0.0f)
				return PlaneIntersectionType::Back;

			return PlaneIntersectionType::Intersecting;
		}


		Plane Plane::Transform(const Plane& plane, const Quaternion& rotation)
		{
			float x2 = rotation.X + rotation.X;
			float y2 = rotation.Y + rotation.Y;
			float z2 = rotation.Z + rotation.Z;
			float wx = rotation.W * x2;
			float wy = rotation.W * y2;
			float wz = rotation.W * z2;
			float xx = rotation.X * x2;
			float xy = rotation.X * y2;
			float xz = rotation.X * z2;
			float yy = rotation.Y * y2;
			float yz = rotation.Y * z2;
			float zz = rotation.Z * z2;

			const float& x = plane.X;
			const float& y = plane.Y;
			const float& z = plane.Z;

			Plane result;
			result.X = ((x * ((1.0f - yy) - zz)) + (y * (xy - wz))) + (z * (xz + wy));
			result.Y = ((x * (xy + wz)) + (y * ((1.0f - xx) - zz))) + (z * (yz - wx));
			result.Z = ((x * (xz - wy)) + (y * (yz + wx))) + (z * ((1.0f - xx) - yy));
			result.D = plane.D;
			return result;
		}


		bool Plane::IntersectsLineSegment(const RaySegment& ray, Vector3& intersectPoint) const
		{
			return IntersectsLineSegment(ray.Start, ray.End, intersectPoint);
		}
		bool Plane::IntersectsRay(const Ray& ray, Vector3& intersectionPoint) const
		{
			return IntersectsRay(ray.Position, ray.Direction, intersectionPoint);
		}

	}
}