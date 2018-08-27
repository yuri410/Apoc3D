#pragma once
#ifndef APOC3D_TEXTUREMANAGER_H
#define APOC3D_TEXTUREMANAGER_H

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

#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Graphics/PixelFormat.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		/**
		 *  The resource manager for textures.
		 * 
		 *  In addition, this class can also create unmanaged resource. 
		 *  CreateUnmanagedInstance directly loads a texture; then return it. Instances created in this
		 *  way need to be taken care by the client code. Deleting is required when no longer used.
		 */
		class APAPI TextureManager : public ResourceManager
		{
			SINGLETON_DECL(TextureManager);

		public:
			static int64 CacheSize;
			static bool UseCache;

			void SetRedirectLocation(FileLocation* fl);
			
			TextureManager();
			~TextureManager();

			Texture* CreateUnmanagedInstance(RenderDevice* rd, const FileLocation& fl, bool generateMips = false);
			ResourceHandle<Texture>* CreateInstance(RenderDevice* rd, const FileLocation& fl, bool generateMips = false);

		private:
			FileLocation* m_redirectLocation;

		};
	}
}
#endif