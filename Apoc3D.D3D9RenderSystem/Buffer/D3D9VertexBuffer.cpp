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

#include "D3D9VertexBuffer.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			int32 GetVBSize(D3DVertexBuffer* vb)
			{
				D3DVERTEXBUFFER_DESC desc;
				vb->GetDesc(&desc);
				return desc.Size;
			}
			BufferUsageFlags GetVBUsage(D3DVertexBuffer* vb)
			{
				D3DVERTEXBUFFER_DESC desc;
				vb->GetDesc(&desc);
				return D3D9Utils::GetBufferUsage(desc.Usage);
			}
			


			void D3D9VertexBuffer::ReleaseVolatileResource()
			{
				if (getUsage() & BU_Dynamic)
				{
					if (m_tempData)
					{
						delete[] m_tempData;
						m_tempData = NULL;
					}

					HRESULT hr;
					if ((getUsage() & BU_WriteOnly)==0)
					{
						m_tempData = new char[getSize()];

						void* data;
						hr = m_vertexBuffer->Lock(0,getSize(), &data, D3DLOCK_READONLY);
						assert(SUCCEEDED(hr));
						memcpy(m_tempData, data, getSize());
						hr = m_vertexBuffer->Unlock();
						assert(SUCCEEDED(hr));
					}
					hr = m_vertexBuffer->Release();
					assert(SUCCEEDED(hr));
					m_vertexBuffer = 0;
				}
				
			}
			void D3D9VertexBuffer::ReloadVolatileResource()
			{
				if (getUsage() & BU_Dynamic)
				{
					D3DDevice* dev = m_device->getDevice();
					HRESULT hr = dev->CreateVertexBuffer(getSize(), 
						D3D9Utils::ConvertBufferUsage(getUsage()), 0, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL);
					assert(SUCCEEDED(hr));

					if (m_tempData)
					{
						void* data;
						hr = m_vertexBuffer->Lock(0, getSize(), &data, D3DLOCK_DISCARD);
						assert(SUCCEEDED(hr));
						memcpy(data, m_tempData, getSize());
						hr = m_vertexBuffer->Unlock();
						assert(SUCCEEDED(hr));
					}
					delete[] m_tempData;
					m_tempData = 0;
				}
			}


			//D3D9VertexBuffer::D3D9VertexBuffer(D3D9RenderDevice* device, D3DVertexBuffer* vb)
			//	: VertexBuffer(GetVBSize(vb), GetVBUsage(vb)), m_vertexBuffer(vb), m_device(device)
			//{

			//}
			D3D9VertexBuffer::D3D9VertexBuffer(D3D9RenderDevice* device, int32 size, BufferUsageFlags usage)
				: VertexBuffer(size, usage), VolatileResource(device), m_device(device), m_tempData(0)
			{
				D3DDevice* dev = device->getDevice();

				D3DPOOL pool;

				if (usage & BU_Dynamic)
				{
					pool = D3DPOOL_DEFAULT;
				}
				else
				{
					pool = D3DPOOL_MANAGED;
				}

				HRESULT hr = dev->CreateVertexBuffer(size, 
					D3D9Utils::ConvertBufferUsage(usage), 0, pool, &m_vertexBuffer, NULL);
				assert(SUCCEEDED(hr));
			}
			D3D9VertexBuffer::~D3D9VertexBuffer()
			{
				m_vertexBuffer->Release();
			}
			void* D3D9VertexBuffer::lock(int offset, int size, LockMode mode)
			{
				void* result;
				HRESULT hr = m_vertexBuffer->Lock(offset, size, &result, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				return result;
			}
			void D3D9VertexBuffer::unlock()
			{
				m_vertexBuffer->Unlock();
			}
		}
	}
}