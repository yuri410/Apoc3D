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
#ifndef RESOURCE_H
#define RESOURCE_H

#include "Common.h"
#include "Core/Streaming/AsyncProcessor.h"
#include "Collections/FastQueue.h"
#include "tthread/tinythread.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core::Streaming;

using namespace tthread;

namespace Apoc3D
{
	namespace Core
	{
		
		enum APAPI ResourceState
		{
			RS_Unloaded = 0,
			RS_Loaded = 1,
			RS_Loading = 2,			
			RS_Unloading = 3,
			/** The resource is in the waiting queue. To be loaded or unloaded,
				
				When a resource is pending, no more load or unload operation is processed.
			*/
			RS_Pending = 4
		};
		
		template class APAPI fastdelegate::FastDelegate1<Resource*, void>;
		typedef fastdelegate::FastDelegate1<Resource*, void> ResourceEventHandler;


		class APAPI Resource
		{
		private:
			class ResourceLoadOperation : public ResourceOperation
			{
			public:
				ResourceLoadOperation(Resource* resource)
					: ResourceOperation(resource)
				{
					
				}

				void Process()
				{ 
					Resource* res = getResource();
					
					ResourceState state = res->getState();
					if (state != RS_Unloaded)
						return;
					
					res->setState(RS_Loading);
					res->load();
					res->setState(RS_Loaded);

					res->OnLoaded();
				}
			};
			class ResourceUnloadOperation : public ResourceOperation
			{
			public:
				ResourceUnloadOperation(Resource* resource)
					: ResourceOperation(resource)
				{
					
				}

				void Process()
				{ 
					Resource* res = getResource();

					ResourceState state = res->getState();
					if (state != RS_Loaded)
						return;
					res->setState(RS_Unloading);
					res->unload();
					res->setState(RS_Unloaded);

					res->OnUnloaded();
				}
			};
		
			class GenerationCalculator
			{
			public:
				volatile int Generation;
			private:
				const GenerationTable* m_table;
				FastQueue<float> m_timeQueue;

				fast_mutex m_queueLock;
				
			public:

				GenerationCalculator(const GenerationTable* table);

				void Use(Resource* resource);
				void UpdateGeneration();

				bool IsGenerationOutOfTime(float time);

			};

			GenerationCalculator* m_generation;
			ResourceManager* m_manager;

			const String m_hashString;

			int m_refCount;

			ResourceState m_state;

			ResourceLoadOperation* m_resLoader;
			ResourceUnloadOperation* m_resUnloader;
			
			fast_mutex m_lock;

			ResourceEventHandler m_eventLoaded;
			ResourceEventHandler m_eventUnloaded;

			void OnLoaded()
			{
				if (!m_eventLoaded.empty())
				{
					m_eventLoaded(this);
				}
			}
			void OnUnloaded()
			{
				if (!m_eventUnloaded.empty())
				{
					m_eventUnloaded(this);
				}
			}

			void LoadSync();
		protected:
			virtual void load() = 0;
			virtual void unload() = 0;

			/** Create a unmanaged resource
			*/
			Resource() 
				: m_refCount(0), m_manager(0), m_resLoader(0), m_resUnloader(0), m_state(RS_Unloaded)
			{
			}
			Resource(ResourceManager* manager, const String& hashString);

			
		public: 
			friend class GenerationTable;
			typedef Resource ResHandleTemplateConstraint;   
			
			virtual ~Resource();

			ResourceEventHandler& eventLoaded() { return m_eventLoaded; }
			ResourceEventHandler& eventUnloaded() { return m_eventUnloaded; }

			int GetGeneration() const 
			{
				if (m_generation)
					return m_generation->Generation;
				return -1;
			}
			int getReferenceCount() const { return m_refCount; }

			virtual bool IsUnloadable() const { return true; }

			virtual uint32 getSize() = 0;

			void Use();
			void UseSync();

			void Load();
			void Unload();

			const String& getHashString() const { return m_hashString; }
			bool isLoaded() { return getState() == RS_Loaded; }
			ResourceState getState()
			{
				ResourceState state;
				m_lock.lock();
				state = m_state;
				m_lock.unlock();
				return state;
			}
			void setState(ResourceState st)
			{
				m_lock.lock();
				m_state = st;
				m_lock.unlock();
			}
			bool isManaged() const { return !!m_manager; }

			void _Ref()
			{
				if (isManaged())
				{
					m_refCount++;
				}
			}
			void _Unref()
			{
				if (isManaged())
				{
					m_refCount--;
					//if (--m_refCount == 0)
					//{
						//delete this;
					//}
				}
			}
		};
	};
};
#endif