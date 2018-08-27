#pragma once
#ifndef APOC3D_OCTREEBOX_H
#define APOC3D_OCTREEBOX_H

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

#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI OctreeBox
		{
		public:
			OctreeBox() { }

			explicit OctreeBox(float length)
				: Length(length)
			{ }

			OctreeBox(const BoundingBox& aabb);
			OctreeBox(const BoundingSphere& sph);

			void GetBoundingSphere(BoundingSphere& sp) const;

			Vector3 Center = Vector3::Zero;
			float Length = 0;
		};
	}
}
#endif