#pragma once
#ifndef APOC3D_RAY_H
#define APOC3D_RAY_H

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

namespace Apoc3D
{
	namespace Math
	{
		class APAPI Ray
		{
		public:
			/** Specifies the location of the ray's origin. */
			Vector3 Position;

			/** A unit vector specifying the direction in which the ray is pointing. */
			Vector3 Direction;

			Ray() { }
			Ray(const Vector3& position, const Vector3& direction)
				: Position(position), Direction(direction)
			{ }

			bool operator==(const Ray &other) const
			{
				return other.Position == Position && other.Direction == Direction; 
			}
			bool operator!=(const Ray &other) const { return !(*this == other); }

			bool IntersectsTriangle(const Vector3& a, const Vector3& b, const Vector3& c, Vector3& result);

			/** Determines whether a ray intersects the specified object. */
			static bool Intersects(const Ray& ray, const BoundingBox& box, float& distance);
		};

		class RaySegment
		{
		public:
			Vector3 Start;
			Vector3 End;

			RaySegment() { }
			RaySegment(const Vector3& start, const Vector3& end)
				: Start(start), End(end) { }

		};
	}
}
#endif