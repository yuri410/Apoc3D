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

#include "Logging.h"
#include "Streaming/AsyncProcessor.h"
#include "Streaming/GenerationTable.h"

#include "apoc3d/Apoc3DException.h"

namespace Apoc3D
{
	//SINGLETON_DECL(Apoc3D::Core::ResourceManager);

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
		bool ResourceManager::NeutralizeTask(ResourceOperation* op) const
		{
			return m_asyncProc->NeutralizeTask(op);
		}
		void ResourceManager::AddTask(ResourceOperation* op) const
		{
			if (!m_asyncProc)
			{
				throw Apoc3DException::createException(EX_NotSupported, L"Async processing not enabled");
			}
			m_asyncProc->AddTask(op);
		}
		void ResourceManager::RemoveTask(ResourceOperation* op) const
		{
			if (!m_asyncProc)
			{
				throw Apoc3DException::createException(EX_NotSupported, L"Async processing not enabled");
			}
			m_asyncProc->RemoveTask(op);
		}

		void ResourceManager::Shutdown()
		{
			if (m_asyncProc)
				m_asyncProc->Shutdown();
			if (m_generationTable)
				m_generationTable->ShutDown();
		}

		ResourceManager::ResourceManager(const String& name, int64 cacheSize, bool useAsync)
			: m_name(name), m_totalCacheSize(cacheSize), m_curUsedCache(0), m_isShutDown(false)
		{
			if (useAsync)
			{
				m_generationTable = new GenerationTable(this);
				m_asyncProc = new AsyncProcessor(m_generationTable, m_name + L" Async ResourceLoader");
			}
			else
			{
				m_asyncProc = 0; m_generationTable = 0;
			}
		}
		ResourceManager::~ResourceManager()
		{
			if (m_hashTable.size())
			{
				for (ResHashTable::iterator iter = m_hashTable.begin();iter!=m_hashTable.end();iter++)
				{
					if (iter->second->getState() == RS_Loaded)
					{
						LogManager::getSingleton().Write(LOG_System, 
							L"ResMgr: Resource leak detected: " + iter->first, LOGLVL_Warning);
					}
					
				}
				
			}
			if (m_asyncProc)
				delete m_asyncProc;
			if (m_generationTable)
				delete m_generationTable;
		}

		void ResourceManager::ReloadAll()
		{
			for (ResHashTable::iterator iter = m_hashTable.begin();iter!=m_hashTable.end();iter++)
			{
				Resource* res = iter->second;

				if (res->getState() == RS_Loaded)
				{
					res->Reload();
				}
			}
		}

		Resource* ResourceManager::Exists(const String& hashString)
		{
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

				if (m_generationTable)
					m_generationTable->AddResource(res);

				res->eventLoaded().bind(this, &ResourceManager::Resource_Loaded);
				res->eventUnloaded().bind(this, &ResourceManager::Resource_Unloaded);

				if (!usesAsync())
				{
					res->Load();
				}
			}
		}
		void ResourceManager::NotifyReleaseResource(Resource* res)
		{
			//assert(!res->isManaged());
			if (!m_isShutDown)
			{
				//if (!usesAsync())
				//{
				//	res->Unload();
				//}

				m_hashTable.erase(res->getHashString());

				if (m_generationTable)
					m_generationTable->RemoveResource(res);
			}
			//m_hashTable.erase(res->getHashString());
		}
		bool ResourceManager::IsIdle() const
		{
			if (!m_asyncProc)
			{
				throw Apoc3DException::createException(EX_NotSupported, L"Async processing not enabled");
			}
			return m_asyncProc->TaskCompleted();
		}
		void ResourceManager::WaitForIdle() const
		{
			if (!m_asyncProc)
			{
				throw Apoc3DException::createException(EX_NotSupported, L"Async processing not enabled");
			}
			m_asyncProc->WaitForCompletion();
		}
		int ResourceManager::GetCurrentOperationCount() const
		{
			if (!m_asyncProc)
			{
				throw Apoc3DException::createException(EX_NotSupported, L"Async processing not enabled");
			}
			return m_asyncProc->GetOperationCount();
		}

	}
}