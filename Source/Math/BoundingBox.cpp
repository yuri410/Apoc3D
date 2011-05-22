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

#include "BoundingBox.h"

#include "BoundingSphere.h"


namespace Apoc3D
{
	namespace Math
	{
		ContainmentType BoundingBox::Contains(const BoundingBox& box, const BoundingSphere& sphere)
		{				
			Vector3 clamped = Vector3Utils::Clamp(sphere.Center, box.Minimum, box.Maximum);

			float dist = Vector3Utils::DistanceSquared(sphere.Center, clamped);
			const float radius = sphere.Radius;

			if (dist > (radius * radius))
				return CONTAIN_Disjoint;

			Vector3 rrr = Vector3Utils::LDVector(radius, radius, radius);
			Vector3 boxmax_near = Vector3Utils::Subtract(box.Maximum, rrr);
			Vector3 boxmin_near = Vector3Utils::Add(box.Minimum, rrr);

			Vector3 ext = Vector3Utils::Subtract(box.Maximum, box.Minimum);

			if (Vector3Utils::IsLessEqual(boxmin_near, sphere.Center) && 
				Vector3Utils::IsLessEqual(sphere.Center, boxmin_near) && 
				Vector3Utils::IsGreater(ext, rrr))
			{
				return CONTAIN_Contains;
			}
				
			return CONTAIN_Intersects;
		}

		void BoundingBox::CreateFromSphere(BoundingBox& res, const BoundingSphere& sphere)
		{
			Vector3 r = Vector3Utils::LDVector(sphere.Radius);
			res.Minimum = Vector3Utils::Subtract(sphere.Center, r);
			res.Maximum = Vector3Utils::Add(sphere.Center, r);
		}

		bool BoundingBox::Intersects(const BoundingBox& box, const BoundingSphere& sphere)
		{
			Vector3 clamped = Vector3Utils::Clamp(sphere.Center, box.Minimum, box.Maximum);
			float dist = Vector3Utils::DistanceSquared(sphere.Center, clamped);

			return (dist <= (sphere.Radius * sphere.Radius));
		}
	}
}