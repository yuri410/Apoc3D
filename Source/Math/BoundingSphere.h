/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "Common.h"
#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
		enum ContainmentType
		{
			CONTAIN_Disjoint,
			CONTAIN_Contains,
			CONTAIN_Intersects
		};

		class APAPI BoundingSphere
		{
		public:
			/* Specifies the center point of the sphere.
			*/
			Vector3 Center;
			/* The radius of the sphere.
			*/
			float Radius;

			BoundingSphere(Vector3 center, float radius)
			{
				Center = center;
				Radius = radius;
			}

			/* Determines whether the sphere contains the specified box.
			*/
			static ContainmentType Contains(const BoundingSphere& sphere, const BoundingBox& box);
		};
	}
}

#endif