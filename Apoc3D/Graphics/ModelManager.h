#pragma once
#ifndef APOC3D_MODELMANAGER_H
#define APOC3D_MODELMANAGER_H

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

using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		/** 
		 *  The resource manager for all ModelSharedData.
		 *
		 *  In addition, this class can also create unmanaged resource. 
		 *  CreateUnmanagedInstance directly loads a model; then return it. Instances created in this
		 *  way need to be taken care by the client code. Deleting is required when no longer used.
		 *
		 */
		class APAPI ModelManager : public ResourceManager
		{
			SINGLETON_DECL(ModelManager);
		public:
			static int64 CacheSize;
			static bool UseCache;

			ModelManager();
			~ModelManager();

			ModelSharedData* CreateInstanceUnmanaged(RenderDevice* renderDevice, const ResourceLocation& rl);
			ResourceHandle<ModelSharedData>* CreateInstance(RenderDevice* renderDevice, const ResourceLocation& rl);
			
		};
	}
}

#endif