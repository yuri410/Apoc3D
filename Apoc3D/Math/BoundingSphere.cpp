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

#include "BoundingSphere.h"

#include "Plane.h"
#include "BoundingBox.h"
#include "Ray.h"

namespace Apoc3D
{
	namespace Math
	{
		bool BoundingSphere::IntersectsRay(const Ray& ray, Vector3* _p1) const
		{
			Vector3 sc = Center - ray.Position;
			float slen = Vector3::Dot(ray.Direction, sc);

			if (slen > 0)
			{
				float dist = sqrtf(sc.LengthSquared() - slen * slen);

				if (dist <= Radius)
				{
					if (_p1)
					{
						const float dd = sqrtf(Radius * Radius - dist * dist);
						Vector3 t0 = ray.Direction * (slen - dd);						
						Vector3 p1 = ray.Position + t0;

						//t0 = ray.Direction * (slen + dd);
						//Vector3 p2 = ray.Position + t0;

						//float d1 = Vector3::DistanceSquared(p1, ray.Position);
						//float d2 = Vector3::DistanceSquared(p2, ray.Position);

						*_p1 = p1;// d2 < d1 ? p2 : p1;
					}
					return true;
				}
			}

			return false;
		}
		bool BoundingSphere::IntersectsRayDist(const Ray& ray, float* _dist) const
		{
			Vector3 sc = Center - ray.Position;
			float slen = Vector3::Dot(ray.Direction, sc);

			if (slen > 0)
			{
				float dist = sqrtf(sc.LengthSquared() - slen * slen);

				if (dist <= Radius)
				{
					if (_dist)
					{
						*_dist = dist;
					}
					return true;
				}
			}

			return false;
		}
		bool BoundingSphere::IntersectsLineSegmenent(const Vector3& start, const Vector3& end, 
			float* _dist, Vector3* _n, Vector3* _pos) const
		{
			Vector3 v = (start + end) * 0.5f;
			Vector3 v2 = start - end;

			float r = v2.Length() * 0.5f;

			if (Vector3::Distance(v, Center) <= (r + Radius))
			{
				Vector3 v1 = Center - end;

				Vector3 n = Vector3::Cross(Vector3::Cross(v2, v1), v2);
				n.NormalizeInPlace();

				float dist = Vector3::Dot(v1, n);
				Vector3 pos = Center - n * dist;

				if (_dist)
					*_dist = dist;
				if (_n)
					*_n = n;
				if (_pos)
					*_pos = pos;

				return (dist <= Radius) && (Vector3::DistanceSquared(pos, v) <= r * r);
			}
			return false;
		}

		bool BoundingSphere::IntersectsTriangle(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3* _triN,
			Vector3* _pos, Vector3* _n, float* _depth) const
		{
			Vector3 triN;

			if (_triN)
				triN = *_triN;
			else
			{
				triN = Vector3::Cross(b-a, c-a);
				triN.NormalizeInPlace();
			}

			// surface test
			bool res1 = Vector3::Dot(Center - a, Vector3::Cross((b - a), triN)) >= 0;//ab
			bool res2 = Vector3::Dot(Center - b, Vector3::Cross((c - b), triN)) >= 0;//bc
			bool res3 = Vector3::Dot(Center - c, Vector3::Cross((a - c), triN)) >= 0;//ca
			float dist;

			if ((!res1 & !res2 & !res3) | (res1 & res2 & res3))
			{
				dist = Vector3::Dot(triN, Center - a);

				if (dist < 0)
				{
					triN.X = -triN.X; triN.Y = -triN.Y; triN.Z = -triN.Z;
					dist = -dist;
				}

				if (dist <= Radius)
				{
					if (_pos)
						*_pos = Center - triN * dist;

					if (_depth)
						*_depth = dist - Radius;
					return true;
				}
			}

			// edge test

			bool ab = (res1 != res2 && res1 != res3);
			bool bc = (res2 != res1 && res2 != res3);
			bool ca = (res3 != res1 && res3 != res2);

			if (ab && IntersectsLineSegmenent(b, a, &dist, _n, _pos))
			{
				if (_depth)
					*_depth = dist - Radius;
				return true;
			}
			if (bc && IntersectsLineSegmenent(c, b, &dist, _n, _pos))
			{
				if (_depth)
					*_depth = dist - Radius;
				return true;
			}
			if (ca && IntersectsLineSegmenent(a, c, &dist, _n, _pos))
			{
				if (_depth)
					*_depth = dist - Radius;
				return true;
			}

			// point test
			if (bc)
			{
				dist = Vector3::DistanceSquared(a, Center);
				if (dist <= Radius * Radius)
				{
					if (_depth)
					{
						dist = sqrtf(dist);
						*_depth = dist - Radius;
					}
					
					if (_pos)
						*_pos = a;

					if (_n)
					{
						*_n = Center - a;
						_n->NormalizeInPlace();
					}

					return true;
				}
			}
			if (ca)
			{
				dist = Vector3::DistanceSquared(b, Center);
				if (dist <= Radius * Radius)
				{
					if (_depth)
					{
						dist = sqrtf(dist);
						*_depth = dist - Radius;
					}
					if (_pos)
						*_pos = b;

					if (_n)
					{
						*_n = Center - b;
						_n->NormalizeInPlace();
					}
					return true;
				}
			}
			if (ab)
			{
				dist = Vector3::DistanceSquared(c, Center);
				if (dist <= Radius)
				{
					if (_depth)
					{
						dist = sqrtf(dist);
						*_depth = dist - Radius;
					}
					if (_pos)
						*_pos = c;

					if (_n)
					{
						*_n = Center - c;
						_n->NormalizeInPlace();
					}
					return true;
				}
			}
			return false;
		}

		ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere, const BoundingBox& box) 
		{
			Vector3 vector;

			if (!BoundingBox::Intersects(box, sphere))
				return ContainmentType::Disjoint;

			const float radius = sphere.Radius * sphere.Radius;
			Vector3 d = sphere.Center - box.GetCorner(0);
			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (vector.LengthSquared() > radius)
				return ContainmentType::Intersects;

			return ContainmentType::Contains;
		}

		void BoundingSphere::CreateFromBox(BoundingSphere& res, const BoundingBox& box)
		{
			res.Center = (box.Minimum + box.Maximum) * 0.5f;

			float distance = Vector3::Distance(box.Minimum, box.Maximum);

			res.Radius = distance * 0.5f;
		}
		void BoundingSphere::CreateFromPoints(BoundingSphere& res, const Vector3* points, int count)
		{
			Vector3 center = Vector3::Zero;
			for (int i = 0; i < count; i++)
			{
				center += points[i];
			}
			if (count > 0)
				center /= (float)count;

			float radius = 0;
			for (int i = 0; i < count; i++)
			{
				float dist = Vector3::DistanceSquared(center, points[i]);
				if (dist > radius)
					radius = dist;
			}

			res.Center = center;
			res.Radius = sqrtf(radius);
		}

		void BoundingSphere::Merge(BoundingSphere& res, const BoundingSphere& sphere1, const BoundingSphere& sphere2)
		{
			Vector3 difference = sphere2.Center - sphere1.Center;

			float length = difference.Length();
			const float& radius = sphere1.Radius;
			const float& radius2 = sphere2.Radius;

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

			Vector3 vector = difference / length;
			float minv = Min(-radius, length - radius2);
			float maxv = (Max(radius, length + radius2) - minv) * 0.5f;

			res.Center = sphere1.Center + vector * (maxv + minv);
			res.Radius = maxv;
		}

		bool BoundingSphere::Intersects(const BoundingSphere& sphere, const BoundingBox& box)
		{
			return BoundingBox::Intersects(box, sphere);
		}
		
	}
}
