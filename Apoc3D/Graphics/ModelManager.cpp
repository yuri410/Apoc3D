/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "Model.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		SINGLETON_IMPL(ModelManager);

		int64 ModelManager::CacheSize = 100 * 1048576;
		bool ModelManager::UseCache = true;

		ModelManager::ModelManager(void)
			: ResourceManager(L"Model Manager ", CacheSize, UseCache)
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Model manager initialized with a cache size " + StringUtils::IntToString(CacheSize) + (UseCache ? L". Use async streaming." : L"."), 
				LOGLVL_Infomation);
		}


		ModelManager::~ModelManager(void)
		{
		}

		ResourceHandle<ModelSharedData>* ModelManager::CreateInstance(RenderDevice* renderDevice, const ResourceLocation& rl)
		{
			Resource* retrived = Exists(rl.GetHashString());
			if (retrived == nullptr)
			{
				ModelSharedData* mdl = new ModelSharedData(renderDevice, rl);
				retrived = mdl;
				NotifyNewResource(retrived);
				if (!usesAsync())
					mdl->Load();
			}
			return new ResourceHandle<ModelSharedData>((ModelSharedData*)retrived);
		}

		ModelSharedData* ModelManager::CreateInstanceUnmanaged(RenderDevice* renderDevice, const ResourceLocation& rl)
		{			
			return new ModelSharedData(renderDevice, rl,false);
		}
	}
}
