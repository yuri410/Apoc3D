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

#include "D3D9IndexBuffer.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			IndexBufferType GetIBType(D3DIndexBuffer* ib)
			{
				D3DINDEXBUFFER_DESC desc;
				ib->GetDesc(&desc);

				return desc.Format == D3DFMT_INDEX16 ? IBT_Bit16 : IBT_Bit32;
			}
			int32 GetIBSize(D3DIndexBuffer* ib)
			{
				D3DINDEXBUFFER_DESC desc;
				ib->GetDesc(&desc);

				return desc.Size;
			}
			BufferUsageFlags GetIBUsage(D3DIndexBuffer* ib)
			{
				D3DINDEXBUFFER_DESC desc;
				ib->GetDesc(&desc);
				return D3D9Utils::GetBufferUsage(desc.Usage);
			}


			void D3D9IndexBuffer::ReleaseVolatileResource()
			{
				if (getUsage() & BU_Dynamic)
				{
					if (m_tempData)
					{
						delete[] m_tempData;
						m_tempData = NULL;
					}

					HRESULT hr;

					if ((getUsage() & BU_WriteOnly) == 0)
					{
						m_tempData = new char[getSize()];

						void* data;
						hr = m_indexBuffer->Lock(0, getSize(), &data, D3DLOCK_READONLY);
						assert(SUCCEEDED(hr));
						memcpy(m_tempData, data, getSize());
						hr = m_indexBuffer->Unlock();
						assert(SUCCEEDED(hr));
					}
					hr = m_indexBuffer->Release();
					assert(SUCCEEDED(hr));
					m_indexBuffer = 0;
				}
			}
			void D3D9IndexBuffer::ReloadVolatileResource()
			{
				if (getUsage() & BU_Dynamic)
				{
					D3DDevice* dev = m_device->getDevice();
					HRESULT hr = dev->CreateIndexBuffer(getSize(), 
						D3D9Utils::ConvertBufferUsage(getUsage()), 
						getIndexType() == IBT_Bit16 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, 
						D3DPOOL_DEFAULT, &m_indexBuffer, NULL);
					assert(SUCCEEDED(hr));

					if (m_tempData)
					{
						void* data;
						hr = m_indexBuffer->Lock(0,getSize(),&data,D3DLOCK_DISCARD);
						assert(SUCCEEDED(hr));
						memcpy(data, m_tempData, getSize());
						hr = m_indexBuffer->Unlock();
						assert(SUCCEEDED(hr));
					}
					delete[] m_tempData;
					m_tempData = 0;
				}
			}

			//D3D9IndexBuffer::D3D9IndexBuffer(D3D9RenderDevice* device, D3DIndexBuffer* ib)
			//	: IndexBuffer(GetIBType(ib), GetIBSize(ib), GetIBUsage(ib))
			//{

			//}
			D3D9IndexBuffer::D3D9IndexBuffer(D3D9RenderDevice* device, IndexBufferType type, int32 size, BufferUsageFlags usage)
				: IndexBuffer(type, size, usage), VolatileResource(device), m_device(device), m_tempData(0)
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


				HRESULT hr = dev->CreateIndexBuffer(size, 
					D3D9Utils::ConvertBufferUsage(usage), 
					type == IBT_Bit16 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, 
					pool, &m_indexBuffer, NULL);
				assert(SUCCEEDED(hr));
			}


			D3D9IndexBuffer::~D3D9IndexBuffer()
			{
				m_indexBuffer->Release();
			}


			void* D3D9IndexBuffer::lock(int offset, int size, LockMode mode)
			{
				void* result;
				HRESULT hr = m_indexBuffer->Lock(offset, size, &result, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				return result;
			}
			void D3D9IndexBuffer::unlock()
			{
				m_indexBuffer->Unlock();
			}

		}
	}
}