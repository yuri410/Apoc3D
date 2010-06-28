
#ifndef RESOUECEMANAGER_H
#define RESOURCEMANAGER_H

#include "..\Common.h"
#include "Singleton.h"

#include <unordered_map>

namespace Apoc3D
{
	namespace Core
	{
		typedef std::tr1::unordered_map<String, Resource*> ResHashTable;

		class _Export ResourceManager : public Singleton<ResourceManager>
		{
		private:
			ResHashTable m_hashTable;


		};
	}
}

#endif