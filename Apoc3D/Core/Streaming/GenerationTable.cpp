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

#include "GenerationTable.h"
#include "Platform/Thread.h"
#include "Core/ResourceManager.h"
#include "Core/Resource.h"
#include "Core/Logging.h"
#include "Utility/StringUtils.h"

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
				m_generations = new ExistTable<Resource*>[MaxGeneration];

				//m_thread = new thread(&GenerationTable::ThreadEntry, this);
				//SetThreadName(m_thread, mgr->getName() + L" Generation");
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
						ExistTable<Resource*>::Enumerator iter = m_generations[i].GetEnumerator();

						while (iter.MoveNext())
						{
							Resource* r = *iter.getCurrent();

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
					for (ExistTable<Resource*>::Enumerator iter = m_generations[MaxGeneration-1].GetEnumerator();iter.MoveNext();)
					{
						Resource* r = *iter.getCurrent();
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
				return state == RS_PendingLoad || state == RS_Loaded;
			}
			//void GenerationTable::GenerationUpdate_Main()
			//{
			//	static const int ManageInterval = 10;

			//	int times = 0;
			//	while (!m_isShutdown)
			//	{
			//		SubTask_GenUpdate();
			//		if ((times++) % ManageInterval == 0)
			//		{
			//			SubTask_Manage();
			//		}
			//		ApocSleep(100);
			//	}
			//}

			void GenerationTable::AddResource(Resource* res)
			{
				int g = res->GetGeneration();
				if (g!=-1)
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
				if (g!=-1)
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

				if (oldGeneration != -1 && m_generations[oldGeneration].Exists(resource))
				{
					m_generations[oldGeneration].Remove(resource);
				}
				if (!m_generations[newGeneration].Exists(resource))
				{
					m_generations[newGeneration].Add(resource);
				}

				m_genLock.unlock();
			}
		}
	}
}