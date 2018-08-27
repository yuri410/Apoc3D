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