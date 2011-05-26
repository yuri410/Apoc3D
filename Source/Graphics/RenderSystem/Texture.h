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
//#include "Core\Resource.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			enum TextureType
			{
				TT_Texture1D = 0,
				TT_Texture2D,
				TT_Texture3D,
				TT_CubeTexture
			};
			class APAPI Texture// : public Resource
			{
			private:
				//IDirect3DBaseTexture9* m_baseTexture;
				RenderDevice* m_renderDevice;
				const ResourceLocation* m_resourceLocation;
				TextureType* m_type;
				int32 m_width;
				int32 m_height;
				int32 m_depth;
				int32 m_contentSize;
				int32 m_levelCount;
				TextureUsage m_usage;
				PixelFormat m_format;
			protected:

			public:
				Texture(void);
				~Texture(void);
			};
		}

	}
}
#endif