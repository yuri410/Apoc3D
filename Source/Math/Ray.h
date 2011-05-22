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

#ifndef RAY_H
#define RAY_H

#include "Common.h"
#include "Vector.h"
#include "Plane.h"

using namespace std;

namespace Apoc3D
{
	namespace Math
	{
		class APAPI Ray
		{
		public:
			/* Specifies the location of the ray's origin.
			*/
			Vector3 Position;

			/* A unit vector specifying the direction in which the ray is pointing.
			*/
			Vector3 Direction;

			Ray() { }
			Ray(Vector3 position, Vector3 direction)
				: Position(position), Direction(direction)
			{ }

			/* Determines whether a ray intersects the specified object.
			*/
			static bool Intersects(const Ray& ray, const Plane& plane, float& distance)
			{
				float dotDirection = plane.DotNormal(ray.Direction);

				if (fabs(dotDirection) < EPSILON)
				{
					distance = 0;
					return false;
				}

				float dotPosition = plane.DotNormal(ray.Position);
				float num = (-plane.D - dotPosition) / dotDirection;

				if (num < 0.0f)
				{
					if (num < EPSILON)
					{
						distance = 0;
						return false;
					}
					num = 0.0f;
				}

				distance = num;
				return true;
			}

			/* Determines whether a ray intersects the specified object.
			*/
			static bool Intersects(const Ray& ray, const BoundingBox& box, float& distance);

			/* Determines whether a ray intersects the specified object.
			*/
			static bool Intersects(const Ray& ray, const BoundingSphere& sphere, float& distance);
		};
	}
}
#endif