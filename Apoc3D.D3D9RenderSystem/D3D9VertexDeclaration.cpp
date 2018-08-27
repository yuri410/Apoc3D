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

#include "D3D9VertexDeclaration.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"

#include "apoc3d/Collections/List.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			List<VertexElement> GetElements(D3DVertexDeclaration* vtxdecl)
			{
				UINT count;
				HRESULT hr = vtxdecl->GetDeclaration(NULL, &count);
				assert(SUCCEEDED(hr));

				D3DVERTEXELEMENT9* elements = new D3DVERTEXELEMENT9[count];
				hr = vtxdecl->GetDeclaration(elements, &count);
				assert(SUCCEEDED(hr));

				List<VertexElement> result;
				if (count)
				{
					result.ResizeDiscard(count);

					for (size_t i = 0; i < count - 1; i++)
					{
						VertexElement elem(static_cast<int32>(elements[i].Offset),
							D3D9Utils::ConvertBackVertexElementFormat((D3DDECLTYPE)elements[i].Type),
							D3D9Utils::ConvertBackVertexElementUsage((D3DDECLUSAGE)elements[i].Usage), elements[i].UsageIndex);
						result.Add(elem);
					}
				}
				
				delete[] elements;
				return result;
			}

			D3D9VertexDeclaration::D3D9VertexDeclaration(D3D9RenderDevice* device, const List<VertexElement>& elements)
				: VertexDeclaration(elements), m_device(device)
			{

				D3DVERTEXELEMENT9* delems = new D3DVERTEXELEMENT9[elements.getCount() + 1];

				for (int i=0;i<elements.getCount();i++)
				{
					delems[i].Method = D3DDECLMETHOD_DEFAULT;
					delems[i].Stream = 0;
					delems[i].Offset = (WORD)elements[i].getOffset();
					delems[i].Usage = (BYTE) D3D9Utils::ConvertVertexElementUsage(elements[i].getUsage());
					delems[i].Type = (BYTE) D3D9Utils::ConvertVertexElementFormat(elements[i].getType());
					delems[i].UsageIndex = (BYTE) elements[i].getIndex();
				}

				const D3DVERTEXELEMENT9 declend = D3DDECL_END();
				delems[elements.getCount()] = declend;

				D3DDevice* dev = device->getDevice();
				
				HRESULT hr = dev->CreateVertexDeclaration(delems, &m_vtxDecl);
				assert(SUCCEEDED(hr));
				delete[] delems;
			}

			D3D9VertexDeclaration::D3D9VertexDeclaration(D3D9RenderDevice* device, D3DVertexDeclaration* vtxdecl)
				: VertexDeclaration(GetElements(vtxdecl)), m_device(device), m_vtxDecl(vtxdecl)
			{

			}

			D3D9VertexDeclaration::~D3D9VertexDeclaration()
			{
				m_vtxDecl->Release();
				m_vtxDecl = 0;
			}
		}
	}
}