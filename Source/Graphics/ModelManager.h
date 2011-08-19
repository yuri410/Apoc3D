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

#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Common.h"
#include "Core/ResourceManager.h"
#include "Core/Singleton.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI ModelManager : public ResourceManager, public Singleton<ModelManager>
		{
		public:
			static int64 CacheSize;
			static bool UseCache;
		private:
			
		public:
			ModelManager(void);
			~ModelManager(void);

			ResourceHandle<ModelSharedData>* CreateInstance(RenderDevice* renderDevice, 
				ResourceLocation* rl);

			ModelSharedData* CreateInstanceUnmanaged(RenderDevice* renderDevice, 
				ResourceLocation* rl);

			SINGLETON_DECL_HEARDER(ModelManager);
		};
	}
}

#endif