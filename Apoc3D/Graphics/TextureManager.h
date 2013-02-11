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
#ifndef APOC3D_TEXTUREMANAGER_H
#define APOC3D_TEXTUREMANAGER_H

#include "Common.h"
#include "Core/ResourceManager.h"
#include "Core/Singleton.h"
#include "PixelFormat.h"

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
		class APAPI TextureManager : public ResourceManager, public Singleton<TextureManager>
		{
		private:
			FileLocation* m_redirectLocation;

		public:
			static int64 CacheSize;
			static bool UseCache;

			void SetRedirectLocation(FileLocation* fl);
			
			TextureManager();
			~TextureManager(void);

			Texture* CreateUnmanagedInstance(RenderDevice* rd, FileLocation* fl, bool genMips);
			ResourceHandle<Texture>* CreateInstance(RenderDevice* rd, FileLocation* fl, bool genMips);

			SINGLETON_DECL_HEARDER(TextureManager);
		};
	}
}
#endif