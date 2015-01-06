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
#include "D3D9InstancingData.h"
#include "D3D9RenderDevice.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9Utils.h"

#include "apoc3d/Graphics/RenderOperationBuffer.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			static const D3DVERTEXELEMENT9 InstanceVertexElements[2] = 
			{
				{1, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 15},
				D3DDECL_END()
			};

			D3D9InstancingData::D3D9InstancingData(D3D9RenderDevice* device)
				: InstancingData(device), m_d3ddev(device)
			{
				IDirect3DDevice9* dd = m_d3ddev->getDevice();
				HRESULT hr = dd->CreateVertexBuffer(getInstanceDataSize()*MaxOneTimeInstances, 
					D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_vertexBuffer,0);
				assert(SUCCEEDED(hr));

				// fill up buffer with instance index 
				void* dstData;
				m_vertexBuffer->Lock(0,0, &dstData, 0);

				float* dst = reinterpret_cast<float*>(dstData);
				for (int i = 0; i < MaxOneTimeInstances; i++)
				{
					dst[i] = (float)i;
				}

				m_vertexBuffer->Unlock();

			}

			D3D9InstancingData::~D3D9InstancingData()
			{
				for (IDirect3DVertexDeclaration9* decl : m_vtxDeclExpansionTable.getValueAccessor())
				{
					decl->Release();
				}
				m_vertexBuffer->Release();
				m_vertexBuffer = 0;
			}

			IDirect3DVertexDeclaration9* D3D9InstancingData::ExpandVertexDecl(VertexDeclaration* decl)
			{
				// add the instancing vertex element on the given one
				// save it the a map for future look ups

				// Since render system API layer does not support multi-stream mixing
				// this is the only way to do it, API specific

				IDirect3DVertexDeclaration9* result;
				if (!m_vtxDeclExpansionTable.TryGetValue(decl, result))
				{
					D3DVERTEXELEMENT9* elems = new D3DVERTEXELEMENT9[decl->getElementCount() + 2];
					for (int i = 0; i < decl->getElementCount(); i++)
					{
						const VertexElement& e = decl->getElement(i);
						elems[i].Method = D3DDECLMETHOD_DEFAULT;
						elems[i].Stream = 0;
						elems[i].Offset = (WORD)e.getOffset();
						elems[i].Usage = (BYTE)D3D9Utils::ConvertVertexElementUsage(e.getUsage());
						elems[i].Type = (BYTE)D3D9Utils::ConvertVertexElementFormat(e.getType());
						elems[i].UsageIndex = (BYTE)e.getIndex();
					}

					elems[decl->getElementCount()] = InstanceVertexElements[0];
					elems[decl->getElementCount() + 1] = InstanceVertexElements[1];

					HRESULT hr = m_d3ddev->getDevice()->CreateVertexDeclaration(elems, &result);
					assert(SUCCEEDED(hr));

					m_vtxDeclExpansionTable.Add(decl, result);

					delete[] elems;
				}
				return result;
			}
			int D3D9InstancingData::Setup(const RenderOperation* op, int count, int beginIndex)
			{
				// In d3d no more setup is needed
				// The instancing index is mixed in the vertex stream. The rest of work would be: per-instancing transformations
				// set in effects.

				return min(MaxOneTimeInstances, count-beginIndex);
			}


		}
	}
}