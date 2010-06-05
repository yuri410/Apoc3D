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
#ifndef FRUSTUM_H
#define FRUSTUM_H
#pragma once

#include "..\Common.h"
#include "BoundingSphere.h"

namespace Apoc3D
{
	namespace Core
	{
		/* Defines the plane indexes of a frustum
		*/
		enum _Export FrustumPlane
		{
			FPL_Right = 0,
			FPL_Left = 1,
			FPL_Bottom = 2,
			FPL_Top = 3,
			FPL_Far = 4,
			FPL_Near = 5
		};

		/* Defines a frustum
		*/
		class _Export Frustum
		{
		private:
			const static int ClipPlaneCount = 6;
			Plane m_planes[ClipPlaneCount];

		public:
			Frustum(void);
			~Frustum(void);

			bool Intersects(const BoundingSphere& sp) const
			{
				for (int i=0;i<ClipPlaneCount;i++)
				{
					float d = D3DXPlaneDotCoord(&m_planes[i], &sp.Center);
					if (d<=-sp.Radius)
					{
						return false;
					}
				}
				return true;
			}

			/* Update the frustum with new view and projection matrix.
			*/
			void Update(const Matrix& view, const Matrix& proj);

			/* Update the frustum with new view-pojection matrix.
			*/
			void Update(const Matrix& viewProj);
		};
	};
};
#endif