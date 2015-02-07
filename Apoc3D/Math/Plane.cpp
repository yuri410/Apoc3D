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