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

#include "D3D9VertexDeclaration.h"

#include "D3D9RenderDevice.h"

#include "D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			vector<VertexElement> GetElements(D3DVertexDeclaration* vtxdecl)
			{
				UINT count;
				HRESULT hr = vtxdecl->GetDeclaration(NULL, &count);
				assert(SUCCEEDED(hr));

				D3DVERTEXELEMENT9* elements = new D3DVERTEXELEMENT9[count];
				hr = vtxdecl->GetDeclaration(elements, &count);
				assert(SUCCEEDED(hr));

				vector<VertexElement> result;
				if (count)
				{
					result.reserve(count);

					for (size_t i=0;i<count-1;i++)
					{
						VertexElement elem(static_cast<int32>(elements[i].Offset), 
							D3D9Utils::ConvertBackVertexElementFormat((D3DDECLTYPE)elements[i].Type), 
							D3D9Utils::ConvertBackVertexElementUsage((D3DDECLUSAGE)elements[i].Usage), elements[i].UsageIndex);
						result.push_back(elem);
					}

				}
				
				delete[] elements;
				return result;
			}
			D3D9VertexDeclaration::D3D9VertexDeclaration(D3D9RenderDevice* device, const vector<VertexElement>& elements)
				: VertexDeclaration(elements), m_device(device)
			{

				D3DVERTEXELEMENT9* delems = new D3DVERTEXELEMENT9[elements.size() + 1];

				for (size_t i=0;i<elements.size();i++)
				{
					delems[i].Method = D3DDECLMETHOD_DEFAULT;
					delems[i].Stream = 0;
					delems[i].Offset = elements[i].getOffset();
					delems[i].Usage = D3D9Utils::ConvertVertexElementUsage(elements[i].getUsage());
					delems[i].Type = D3D9Utils::ConvertVertexElementFormat(elements[i].getType());
					delems[i].UsageIndex = elements[i].getIndex();
				}

				const D3DVERTEXELEMENT9 declend = D3DDECL_END();
				delems[elements.size()] = declend;

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