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
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Common.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/LockData.h"
#include "Core\Resource.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Core;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APAPI Texture : public Resource
			{
			private:
				//IDirect3DBaseTexture9* m_baseTexture;
				RenderDevice* m_renderDevice;
				const ResourceLocation* m_resourceLocation;
				TextureType m_type;
				int32 m_width;
				int32 m_height;
				int32 m_depth;
				int32 m_contentSize;
				int32 m_levelCount;
				TextureUsage m_usage;
				PixelFormat m_format;

				bool m_isLocked;
			protected:
				void UpdateInfo(const TextureData& data);

				Texture(RenderDevice* device, const ResourceLocation* rl, TextureUsage usage, bool managed);
				Texture(RenderDevice* device, int32 width, int32 height, int32 depth, 
					int32 levelCount, PixelFormat format, TextureUsage usage);
				Texture(RenderDevice* device, int32 length, int32 levelCount, TextureUsage usage, PixelFormat format);


				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rectangle) = 0;
				virtual DataBox lock(int surface, LockMode mode, const Box& box) = 0;
				virtual DataRectangle lock(int surface, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rectangle) = 0;

				virtual void unlock(int surface) = 0;
				virtual void unlock(CubeMapFace cubemapFace, int surface) = 0;

			public:
				bool isLocked() const { return m_isLocked; }
				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				const ResourceLocation* getResourceLocation() const { return m_resourceLocation; }
				TextureType getType() const { return m_type; }
				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }
				int32 getDepth() const { return m_depth; }
				int32 getContentSize() const { return m_contentSize; }
				int32 getLevelCount() const { return m_levelCount; }
				TextureUsage getUsage() const { return m_usage; }
				PixelFormat getFormat() const { return m_format; }


				DataRectangle Lock(int32 surface, LockMode mode, CubeMapFace cubemapFace, const Apoc3D::Math::Rectangle& rect);
				DataRectangle Lock(int32 surface, LockMode mode, CubeMapFace cubemapFace);
				DataRectangle Lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect);
				DataRectangle Lock(int32 surface, LockMode mode);
				DataBox Lock(int32 surface, LockMode mode, const Box& box);

				void Unlock(int32 surface);
				void Unlock(CubeMapFace cubemapFace, int32 surface);

				virtual void SetData(const char* data, int32 length) = 0;
				virtual void Save(Stream* strm) = 0;
			};
		}

	}
}
#endif