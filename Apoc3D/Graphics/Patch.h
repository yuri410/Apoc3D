#pragma once
#ifndef APOC3D_PATCH_H
#define APOC3D_PATCH_H

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

#include "Renderable.h"
#include "RenderSystem/HardwareBuffer.h"
#include "RenderSystem/VertexElement.h"
#include "RenderOperationBuffer.h"

#include "apoc3d/Math/Matrix.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI Patch : public Renderable
		{
		public:
			Patch(RenderDevice* device, const void* vertexData, int vertexCount, const List<VertexElement>& vtxElems);
			~Patch();

			virtual RenderOperationBuffer* GetRenderOperation(int lod);

			void setMaterial(Material* mtrl)
			{
				m_mtrl = mtrl;
			}
			void setTransform(const Matrix& trans) { m_transfrom = trans; }
		private:
			VertexBuffer* m_vertexBuffer;
			IndexBuffer* m_indexBuffer;
			VertexDeclaration* m_vertexDecl;

			int m_vertexSize;
			int m_vertexCount;
			int m_primitiveCount;

			Matrix m_transfrom;
			Material* m_mtrl = nullptr;
			GeometryData* m_geoData = nullptr;
			RenderOperationBuffer m_opBuffer;

		};
	}
}

#endif