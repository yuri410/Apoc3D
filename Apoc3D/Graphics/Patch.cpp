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

#include "Patch.h"

#include "apoc3d/Graphics/RenderSystem/VertexDeclaration.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "RenderOperationBuffer.h"
#include "RenderOperation.h"
#include "GeometryData.h"

namespace Apoc3D
{
	namespace Graphics
	{
		Patch::Patch(RenderDevice* device, const void* vertexData, int vertexCount, const List<VertexElement>& vtxElems)
			: m_primitiveCount(vertexCount - 3 + 2), m_vertexCount(vertexCount)
		{
			assert(vertexCount>3);

			ObjectFactory* objectFactory = device->getObjectFactory();
			m_vertexDecl = objectFactory->CreateVertexDeclaration(vtxElems);

			m_vertexBuffer = objectFactory->CreateVertexBuffer(vertexCount, m_vertexDecl, BU_Dynamic);
		}
		Patch::~Patch()
		{
			delete m_vertexBuffer;
			delete m_vertexDecl;
			delete m_indexBuffer;
		}

		RenderOperationBuffer* Patch::GetRenderOperation(int lod)
		{
			if (!m_mtrl)
				return nullptr;

			if (m_opBuffer.getCount()==0)
			{
				m_geoData = new GeometryData();
				m_geoData->BaseVertex = 0;
				m_geoData->IndexBuffer = m_indexBuffer;
				m_geoData->PrimitiveCount = m_primitiveCount;
				m_geoData->PrimitiveType = PrimitiveType::TriangleStrip;
				m_geoData->VertexBuffer = m_vertexBuffer;
				m_geoData->VertexCount = m_vertexCount;
				m_geoData->VertexDecl = m_vertexDecl;
				m_geoData->VertexSize = m_vertexSize;

				RenderOperation rop;

				rop.GeometryData = m_geoData;

				m_opBuffer.Add(rop);
			}

			m_opBuffer[0].Material = m_mtrl;
			m_opBuffer[0].RootTransform = m_transfrom;
			return &m_opBuffer;
		}
	}
}