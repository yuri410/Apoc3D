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
#include "Apoc3D/Graphics/RenderSystem/Texture.h"
#include "Apoc3D/Graphics/GraphicsCommon.h"
#include "Apoc3D/Math/Rectangle.h"
#include "Apoc3D/Math/Box.h"

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
				GL3Texture(GL3RenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed);
				GL3Texture(GL3RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
					PixelFormat format, TextureUsage usage);
				GL3Texture(GL3RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage);
				~GL3Texture();

				virtual void Save(Stream& strm) override;

				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect) override;
				virtual DataBox lock(int32 surface, LockMode mode, const Box& box) override;
				virtual DataRectangle lock(int32 surface, CubeMapFace cubemapFace, LockMode mode,
					const Apoc3D::Math::Rectangle& rect) override;

				virtual void unlock(int32 surface) override;
				virtual void unlock(CubeMapFace cubemapFace, int32 surface) override;

				virtual void load() override;
				virtual void unload() override;

				GLuint getGLTexID() const { return m_textureID; }
				GLenum getGLTexTarget() const { return m_glTarget; }

			private:

				void InitializeGLTexture(const TextureData* td);
				void LoadTexture(const ResourceLocation* rl);

				GL3RenderDevice* m_renderDevice;
				GLuint m_textureID = 0;
				GLuint m_mapPBO = 0;

				GLenum m_glFormat;
				GLenum m_glTarget;
				GLenum m_glType;

				GLint* m_levelOffsets = nullptr;

				GLint  m_lockedPboOffset;
				GLenum m_lockedCubemapFace;
				GLint  m_lockedLevel;
				Apoc3D::Math::Rectangle m_lockedRect;
				Apoc3D::Math::Box m_lockedBox;
			};
		}
	}
}

#endif