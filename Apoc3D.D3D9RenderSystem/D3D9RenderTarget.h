#pragma once
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
#ifndef D3D9RENDERTARGET_H
#define D3D9RENDERTARGET_H

#include "D3D9Common.h"
#include "VolatileResource.h"

#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RenderTarget : public RenderTarget, public VolatileResource
			{
			private:
				D3DTexture2D* m_color;
				IDirect3DSurface9* m_colorSurface;
				D3D9Texture* m_d3dTexture;

				IDirect3DSurface9* m_depthSurface;
				D3D9DepthBuffer* m_depthBuffer;

				D3D9RenderDevice* m_device;
				
				bool m_isDefault;

				bool m_hasColor;
				bool m_hasDepth;

			public:

				virtual void ReleaseVolatileResource();
				virtual void ReloadVolatileResource();

				IDirect3DSurface9* getColorSurface() const { return m_colorSurface; }
				IDirect3DSurface9* getDepthSurface() const { return m_depthSurface; }

				D3D9RenderTarget(D3D9RenderDevice* device, IDirect3DSurface9* color, IDirect3DSurface9* depth);

				D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt);
				D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt, IDirect3DSurface9* depth);
				
				D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format);
				D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format, DepthFormat depthFormat);
				D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, uint32 sampleCount, PixelFormat format, DepthFormat depthFormat);

				~D3D9RenderTarget();

				virtual Texture* GetColorTexture();
				virtual DepthBuffer* GetDepthBuffer();

				/** When the RT is just un-binded from the device.
				 *  This will be called by the RenderDevice to resolve the color texture.
				 */
				void Resolve();
			};
		}
	}
}
#endif