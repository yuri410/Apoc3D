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

#include "Streaming/AsyncProcessor.h"
#include "Streaming/GenerationTable.h"

namespace Apoc3D
{
	SINGLETON_DECL(Apoc3D::Core::ResourceManager);

	namespace Core
	{

		void ResourceManager::Resource_Loaded(Resource* res)
		{
			m_curUsedCache += res->getSize();
		}
		void ResourceManager::Resource_Unloaded(Resource* res)
		{
			m_curUsedCache -= res->getSize();
		}

		void ResourceManager::AddTask(ResourceOperation* op) const
		{
			m_asyncProc->AddTask(op);
		}

		void ResourceManager::Shutdown()
		{
			m_asyncProc->Shutdown();
		}

		ResourceManager::ResourceManager(int64 cacheSize)
			: m_totalCacheSize(cacheSize), m_curUsedCache(0)
		{
			m_asyncProc = new AsyncProcessor(L"");
			m_generationTable = new GenerationTable(this);
		}
		ResourceManager::~ResourceManager()
		{
			delete m_asyncProc;
			delete m_generationTable;
		}
		Resource* ResourceManager::Exists(const String& hashString)
		{
			Resource* res;
			ResHashTable::iterator iter = m_hashTable.find(hashString);

			if (iter != m_hashTable.end())
			{
				return iter->second;
			}

			return 0;
		}

		void ResourceManager::NotifyNewResource(Resource* res)
		{
			//assert(!res->isManaged());
			if (!m_isShutDown)
			{
				m_hashTable.insert(ResHashTable::value_type(res->getHashString(), res));
				m_generationTable->AddResource(res);
			}
		}
		void ResourceManager::NotifyReleaseResource(Resource* res)
		{
			//assert(!res->isManaged());
			if (!m_isShutDown)
			{
				m_hashTable.erase(res->getHashString());
				m_generationTable->RemoveResource(res);
			}
			//m_hashTable.erase(res->getHashString());
		}

		bool ResourceManager::IsIdle() const
		{
			return m_asyncProc->TaskCompleted();
		}
		void ResourceManager::WaitForIdle() const
		{
			m_asyncProc->WaitForCompletion();
		}
		int ResourceManager::GetCurrentOperationCount() const
		{
			return m_asyncProc->GetOperationCount();
		}

	}
}