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

#ifndef GL3TEXTURE_H
#define GL3TEXTURE_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
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
		namespace GL3RenderSystem
		{
			class GL3Texture : public Apoc3D::Graphics::RenderSystem::Texture
			{
			public:
				GL3Texture(GL3RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed);
				GL3Texture(GL3RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
					PixelFormat format, TextureUsage usage);
				GL3Texture(GL3RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage);
				~GL3Texture();

				virtual void Save(Stream* strm);

				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect);
				virtual DataBox lock(int32 surface, LockMode mode, const Box& box);
				virtual DataRectangle lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, 
					const Apoc3D::Math::Rectangle& rect);

				virtual void unlock(int32 surface);
				virtual void unlock(CubeMapFace cubemapFace, int32 surface);

				virtual void load();
				virtual void unload();

				GLuint getGLTexID() const { return m_textureID; }

			private:

				void InitializeGLTexture(int32 width, int32 height, int32 depth, int32 level, PixelFormat format);

				GL3RenderDevice* m_renderDevice;
				GLuint m_textureID = 0;
				GLuint m_mapPBO = 0;

				GLenum m_target;


				GLenum m_lockedCubemapFace;
				
			};
		}
	}
}

#endif