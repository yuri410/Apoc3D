#pragma once
#ifndef APOC3D_VIEWPORT_H
#define APOC3D_VIEWPORT_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
		/**
		 *  Defines a rectangular region of a render-target surface onto which 
		 *	a 3D rendering is projected.
		 */
		class APAPI Viewport
		{
		public:
			/**
			 *  the viewport's X position.
			 */
			int X;
			/**
			 *  the viewport's Y position.
			 */
			int Y;
			/**
			 *  the viewport's width.
			 */
			int Width;
			/**
			 *  the viewport's height.
			 */
			int Height;
			/**
			 *  the viewport's minimum Z depth.
			 */
			float MinZ;
			/**
			 *  the viewport's maximum Z depth.
			 */
			float MaxZ;

			Viewport(int x, int y, int width, int height)
				: X(x), Y(y), Width(width), Height(height),
				MinZ(0), MaxZ(1)
			{
			}
			Viewport(int x, int y, int width, int height, float minZ, float maxZ)
				: X(x), Y(y), Width(width), Height(height),
				MinZ(minZ), MaxZ(maxZ)
			{
			}

			/**
			 *  Converts a screen space point into a corresponding point in world space.
			 */
			Vector3 Unproject(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world);
			/**
			 *  Projects a 3D vector from object space into screen space.
			 */
			Vector3 Project(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world);
		};
	}
}


#endif