#pragma once
#ifndef APOC3D_FRUSTUM_H
#define APOC3D_FRUSTUM_H

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

#include "BoundingSphere.h"
#include "Plane.h"

namespace Apoc3D
{
	namespace Math
	{
		/**
		 * Defines the plane indexes of a frustum
		 */
		enum FrustumPlane
		{
			FPL_Right = 0,
			FPL_Left = 1,
			FPL_Bottom = 2,
			FPL_Top = 3,
			FPL_Far = 4,
			FPL_Near = 5
		};

		/**
		 *  Defines a frustum
		 */
		class APAPI Frustum
		{
		public:
			Frustum();
			~Frustum();

			/*
			 *  Check if a bounding sphere is intersecting the frustum
			 */
			bool Intersects(const BoundingSphere& sp) const;

			/**
			 *  Update the frustum with new view and projection matrix.
			 */
			void Update(const Matrix& view, const Matrix& proj);

			/**
			 *  Update the frustum with new view-projection matrix.
			 */
			void Update(const Matrix& viewProj);

			const Plane& getPlane(FrustumPlane p) const { return m_planes[static_cast<int32>(p)]; }

		private:
			static const int ClipPlaneCount = 6;

			Plane m_planes[ClipPlaneCount];

		};
	};
};
#endif