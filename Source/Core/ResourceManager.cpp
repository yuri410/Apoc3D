
#include "ResourceManager.h"
#include "Resource.h"

namespace Apoc3D
{
	namespace Core
	{
		Resource* ResourceManager::Exists(const String& name)
		{
			ResHashTable::const_iterator iter = m_hashTable.find(name);
			if (iter != m_hashTable.end())
			{
				return iter->second;
			}
			return 0;
		}

		void ResourceManager::NotifyNewResource(Resource* res)
		{
			assert(!res->getIsManaged());
			
			m_hashTable.insert(ResHashTable::value_type(res->getHashString(), res));
			
		}
		void ResourceManager::NotifyReleaseResource(Resource* res)
		{
			assert(!res->getIsManaged());

			m_hashTable.erase(res->getHashString());
		}
	}
}