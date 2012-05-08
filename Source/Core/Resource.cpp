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

#include "Resource.h"
#include "ResourceManager.h"
#include "Streaming/GenerationTable.h"
#include "Math/MathCommon.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core::Streaming;

namespace Apoc3D
{
	namespace Core
	{

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		Resource::Resource()
			: m_refCount(0), m_manager(0), m_resLoader(0), m_resUnloader(0), m_state(RS_Unloaded),  m_generation(0), m_unloadableLock(false)
		{

		}
		Resource::Resource(ResourceManager* manager, const String& hashString)
			: m_manager(manager), m_hashString(hashString), m_refCount(0), m_state(RS_Unloaded), m_resLoader(0), m_resUnloader(0), m_generation(0), m_unloadableLock(false)
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					m_resLoader = new ResourceLoadOperation(this);
					m_resUnloader = new ResourceUnloadOperation(this);

					m_generation = new GenerationCalculator(manager->m_generationTable);
				}
			}
			
		}
		Resource::~Resource()
		{
			m_eventLoaded.clear();
			m_eventUnloaded.clear();
			if (isManaged())
			{
				m_manager->NotifyReleaseResource(this);
			}
			if (m_generation)
				delete m_generation;
			if (m_resLoader)
			{
				m_manager->RemoveTask(m_resLoader);
				delete m_resLoader;
			}
			if (m_resUnloader)
			{
				m_manager->RemoveTask(m_resUnloader);
				delete m_resUnloader;
			}
		}

		void Resource::Use()		
		{
			if (isManaged())
			{
				if (m_manager->usesAsync())
				{
					m_generation->Use(this);

					if (getState() == RS_Unloaded)
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
				case RS_Loading:
				case RS_Unloading:
				case RS_PendingLoad:
				case RS_PendingUnload:
				case RS_Loaded:
					break;
				case RS_Unloaded:
					setState(RS_Loading);
					load();
					setState(RS_Loaded);
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

					m_generation->Use(this);

					ResourceState state = getState();
					switch (state)
					{
					case RS_Loading:
					case RS_Unloading:
					case RS_PendingLoad:
					case RS_Loaded:
						return;
					case RS_PendingUnload:
						{
							if (!m_manager->NeutralizeTask(m_resLoader))
							{
								setState(RS_PendingLoad);
								m_manager->AddTask(m_resLoader);
							}
							else
							{
								// rewind state if successfully neutralized
								setState(RS_Loaded);
							}
							break;
						}
					default:
						{
							setState(RS_PendingLoad);
							m_manager->AddTask(m_resLoader);
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
					assert(getState() == RS_Loaded || getState() == RS_PendingLoad);

					ResourceState state = getState();
					switch (state)
					{
					case RS_Loading:
					case RS_Unloading:
					case RS_PendingUnload:
					case RS_Unloaded:
						return;
					case RS_PendingLoad:
						{
							if (!m_manager->NeutralizeTask(m_resUnloader))
							{
								setState(RS_PendingUnload);
								m_manager->AddTask(m_resUnloader);
							}
							else
							{
								// rewind state if successfully neutralized
								setState(RS_Unloaded);
							}
							break;
						}
					default:
						{
							setState(RS_PendingUnload);
							m_manager->AddTask(m_resUnloader);
							break;
						}
					}

				}
				else
				{
					ResourceState state = getState();
					switch (state)
					{
					case RS_Loading:
					case RS_Unloading:
					case RS_PendingLoad:
					case RS_PendingUnload:
					case RS_Loaded:
					case RS_Unloaded:
						setState(RS_Unloading);
						unload();
						setState(RS_Unloaded);
						break;
					}
				}
				
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		Resource::GenerationCalculator::GenerationCalculator(const GenerationTable* table)
			: m_table(table), Generation(GenerationTable::MaxGeneration - 1)
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
				for (int i=0;i<m_timeQueue.getCount();i++)
				{
					result += m_timeQueue.GetElement(i);
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
			bool notEmpty;
			float topVal = 0;
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