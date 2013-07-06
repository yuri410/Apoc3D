/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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

#include "OctreeBox.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

namespace Apoc3D
{
	namespace Math
	{
		OctreeBox::OctreeBox(const BoundingBox& aabb)
		{
			Length = Vector3::Distance(aabb.Minimum, aabb.Maximum) / Math::Root3;
			Center = Vector3::Add(aabb.Minimum, aabb.Maximum);
			Center*= 0.5f;
		}

		OctreeBox::OctreeBox(const BoundingSphere& sph)
		{
			Center = sph.Center;
			Length = sph.Radius * 2;
		}

		void OctreeBox::GetBoundingSphere(BoundingSphere& sp) const
		{
			sp.Center = Center;
			sp.Radius = Length * (Math::Root3 * 0.5f);
		}
	}
}