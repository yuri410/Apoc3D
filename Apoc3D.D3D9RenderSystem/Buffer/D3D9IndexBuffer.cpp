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

#include "D3D9IndexBuffer.h"

#include "../D3D9RenderDevice.h"
#include "../D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			IndexBufferFormat GetIBType(D3DIndexBuffer* ib)
			{
				D3DINDEXBUFFER_DESC desc;
				ib->GetDesc(&desc);

				return desc.Format == D3DFMT_INDEX16 ? IndexBufferFormat::Bit16 : IndexBufferFormat::Bit32;
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
						getIndexType() == IndexBufferFormat::Bit16 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
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

			D3D9IndexBuffer::D3D9IndexBuffer(D3D9RenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage)
				: IndexBuffer(type, size, usage), VolatileResource(device), m_device(device)
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
					type == IndexBufferFormat::Bit16 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
					pool, &m_indexBuffer, NULL);
				assert(SUCCEEDED(hr));
			}


			D3D9IndexBuffer::~D3D9IndexBuffer()
			{
				m_indexBuffer->Release();

				if (m_tempData)
				{
					delete[] m_tempData;
					m_tempData = NULL;
				}
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