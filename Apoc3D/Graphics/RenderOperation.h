#pragma once
#ifndef APOC3D_RENDEROPERATION_H
#define APOC3D_RENDEROPERATION_H

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

#include "apoc3d/ApocCommon.h"

#include "apoc3d/Math/Matrix.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		/** The current-frame transformations for all bones/objects in a sequential order.  */
		struct APAPI PartTransforms
		{
			const Matrix* Transfroms = nullptr;
			int32 Count = 0;
			
			bool operator ==(const PartTransforms& another) const;
			bool operator !=(const PartTransforms& other) const;
		};

		/**
		 *  Represents an operation to render a mesh part in the scene.
		 *
		 *  Usually, transformations data, material and GeometryData including buffers and declarations are part of 
		 *  this operation. Each operation is in equivalent to a DrawPrimitive or DrawIndexedPrimitive call in D3D when not
		 *  using instancing.
		 *
		 *  RenderOperation is used by the engine to manage the scene rendering pipeline.
		 */
		class APAPI RenderOperation
		{
		public:
			GeometryData* GeometryData = nullptr;
			Material* Material = nullptr;
			Matrix RootTransform;
			PartTransforms PartTransform;
			void* UserData = nullptr;

			/** Let the renderer use RootTransform as the final transformation, not multiplying obj's transform */
			bool RootTransformIsFinal = false;

			bool operator ==(const RenderOperation& other);
			bool operator !=(const RenderOperation& other);
		};
	};
};
#endif