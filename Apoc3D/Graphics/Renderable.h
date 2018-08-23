#pragma once
#ifndef APOC3D_IRENDERABLE_H
#define APOC3D_IRENDERABLE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2010 Tao Xin
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

#include "apoc3d/ApocCommon.h"

namespace Apoc3D
{
	namespace Graphics
	{
		/** Represents drawable object in the scene. */
		class APAPI Renderable
		{
		public:
			/** Gets the render operation of this renderable object at a ceratin LOD level */
			virtual RenderOperationBuffer* GetRenderOperation(int level) = 0;

			/** Gets the render operation of this renderable object at the default LOD level */
			virtual RenderOperationBuffer* GetRenderOperation() { return GetRenderOperation(0); }

		protected:
			Renderable()
			{
			}

			virtual ~Renderable()
			{
			}
		};
	};
};
#endif