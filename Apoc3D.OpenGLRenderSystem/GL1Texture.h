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
#ifndef GL1TEXTURE_H
#define GL1TEXTURE_H

#include "GL1Common.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/GraphicsCommon.h"


using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class GL1Texture : public Apoc3D::Graphics::RenderSystem::Texture
			{
			public:
				void setInternalTexID(GLuint id) { m_textureID = id; }
				GLuint getInternalTexID() const { return m_textureID; }
			

				//GL1Texture(GL1RenderDevice* device, GLuint id, TextureType type);


				GL1Texture(GL1RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed);
				GL1Texture(GL1RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
					PixelFormat format, TextureUsage usage);

				GL1Texture(GL1RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage);

				~GL1Texture();
				virtual void Save(Stream* strm);

				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect);
				virtual DataBox lock(int32 surface, LockMode mode, const Box& box);
				virtual DataRectangle lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, 
					const Apoc3D::Math::Rectangle& rect);

				virtual void unlock(int32 surface);
				virtual void unlock(CubeMapFace cubemapFace, int32 surface);

				virtual void load();
				virtual void unload();

			private:
				GL1RenderDevice* m_renderDevice;
				GLuint m_textureID;

				GLenum m_lockedCubemapFace;
				char* m_lockBuffer;

				void InitializeEmptyGLTexture(int32 width, int32 height, int32 depth, int32 level, PixelFormat format);
			};
		}
	}
}

#endif