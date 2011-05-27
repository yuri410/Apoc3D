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

#include "D3D9RenderTarget.h"

#include "D3D9RenderDevice.h"
#include "D3D9Texture.h"
#include "D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt)
				: RenderTarget(device, 
				D3D9Utils::GetD3DTextureWidth(rt),
				D3D9Utils::GetD3DTextureHeight(rt), 
				D3D9Utils::GetD3DTextureFormat(rt)),
				m_device(device), m_isMultisampled(false), m_color(rt), m_depthSurface(0), m_depthBuffer(0)
			{
				m_color->GetSurfaceLevel(0, &m_colorSurface);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt, IDirect3DSurface9* depth)
				: RenderTarget(device, 
				D3D9Utils::GetD3DTextureWidth(rt),
				D3D9Utils::GetD3DTextureHeight(rt), 
				D3D9Utils::GetD3DTextureFormat(rt)),
				m_device(device), m_isMultisampled(false), m_color(rt), m_depthSurface(depth)
			{
				m_color->GetSurfaceLevel(0, &m_colorSurface);
				//TODO
			}

			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format)
				: RenderTarget(device, width, height, format),
				m_device(device), m_isMultisampled(false), m_depthSurface(0), m_depthBuffer(0)
			{
				D3DDevice* dev = device->getDevice();

								
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format, DepthFormat depthFormat)
				: RenderTarget(device, width, height, format),
				m_device(device), m_isMultisampled(false)
			{

			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, uint32 sampleCount, PixelFormat format, DepthFormat depthFormat)
				: RenderTarget(device, width, height, format),
				m_device(device)
			{
				if (sampleCount)
				{
					m_isMultisampled = false;
				}
				else
				{
					m_isMultisampled = true;
				}
			}

			D3D9RenderTarget::~D3D9RenderTarget()
			{
				if (m_color)
				{
					m_color->Release();
					m_color = 0;
				}
				if (m_depthSurface)
				{
					m_depthSurface->Release();
					m_depthSurface = 0;
				}
				if (m_depthBuffer)
				{
					delete m_depthBuffer;
				}
			}

			Texture* D3D9RenderTarget::GetColorTexture()
			{

			}
			DepthBuffer* D3D9RenderTarget::GetDepthBuffer()
			{

			}

			void D3D9RenderTarget::Resolve()
			{

			}
		}
	}
}