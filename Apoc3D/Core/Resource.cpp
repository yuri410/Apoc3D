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

#include "Resource.h"
#include "ResourceManager.h"
#include "Streaming/GenerationTable.h"

#include <ctime>

using namespace Apoc3D::Math;
using namespace Apoc3D::Core::Streaming;

namespace Apoc3D
{
	namespace Core
	{
		Resource::Resource() { }

		Resource::Resource(ResourceManager* manager, const String& hashString)
			: m_manager(manager), m_hashString(hashString)
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					m_generation = new GenerationCalculator(manager->m_generationTable);

					m_lock = new std::mutex();
				}
			}
			
		}
		Resource::~Resource()
		{
			if (isManaged())
			{
				m_manager->NotifyReleaseResource(this);
			}
			DELETE_AND_NULL(m_generation);

			if (isManaged() && m_manager->usesAsync())
				m_manager->RemoveTask(this);

			DELETE_AND_NULL(m_lock);
		}

		void Resource::Use()		
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					m_generation->Use(this);

					if (getState() == ResourceState::Unloaded)
						Load();
				}
				else
				{
					UseSync();
				}
			}			
		}
		void Resource::UseSync()
		{
			if (isManaged())
			{
				//m_generation->Use(this);

				LoadSync();
			}
		}

		void Resource::LoadSync()
		{
			if (isManaged())
			{
				ResourceState state = getState();
				switch (state)
				{
				case ResourceState::Loading:
				case ResourceState::Unloading:
				case ResourceState::Loaded:
					break;
				case ResourceState::Unloaded:
					setState(ResourceState::Loading);
					load();
					setState(ResourceState::Loaded);
					break;
				}
			}
		}
		void Resource::Load()
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					//assert(getState() == RS_Unloaded || getState() == RS_PendingUnload);

					ResourceState state = getState();
					switch (state)
					{
					case ResourceState::Loading:
					case ResourceState::Unloading:
					case ResourceState::Loaded:
						return;
					default:
						{
							if (!m_manager->NeutralizeTask(GetLoadOperation()))
							{
								m_manager->AddTask(GetLoadOperation());
							}
							break;
						}
					}

				}
				else
				{
					LoadSync();
				}
				
			}
		}

		void Resource::Unload()
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					ResourceState state = getState();
					assert(state == ResourceState::Loaded);

					switch (state)
					{
					case ResourceState::Loading:
					case ResourceState::Unloading:
					case ResourceState::Unloaded:
						return;
					default:
						{
							if (!m_manager->NeutralizeTask(GetUnloadOperation()))
							{
								m_manager->AddTask(GetUnloadOperation());
							}
							break;
						}
					}
				}
				else
				{
					ResourceState state = getState();
					switch (state)
					{
					case ResourceState::Loading:
					case ResourceState::Unloading:
					case ResourceState::Unloaded:
						return;
					default:
						setState(ResourceState::Unloading);
						unload();
						setState(ResourceState::Unloaded);
						break;
					}
				}
				
			}
		}

		void Resource::Reload()
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					if (getState() == ResourceState::Loaded)
					{
						m_manager->AddTask(GetUnloadOperation());
						m_manager->AddTask(GetLoadOperation());
					}
				}
				else
				{
					setState(ResourceState::Unloading);
					unload();
					setState(ResourceState::Unloaded);
					setState(ResourceState::Loading);
					load();
					setState(ResourceState::Loaded);
				}
			}
		}

		int Resource::GetGeneration() const
		{
			if (m_generation)
				return m_generation->Generation;
			return -1;
		}

		bool Resource::IsUnloadable()
		{
			if (m_lock)
			{
				volatile bool ul;
				m_lock->lock(); 
				ul = m_unloadableLock;
				m_lock->unlock();

				return !ul;
			}
			return m_unloadableLock;
		}

		ResourceState Resource::getState() const
		{
			if (m_lock)
			{
				volatile ResourceState state;
				m_lock->lock();
				state = m_state;
				m_lock->unlock();
				return state;
			}
			return m_state;
		}

		void Resource::setState(ResourceState st)
		{
			if (m_lock)
			{
				m_lock->lock();
				m_state = st;
				m_lock->unlock();
			}
			else
			{
				m_state = st;
			}
		}

		void Resource::Lock_Unloadable() { if (m_lock){ m_lock->lock(); m_unloadableLock = true; m_lock->unlock(); } }
		void Resource::Unlock_Unloadable() { if (m_lock) { m_lock->lock(); m_unloadableLock = false; m_lock->unlock(); } }

		void Resource::ProcessResourceOperation(const ResourceOperation& resOp)
		{
			Resource* res = resOp.Subject;
			if (resOp.Type == ResourceOperation::RESOP_Load)
			{
				ResourceState state = res->getState();

				if (state != ResourceState::Unloaded)
					return;

				res->setState(ResourceState::Loading);
				res->load();
				res->setState(ResourceState::Loaded);

				if (res->m_manager)
					res->m_manager->NotifyResourceLoaded(res);
			}
			else if (resOp.Type == ResourceOperation::RESOP_Unload)
			{
				ResourceState state = res->getState();
				if (state != ResourceState::Loaded)
					return;
				res->setState(ResourceState::Unloading);
				res->unload();
				res->setState(ResourceState::Unloaded);

				if (res->m_manager)
					res->m_manager->NotifyResourceUnloaded(res);
			}
			else assert(0);
		}
		void Resource::ProcessResourceOperationPostSyc(const ResourceOperation& resOp, int32 percentage)
		{
			Resource* res = resOp.Subject;

			if (resOp.Type == ResourceOperation::RESOP_Load)
			{
				assert(res->getState() == ResourceState::Loaded);
				res->loadPostSync(percentage);
			}
			else if (resOp.Type == ResourceOperation::RESOP_Unload)
			{
				assert(res->getState() == ResourceState::Unloaded);
				res->unloadPostSync(percentage);
			}
			else assert(0);
		}

		/************************************************************************/
		/*   Resource::GenerationCalculator                                     */
		/************************************************************************/

		Resource::GenerationCalculator::GenerationCalculator(const GenerationTable* table)
			: m_table(table), Generation(GenerationTable::MaxGeneration - 1)
		{
			
		}
		Resource::GenerationCalculator::~GenerationCalculator()
		{
			
		}

		void Resource::GenerationCalculator::Use(Resource* resource)
		{
			clock_t t = clock();

			m_queueLock.lock();
			m_timeQueue.Enqueue((float)t / CLOCKS_PER_SEC);
			while (m_timeQueue.getCount()>5)
				m_timeQueue.Dequeue();
			m_queueLock.unlock();
		}
		void Resource::GenerationCalculator::UpdateGeneration(float time)
		{
			float result = -9999999;

			m_queueLock.lock();

			if (m_timeQueue.getCount())
			{
				result = 0;
				for (int i = 0; i < m_timeQueue.getCount(); i++)
				{
					result += m_timeQueue[i];
				}
				result /= (float)m_timeQueue.getCount();
			}
			m_queueLock.unlock();

			//clock_t t = clock();

			//result = (float)t/ CLOCKS_PER_SEC - result;
			result = time - result;
			if (result > GenerationTable::GenerationLifeTime[0])
			{
				if (result > GenerationTable::GenerationLifeTime[1])
				{
					if (result > GenerationTable::GenerationLifeTime[2])
					{
						Generation = 3;
					}
					else
					{
						Generation = 2;
					}
				}
				else
				{
					Generation = 1;
				}
			}
			else
			{
				Generation = 0;
			}
		}

		bool Resource::GenerationCalculator::IsGenerationOutOfTime(float time)
		{
			volatile bool notEmpty;
			volatile float topVal = 0;
			m_queueLock.lock();
			notEmpty = !!m_timeQueue.getCount();
			if (notEmpty)
				topVal = m_timeQueue.Tail();
			m_queueLock.unlock();

			if (notEmpty)
			{
				float interval = time - topVal;

				if (Generation < GenerationTable::MaxGeneration && 
					interval > GenerationTable::GenerationLifeTime[Generation]) // become older
				{
					return true;
				}
				// become younger
				return Generation >0 && interval <= GenerationTable::GenerationLifeTime[Generation-1];

			}
			return Generation == GenerationTable::MaxGeneration -1;
		}
	}
}