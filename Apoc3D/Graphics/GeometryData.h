#pragma once
#ifndef APOC3D_GEOMETRYDATA_H
#define APOC3D_GEOMETRYDATA_H

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

#include "GraphicsCommon.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Scene;

namespace Apoc3D
{
	namespace Graphics
	{
		/** Define buffers and information used to render a geometry */
		class APAPI GeometryData
		{
		public:
			RenderSystem::VertexBuffer* VertexBuffer	= nullptr;		/** the vertex buffer used to draw the geometry */
			RenderSystem::IndexBuffer* IndexBuffer		= nullptr;		/** the index buffer used to draw the geometry */
			RenderSystem::VertexDeclaration* VertexDecl	= nullptr;		/** the vertex declaration for vertex in the geometry */

			int32 BaseVertex = 0;							/** The starting vertex offset in the vertex buffer */
			int32 StartIndex = 0;

			int32 PrimitiveCount = 0;
			RenderSystem::PrimitiveType PrimitiveType = RenderSystem::PrimitiveType::PointList;

			int32 VertexCount = 0;
			int32 VertexSize = 0;

			int32 UsedVertexRangeStart = 0;
			int32 UsedVertexRangeCount = 0;

			void* UserData = nullptr;

			bool Discard = false;

			bool usesIndex() const { return IndexBuffer != nullptr; }

			GeometryData()
			{ }
			~GeometryData() { }

			void Setup(RenderSystem::VertexBuffer* vb, RenderSystem::IndexBuffer* ib, RenderSystem::VertexDeclaration* decl,
				RenderSystem::PrimitiveType pt);
			
		};
	};
};
#endif