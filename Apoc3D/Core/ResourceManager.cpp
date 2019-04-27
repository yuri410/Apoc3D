/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2010-2017 Tao Xin
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

#include "ResourceManager.h"
#include "Resource.h"

#include "Logging.h"
#include "Streaming/AsyncProcessor.h"
#include "Streaming/GenerationTable.h"

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
				AP_EXCEPTION(ErrorID::NotSupported, L"Async processing not enabled");
			}
		}
	}
}