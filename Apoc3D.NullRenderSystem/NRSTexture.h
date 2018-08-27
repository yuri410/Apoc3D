#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#ifndef NRSTEXTURE_H
#define NRSTEXTURE_H

#include "NRSCommon.h"

#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/IOLib/TextureData.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSTexture final : public Apoc3D::Graphics::RenderSystem::Texture
			{
			public:
				NRSTexture(NRSRenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed);
				NRSTexture(NRSRenderDevice* device, int32 width, int32 height, int32 depth, int32 level,
					PixelFormat format, TextureUsage usage);

				NRSTexture(NRSRenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage);

				~NRSTexture();

				virtual void Save(Stream& strm) override;

				virtual DataRectangle lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect) override;
				virtual DataBox lock(int32 surface, LockMode mode, const Box& box) override;
				virtual DataRectangle lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, 
					const Apoc3D::Math::Rectangle& rect) override;

				virtual void unlock(int32 surface) override;
				virtual void unlock(CubeMapFace cubemapFace, int32 surface) override;

				/** Loads the content of the texture from a ResourceLocation.
				 *  This is called by the Resource class when the Resource need to load.
				 *  See Resource for more information.
				 */
				virtual void load() override;
				/** Release the resources used.
				 */
				virtual void unload() override;

			private:

				void LoadTexture(const ResourceLocation* rl);

				String getResourceLocationName(const ResourceLocation* rl);

				NRSRenderDevice* m_renderDevice;
				TextureData m_data;
			};
		}
	}
}

#endif