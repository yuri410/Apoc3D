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

#include "Plane.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"


namespace Apoc3D
{
	namespace Math
	{
		PlaneIntersectionType Plane::Intersects(const Plane& plane, const BoundingBox& box)
		{
			Vector3 minv;
            Vector3 maxv;
            _V3X(maxv) = (plane.X >= 0.0f) ? _V3X(box.Minimum) : _V3X(box.Maximum);
            _V3Y(maxv) = (plane.Y >= 0.0f) ? _V3Y(box.Minimum) : _V3Y(box.Maximum);
            _V3Z(maxv) = (plane.Z >= 0.0f) ? _V3Y(box.Minimum) : _V3Y(box.Maximum);
            _V3X(minv) = (plane.X >= 0.0f) ? _V3X(box.Maximum) : _V3X(box.Minimum);
            _V3Y(minv) = (plane.Y >= 0.0f) ? _V3Y(box.Maximum) : _V3Y(box.Minimum);
            _V3Y(minv) = (plane.Z >= 0.0f) ? _V3Y(box.Maximum) : _V3Y(box.Minimum);

            float dot = plane.DotNormal(maxv);

            if (dot + plane.D > 0.0f)
                return PLANEIT_Front;

            dot = plane.DotNormal(minv);

            if (dot + plane.D < 0.0f)
                return PLANEIT_Back;

            return PLANEIT_Intersecting;
		}

		PlaneIntersectionType Plane::Intersects(const Plane& plane, const BoundingSphere& sphere)
        {
            float dot = plane.Dot3(sphere.Center.X);

            if (dot > sphere.Radius)
                return PLANEIT_Front;

            if (dot < -sphere.Radius)
                return PLANEIT_Back;

            return PLANEIT_Intersecting;
        }

	}
}