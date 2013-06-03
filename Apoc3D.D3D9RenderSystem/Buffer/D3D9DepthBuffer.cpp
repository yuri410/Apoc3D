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

#include "D3D9DepthBuffer.h"

#include "../D3D9RenderDevice.h"
#include "../D3D9Utils.h"

#include "apoc3d/ApocException.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			int32 GetDBWidth(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);

				return desc.Width;
			}
			int32 GetDBHeight(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);

				return desc.Height;
			}
			DepthFormat GetDBFormat(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);
				return D3D9Utils::ConvertBackDepthFormat(desc.Format);
			}

			D3D9DepthBuffer::D3D9DepthBuffer(D3D9RenderDevice* device, IDirect3DSurface9* buffer)
				: DepthBuffer(GetDBWidth(buffer), GetDBHeight(buffer), BU_Static, GetDBFormat(buffer)),
				m_buffer(buffer)
			{

			}


			D3D9DepthBuffer::~D3D9DepthBuffer()
			{
				m_buffer->Release();
			}


			void* D3D9DepthBuffer::lock(int offset, int size, LockMode mode)
			{
				throw AP_EXCEPTION(EX_NotSupported, L"DepthBuffer lock");
			}
			void D3D9DepthBuffer::unlock()
			{
				throw AP_EXCEPTION(EX_NotSupported, L"DepthBuffer unlock");
			}

		}
	}
}