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
#ifndef D3D9TEXTURE_H
#define D3D9TEXTURE_H

#include "D3D9Common.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/GraphicsCommon.h"
#include "VolatileResource.h"

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
			class D3D9Texture : public Apoc3D::Graphics::RenderSystem::Texture
			{
			private:
				D3DTexture2D* m_tex2D;
				D3DTexture3D* m_tex3D;
				D3DTextureCube* m_cube;
				D3D9RenderDevice* m_renderDevice;

				D3DCUBEMAP_FACES m_lockedCubeFace;

			public:
				void setInternal2D(D3DTexture2D* tex) { m_tex2D = tex; }

				D3DTexture2D* getInternal2D() const { return m_tex2D; }
				D3DTexture3D* getInternal3D() const { return m_tex3D; }
				D3DTextureCube* getInternalCube() const { return m_cube; }

				D3DBaseTexture* getBaseTexture() const
				{
					if (m_tex2D)
						return m_tex2D;
					if (m_tex3D)
						return m_tex3D;
					if (m_cube)
						return m_cube;
					return 0;
				}

				D3D9Texture(D3D9RenderDevice* device, D3DTexture2D* tex2D);
				D3D9Texture(D3D9RenderDevice* device, D3DTexture3D* tex3D);
				D3D9Texture(D3D9RenderDevice* device, D3DTextureCube* texCube);

				D3D9Texture(D3D9RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed);
				D3D9Texture(D3D9RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
					PixelFormat format, TextureUsage usage);

				D3D9Texture(D3D9RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage);

				~D3D9Texture();
				virtual void Save(Stream* strm);

				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect);
				virtual DataBox lock(int32 surface, LockMode mode, const Box& box);
				virtual DataRectangle lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, 
					const Apoc3D::Math::Rectangle& rect);

				virtual void unlock(int32 surface);
				virtual void unlock(CubeMapFace cubemapFace, int32 surface);

				virtual void load();
				virtual void unload();

			};
		}
	}
}

#endif