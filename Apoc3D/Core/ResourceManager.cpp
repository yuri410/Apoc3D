/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "apoc3d/Exception.h"

namespace Apoc3D
{
	//SINGLETON_DECL(Apoc3D::Core::ResourceManager);

	namespace Core
	{
		ResourceManager::ManagerList ResourceManager::s_managers;

		ResourceManager::ResourceManager(const String& name, int64 cacheSize, bool useAsync)
			: m_name(name), m_totalCacheSize(cacheSize), m_curUsedCache(0), m_isShutDown(false)
		{
			if (useAsync)
			{
				m_generationTable = new GenerationTable(this);
				m_asyncProc = new AsyncProcessor(m_generationTable, m_name + L" Async ResourceLoader", true);
			}
			else
			{
				m_asyncProc = 0; m_generationTable = 0;
			}

			s_managers.Add(this);
		}
		
		ResourceManager::~ResourceManager()
		{
			if (m_hashTable.getCount())
			{
				for (auto e : m_hashTable)
				{
					if (e.Value->getState() == ResourceState::Loaded)
					{
						LogManager::getSingleton().Write(LOG_System, 
							L"ResMgr: Resource leak detected: " + e.Key, LOGLVL_Warning);
					}
				}
			}

			DELETE_AND_NULL(m_asyncProc);
			DELETE_AND_NULL(m_generationTable);

			s_managers.Remove(this);
		}


		void ResourceManager::Shutdown()
		{
			if (m_asyncProc)
				m_asyncProc->Shutdown();
			if (m_generationTable)
				m_generationTable->ShutDown();
		}

		void ResourceManager::ReloadAll()
		{
			for (Resource* res : m_hashTable.getValueAccessor())
			{
				if (res->getState() == ResourceState::Loaded)
				{
					res->Reload();
				}
			}
		}

		bool ResourceManager::IsIdle() const
		{
			CheckAsync();
			return m_asyncProc->TaskCompleted();
		}
		void ResourceManager::WaitForIdle() const
		{
			CheckAsync();
			m_asyncProc->WaitForCompletion();
		}

		int ResourceManager::GetCurrentOperationCount() const
		{
			CheckAsync();
			return m_asyncProc->GetOperationCount();
		}

		void ResourceManager::ProcessPostSync(float& timeLeft)
		{
			CheckAsync();
			m_asyncProc->ProcessPostSync(timeLeft);
		}

		Resource* ResourceManager::Exists(const String& hashString)
		{
			Resource* result;
			if (m_hashTable.TryGetValue(hashString, result))
				return result;
			return nullptr;
		}

		int64 ResourceManager::CalculateTotalResourceSize() const
		{
			int64 result = 0;
			for (Resource* r : m_hashTable.getValueAccessor())
			{
				result += r->getSize();
			}
			return result;
		}

		void ResourceManager::NotifyNewResource(Resource* res)
		{
			//assert(!res->isManaged());
			if (!m_isShutDown)
			{
				m_hashTable.Add(res->getHashString(), res);

				if (m_generationTable)
					m_generationTable->AddResource(res);

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
				
				m_hashTable.Remove(res->getHashString());

				if (m_generationTable)
					m_generationTable->RemoveResource(res);
			}
			//m_hashTable.erase(res->getHashString());
		}
		
		
		void ResourceManager::PerformAllPostSync(float timelimit)
		{
			const ResourceManager::ManagerList& mgrList = ResourceManager::getManagerInstances();
			for (int32 i = 0; i < mgrList.getCount(); i++)
			{
				if (mgrList[i]->usesAsync())
					mgrList[i]->ProcessPostSync(timelimit);
			}
		}

		void ResourceManager::NotifyResourceLoaded(Resource* res)
		{
			m_curUsedCache += res->getSize();
		}

		void ResourceManager::NotifyResourceUnloaded(Resource* res)
		{
			m_curUsedCache -= res->getSize();
		}

		bool ResourceManager::NeutralizeTask(const ResourceOperation& op) const
		{
			return m_asyncProc->NeutralizeTask(op);
		}

		void ResourceManager::AddTask(const ResourceOperation& op) const
		{
			CheckAsync();
			m_asyncProc->AddTask(op);
		}

		void ResourceManager::RemoveTask(const ResourceOperation& op) const
		{
			CheckAsync();
			m_asyncProc->RemoveTask(op);
		}

		void ResourceManager::RemoveTask(Resource* res) const
		{
			CheckAsync();
			m_asyncProc->RemoveTask(res);
		}

		void ResourceManager::CheckAsync() const
		{
			if (!m_asyncProc)
			{
				throw AP_EXCEPTION(ExceptID::NotSupported, L"Async processing not enabled");
			}
		}
	}
}