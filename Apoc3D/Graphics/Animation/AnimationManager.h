#pragma once
#ifndef APOC3D_ANIMATIONMANAGER_H
#define APOC3D_ANIMATIONMANAGER_H

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
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			typedef HashMap<String, AnimationData*> AnimHashTable;

			/** 
			 *  Animation is only a helper class to make sure animation data is not loaded multiple times.
			 *
			 *  Unlike other real resource management classes, this does not provide features like streaming.
			 *
			 *  All created animation data by this class can not be deleted. 
			 *  They will be deleted by the manager when the manager is shut down. 
			 *  Actually, user can load AnimationData manually if this AnimationManager is felt tricky.
			 *  locating a file and call Animation::Load will do the job, only few lines of code.
			 */
			class APAPI AnimationManager
			{
				SINGLETON_DECL(AnimationManager);
			public:
				AnimationManager();
				~AnimationManager();

				const AnimationData* CreateInstance(const ResourceLocation& rl);

			private:
				AnimHashTable m_hashTable;

			};
		}
	}
}

#endif