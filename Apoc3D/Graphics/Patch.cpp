/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
				m_geoData->PrimitiveType = PT_TriangleStrip;
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