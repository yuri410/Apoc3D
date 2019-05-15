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

#include "GenerationTable.h"
#include "apoc3d/Platform/Thread.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Core/Resource.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

#include <ctime>

using namespace Apoc3D::Platform;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			const float GenerationTable::GenerationLifeTime[MaxGeneration] = { 3, 10, 20, 30 };

			GenerationTable::GenerationTable(ResourceManager* mgr)
				: m_manager(mgr), m_isShutdown(false), m_generationList(100)
			{
				m_generations = new HashSet<Resource*>[MaxGeneration];
			}
			GenerationTable::~GenerationTable()
			{
				delete[] m_generations;
			}

			void GenerationTable::SubTask_GenUpdate()
			{
				clock_t timeStart = clock();

				int count;
				m_genLock.lock();
				count = m_generationList.getCount();
				m_genLock.unlock();

				{
					for (int j=0;j<count;j++)
					{
						Resource* res;
						m_genLock.lock();
						count = m_generationList.getCount();
						if (j<count)
							res = m_generationList[j];
						else
							break;
						m_genLock.unlock();

						if (res->m_generation->IsGenerationOutOfTime((float)timeStart/CLOCKS_PER_SEC))
						{
							int og = res->m_generation->Generation;
							res->m_generation->UpdateGeneration((float)timeStart/CLOCKS_PER_SEC);
							int ng = res->m_generation->Generation;
							if (ng!=og)
							{
								UpdateGeneration(og,ng,res);
			//#ifdef _DEBUG
								//LogManager::getSingleton().Write(LOG_System, L"GEN_CHG" + StringUtils::ToString(ng) +L" ("+ res->getHashString() + L")", LOGLVL_Default);
			//#endif
							}
						}
					}
				}
			}
			void GenerationTable::SubTask_Collect()
			{
				int64 predictSize = m_manager->getUsedCacheSize();

				if (predictSize>m_manager->getTotalCacheSize())
				{
					for (int i=MaxGeneration-1;i>1 && predictSize > m_manager->getTotalCacheSize(); i--)
					{
						m_genLock.lock();
						for (Resource* r : m_generations[i])
						{

							if (CanUnload(r) && r->IsUnloadable())
							{
								predictSize -= r->getSize();
								r->Unload();

								if (predictSize <= m_manager->getTotalCacheSize())
								{
									break;
								}
							}
						}
						m_genLock.unlock();
					}
				}
				{
					m_genLock.lock();
					for (Resource* r : m_generations[MaxGeneration - 1])
					{
						if (!r->getReferenceCount())
						{
							if (CanUnload(r) && r->IsUnloadable())
							{
								r->Unload();
							}
						}
					}
					m_genLock.unlock();
				}
			}
			bool GenerationTable::CanUnload(Resource* res) const
			{
				ResourceState state = res->getState();
				return state == ResourceState::Loaded;
			}

			void GenerationTable::AddResource(Resource* res)
			{
				int g = res->GetGeneration();
				if (g != -1)
				{
					m_genLock.lock();
					m_generations[g].Add(res);
					m_generationList.Add(res);
					m_genLock.unlock();
				}
			}

			void GenerationTable::RemoveResource(Resource* res)
			{
				int g = res->GetGeneration();
				if (g != -1)
				{
					m_genLock.lock();
					m_generations[g].Remove(res);
					m_generationList.Remove(res);
					m_genLock.unlock();
				}
			}

			void GenerationTable::UpdateGeneration(int oldGeneration, int newGeneration, Resource* resource)
			{
				m_genLock.lock();

				if (oldGeneration != -1 && m_generations[oldGeneration].Contains(resource))
				{
					m_generations[oldGeneration].Remove(resource);
				}
				if (!m_generations[newGeneration].Contains(resource))
				{
					m_generations[newGeneration].Add(resource);
				}

				m_genLock.unlock();
			}
		}
	}
}