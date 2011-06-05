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

#include "ModelManager.h"

#include "Core/Logging.h"
#include "Core/ResourceHandle.h"
#include "Utility/StringUtils.h"
#include "Vfs/ResourceLocation.h"
#include "Model.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	SINGLETON_DECL(Apoc3D::Graphics::ModelManager);

	namespace Graphics
	{
		int64 ModelManager::CacheSize = 100 * 1048576;

		ModelManager::ModelManager(void)
			: ResourceManager(CacheSize)
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Model manager initialized with a cache size " + StringUtils::ToString(CacheSize), 
				LOGLVL_Infomation);
		}


		ModelManager::~ModelManager(void)
		{
			ResourceManager::~ResourceManager();
			Singleton::~Singleton();
		}

		ResourceHandle<ModelSharedData>* ModelManager::CreateInstance(RenderDevice* renderDevice, 
			ResourceLocation* rl)
		{
			Resource* retrived = Exists(rl->getName());
			if (!retrived)
			{
				ModelSharedData* mdl = new ModelSharedData(renderDevice, rl);
				retrived = mdl;
				NotifyNewResource(retrived);
			}
			return new ResourceHandle<ModelSharedData>((ModelSharedData*)retrived);
		}
	}
}
