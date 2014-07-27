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

#include "Ray.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

namespace Apoc3D
{
	namespace Math
	{
		bool Ray::IntersectsTriangle(const Vector3& a, const Vector3& b, const Vector3& c, Vector3& result)
		{
			Plane p(a,b,c);
			if (p.IntersectsRay(Position, Direction, result))
			{
				Vector3 planeN(p.X, p.Y, p.Z);

				bool res1 = Vector3::Dot(result - a, Vector3::Cross((b - a), planeN)) >= 0;//ab
				bool res2 = Vector3::Dot(result - b, Vector3::Cross((c - b), planeN)) >= 0;//bc
				bool res3 = Vector3::Dot(result - c, Vector3::Cross((a - c), planeN)) >= 0;//ca

				return ((!res1 & !res2 & !res3) || (res1 & res2 & res3));
			}
			return false;
		}

		bool Ray::Intersects(const Ray& ray, const BoundingBox& box, float& distance)
		{
			float d = 0.0f;
			float maxValue = FLT_MAX;

			if (fabs(ray.Direction.X) < EPSILON)
			{
				if (ray.Position.X < box.Minimum.X || ray.Position.X > box.Maximum.X)
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / ray.Direction.X;
				float minv = (box.Minimum.X - ray.Position.X) * inv;
				float maxv = (box.Maximum.X - ray.Position.X) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = Math::Max(minv, d);
				maxValue = Math::Min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			if (fabs(ray.Direction.Y) < EPSILON)
			{
				if (ray.Position.Y < box.Minimum.Y || ray.Position.Y > box.Maximum.Y)
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / ray.Direction.Y;
				float minv = (box.Minimum.Y - ray.Position.Y) * inv;
				float maxv = (box.Maximum.Y - ray.Position.Y) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = Math::Max(minv, d);
				maxValue = Math::Min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			if (fabs(ray.Direction.Z) < EPSILON)
			{
				if (ray.Position.Z < box.Minimum.Z || ray.Position.Z > box.Maximum.Z)
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / ray.Direction.Z;
				float minv = (box.Minimum.Z - ray.Position.Z) * inv;
				float maxv = (box.Maximum.Z - ray.Position.Z) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = Math::Max(minv, d);
				maxValue = Math::Min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			distance = d;
			return true;
		}
		
	}
}
