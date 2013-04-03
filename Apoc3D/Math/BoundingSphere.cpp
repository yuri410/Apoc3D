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
#include "BoundingSphere.h"

#include "BoundingBox.h"
#include "Ray.h"

namespace Apoc3D
{
	namespace Math
	{
		ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere, const BoundingBox& box)
		{
			Vector3 vector;

			if (!BoundingBox::Intersects(box, sphere))
				return CONTAIN_Disjoint;

			float radius = sphere.Radius * sphere.Radius;
			Vector3 d = Vector3Utils::Subtract(sphere.Center, box.GetCorner(0));
			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Minimum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Maximum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Maximum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Maximum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Maximum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Maximum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Maximum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Minimum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Maximum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Minimum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Minimum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Maximum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Minimum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Maximum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Minimum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Maximum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Maximum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Minimum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Maximum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Minimum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Minimum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			_V3X(vector) = _V3X(sphere.Center) - _V3Z(box.Minimum);
			_V3Y(vector) = _V3Y(sphere.Center) - _V3Z(box.Minimum);
			_V3Z(vector) = _V3Z(sphere.Center) - _V3Z(box.Minimum);

			if (Vector3Utils::LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			return CONTAIN_Contains;
		}

		void BoundingSphere::CreateFromBox(BoundingSphere& res, const BoundingBox& box)
		{
			res.Center = Vector3Utils::Lerp(box.Minimum, box.Maximum, 0.5f);

			float distance = Vector3Utils::Distance(box.Minimum, box.Maximum);

			res.Radius = distance * 0.5f;
		}

		bool BoundingSphere::Intersects(const BoundingSphere& sphere, const BoundingBox& box)
		{
			return BoundingBox::Intersects(box, sphere);
		}
		bool BoundingSphere::Intersects(const BoundingSphere& sphere, const Ray& ray, float& distance)
		{
			return Ray::Intersects(ray, sphere, distance);
		}
		bool BoundingSphere::Intersects(const BoundingSphere& sphere, const Ray& ray, Vector3& p1)
		{
			Vector3 sc = Vector3Utils::Subtract(sphere.Center, ray.Position);

			float slen = Vector3Utils::Dot(ray.Direction, sc);

			if (slen > 0)
			{
				float dist = sqrtf(Vector3Utils::LengthSquared(sc) - slen * slen);

				if (dist <= sphere.Radius)
				{
					const float dd = sqrtf(sphere.Radius * sphere.Radius - dist * dist);
					Vector3 t0 = Vector3Utils::Multiply(ray.Direction,
						slen - dd);						
					p1 = Vector3Utils::Add(ray.Position, t0);

					t0 = Vector3Utils::Multiply(ray.Direction,
						slen + sqrtf(sphere.Radius * sphere.Radius - dist * dist));
					Vector3 p2 = Vector3Utils::Add(ray.Position, t0);						

					float d1 = Vector3Utils::DistanceSquared(p1, ray.Position);
					float d2 = Vector3Utils::DistanceSquared(p2, ray.Position);

					if (d2 < d1)
					{
						p1 = p2;
					}
					return true;
				}
			}

			return false;
		}
	}
}
