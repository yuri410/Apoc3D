/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "AnimationManager.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "AnimationData.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			SINGLETON_IMPL(AnimationManager);

			AnimationManager::AnimationManager(void)
			{
			}

			AnimationManager::~AnimationManager(void)
			{
				m_hashTable.DeleteValuesAndClear();
			}

			const AnimationData* AnimationManager::CreateInstance(const ResourceLocation& rl)
			{
				AnimationData* res;
				//AnimHashTable::Enumerator e = m_hashTable(rl->GetHashString());

				if (m_hashTable.TryGetValue(rl.GetHashString(), res))
					return res;

				res = new AnimationData();

				res->Load(rl);
				m_hashTable.Add(rl.GetHashString(), res);

				return res;
			}
		}
	}
}