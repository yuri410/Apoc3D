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
#ifndef APOC3D_GEOMETRYDATA_H
#define APOC3D_GEOMETRYDATA_H

#include "apoc3d/Common.h"

#include "GraphicsCommon.h"

#include "apoc3d/Core/HashHandleObject.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Scene;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		/**
		 *  Define buffers and information used to render a geometry
		 */
		class APAPI GeometryData : public HashHandleObject
		{
		public:
			/**
			 *  the vertex buffer used to draw the geometry
			 */
			VertexBuffer* VertexBuffer;
			/**
			 *  the index buffer used to draw the geometry
			 */
			IndexBuffer* IndexBuffer;
			/**
			 *  the vertex declaration for vertex in the geometry
			 */
			VertexDeclaration* VertexDecl;
			int32 PrimitiveCount;
			/**
			 *  The starting index offset
			 */
			int32 BaseVertex;
			int32 VertexCount;
			int32 VertexSize;

			PrimitiveType PrimitiveType;
			void* UserData;

			bool Discard;

			bool usesIndex() const { return !!IndexBuffer; }

			GeometryData(void)
				: UserData(0), BaseVertex(0), PrimitiveCount(0), 
				VertexDecl(0), IndexBuffer(0), VertexBuffer(0),
				VertexCount(0), VertexSize(0),
				PrimitiveType(PT_PointList), Discard(false)
			{ }
			~GeometryData(void){}
		};
	};
};
#endif