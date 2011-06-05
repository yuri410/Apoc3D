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
#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "Core/Singleton.h"
#include "Core/ResourceManager.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			template class APAPI std::unordered_map<String, AnimationData*>;
			typedef std::unordered_map<String, AnimationData*> AnimHashTable;

			class APAPI AnimationManager : public Singleton<AnimationManager>
			{
			private:
				AnimHashTable m_hashTable;

			public:
				AnimationManager();
				~AnimationManager();

				const AnimationData* CreateInstance(const ResourceLocation* rl);

				SINGLETON_DECL_HEARDER(AnimationManager);
			};
		}
	}
}

#endif