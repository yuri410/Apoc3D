
#ifndef RESOUECEMANAGER_H
#define RESOURCEMANAGER_H

#include "..\Common.h"
#include "Singleton.h"

namespace Apoc3D
{
	namespace Core
	{
		typedef std::tr1::unordered_map<const String, Resource*> ResHashTable;

		class _Export ResourceManager : public Singleton<ResourceManager>
		{
		private:
			ResHashTable m_hashTable;

		protected:
			Resource* Exists(const String& name);

			void NotifyNewResource(Resource* res);
			void NotifyReleaseResource(Resource* res);
		};
	}
}

#endif