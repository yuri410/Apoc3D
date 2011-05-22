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

#include "Ray.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

namespace Apoc3D
{
	namespace Math
	{
		bool Ray::Intersects(const Ray& ray, const BoundingBox& box, float& distance)
		{
			float d = 0.0f;
			float maxValue = numeric_limits<float>::max();

			if (fabs(_V3X(ray.Direction)) < EPSILON)
			{
				if (_V3X(ray.Position) < _V3X(box.Minimum) || _V3X(ray.Position) > _V3X(box.Maximum))
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / _V3X(ray.Direction);
				float minv = (_V3X(box.Minimum) - _V3X(ray.Position)) * inv;
				float maxv = (_V3X(box.Maximum) - _V3X(ray.Position)) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = max(minv, d);
				maxValue = min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			if (fabs(_V3Y(ray.Direction)) < EPSILON)
			{
				if (_V3Y(ray.Position) < _V3Y(box.Minimum) || _V3Y(ray.Position) > _V3Y(box.Maximum))
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / _V3Y(ray.Direction);
				float minv = (_V3Y(box.Minimum) - _V3Y(ray.Position)) * inv;
				float maxv = (_V3Y(box.Maximum) - _V3Y(ray.Position)) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = max(minv, d);
				maxValue = min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			if (fabs(_V3Z(Direction)) < EPSILON)
			{
				if (_V3Z(ray.Position) < _V3Z(box.Minimum) || _V3Z(ray.Position) > _V3Z(box.Maximum))
				{
					distance = 0.0f;
					return false;
				}
			}
			else
			{
				float inv = 1.0f / _V3Z(ray.Direction);
				float minv = (_V3Z(box.Minimum) - _V3Z(ray.Position)) * inv;
				float maxv = (_V3Z(box.Maximum) - _V3Z(ray.Position)) * inv;

				if (minv > maxv)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;
				}

				d = max(minv, d);
				maxValue = min(maxv, maxValue);

				if (d > maxValue)
				{
					distance = 0.0f;
					return false;
				}
			}

			distance = d;
			return true;
		}
		
		bool Ray::Intersects(const Ray& ray, const BoundingSphere& sphere, float& distance)
        {
			Vector3 diff = Vector3Utils::Subtract(sphere.Center, ray.Position);
            float pyth = Vector3Utils::LengthSquared(diff);
            float rr = sphere.Radius * sphere.Radius;

            if (pyth <= rr)
            {
                distance = 0.0f;
                return true;
            }

            float dot = Vector3Utils::Dot(diff, ray.Direction);
            if (dot < 0.0f)
            {
                distance = 0.0f;
                return false;
            }

            float temp = pyth - (dot * dot);
            if (temp > rr)
            {
                distance = 0.0f;
                return false;
            }

            distance = dot - (float)sqrtf(rr - temp);
            return true;
        }
	}
}
