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

		ModelManager::ModelManager()
			: ResourceManager(L"Model Manager ", CacheSize, UseCache)
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Model manager initialized with a cache size " + StringUtils::IntToString(CacheSize) + (UseCache ? L". Use async streaming." : L"."), 
				LOGLVL_Infomation);
		}


		ModelManager::~ModelManager()
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
