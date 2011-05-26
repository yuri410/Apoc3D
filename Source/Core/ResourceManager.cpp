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
#include "ResourceManager.h"
#include "Resource.h"

namespace Apoc3D
{
	SINGLETON_DECL(Apoc3D::Core::ResourceManager);

	namespace Core
	{
		Resource* ResourceManager::Exists(const String& name)
		{
			//ResHashTable::const_iterator iter = m_hashTable.find(name);
			//if (iter != m_hashTable.end())
			//{
				//return iter->second;
			//}
			return 0;
		}

		void ResourceManager::NotifyNewResource(Resource* res)
		{
			//assert(!res->isManaged());
			
			//m_hashTable.insert(ResHashTable::value_type(res->getHashString(), res));
			
		}
		void ResourceManager::NotifyReleaseResource(Resource* res)
		{
			//assert(!res->isManaged());

			//m_hashTable.erase(res->getHashString());
		}
	}
}