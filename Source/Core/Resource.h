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
		/** Defines some states used when a resource is being processed.
		 *
		 *  When async streaming is turn on in the corresponding resource manager, 
		 *	RS_PendingLoad and RS_PendingUnload may occur. Otherwise, the state will only
		 *	be one of the first four.
		 */
		enum APAPI ResourceState
		{
			/** Represents the resource is already unloaded or not loaded yet. */
			RS_Unloaded = 0,
			/** Represents the resource is already loaded. */
			RS_Loaded = 1,
			/** Represents the resource is currently being loaded */
			RS_Loading = 2,			
			/** Represents the resource is currently being unloaded */
			RS_Unloading = 3,
			/** Represents the resource is in the waiting queue to be loaded, 
			 *  when a resource is pending, no more load or unload operation is processed
			 *  unless future updates are made for the resource managements system.
			 */
			RS_PendingLoad = 4,
			/** Represents the resource is in the waiting queue to be unloaded. 
			 *  This is just opposed to RS_PendingLoad.
			 */
			RS_PendingUnload = 5,

		};
		
		template class APAPI fastdelegate::FastDelegate1<Resource*, void>;
		typedef fastdelegate::FastDelegate1<Resource*, void> ResourceEventHandler;

		/** A resource is data or operation that uses hardware provided resources, which is limited.
		 *  Typical example for resources are textures and models. Both uses video card memory.
		 *  
		 *  A resource controlled by a resource manager is a managed resource. See resource manager for details.
		 *  A resource can be a unmanaged one. In this case, the loading/unloading is all up to the client code.
		 *  And no other management or background work is done for it.
		 *
		 *  Resources are identified by a 'hashString' which uniquely represents each of them.
		 */
		class APAPI Resource
		{
		
			
		public: 
			friend class GenerationTable;
			typedef Resource ResHandleTemplateConstraint;   
			
			virtual ~Resource();

			ResourceEventHandler& eventLoaded() { return m_eventLoaded; }
			ResourceEventHandler& eventUnloaded() { return m_eventUnloaded; }

			/** 
			 *  Get the generation number.
			 *   Only returns valid if the resource is managed and async.
			 */
			int GetGeneration() const 
			{
				if (m_generation)
					return m_generation->Generation;
				return -1;
			}
			int getReferenceCount() const { return m_refCount; }

			/** [ASync resource only]
			 *  Tells if the resource can be unloaded when inactive.
			 */
			virtual bool IsUnloadable() const { return true; }

			virtual bool IsIndependent() const { return true; }

			/** Get the memory token by the resource in bytes.
			*/
			virtual uint32 getSize() = 0;

			/** [Managed resource only]
			 *  Touches the resource. 
			 *  If the resource is not loaded yet, this will cause the resource to load.
			 *  This is not necessary called since the resource handle will do the work unless
			 *  special requirements.
			 */
			void Use();
			void UseSync();

			/** Load the resource
			*/
			void Load();
			/** Unload the resource
			*/
			void Unload();

			/** Gets a string uniquely represents the resource
			*/
			const String& getHashString() const { return m_hashString; }
			/** Check if the resource's state is RS_Loaded.
			*/
			bool isLoaded() { return getState() == RS_Loaded; }
			/** Gets the resource's current state.
			*/
			ResourceState getState()
			{
				ResourceState state;
				m_lock.lock();
				state = m_state;
				m_lock.unlock();
				return state;
			}
			


			/** Check if the resource is managed.
			*/
			bool isManaged() const { return !!m_manager; }
			/** Gets the corresponding resource manager. 
			 *  &return The pointer to the resource manager object, 0 if the resource is not managed.
			 */
			ResourceManager* getManager() const { return m_manager; }
			


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
		protected:
			/** implement load processing here
			*/
			virtual void load() = 0;
			/** implement unload processing here
			*/
			virtual void unload() = 0;

			/** Create a unmanaged resource
			*/
			Resource();
			/** If manager is not 0, creates a managed resource.
			*/
			Resource(ResourceManager* manager, const String& hashString);

		private:
			/** Implements a general resource loading operation
			*/
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
					

					if (state != RS_PendingLoad)
						return;
					
					res->setState(RS_Loading);
					res->load();
					res->setState(RS_Loaded);

					res->OnLoaded();
				}

				virtual OperationType getType() const { return RESOP_Load; }
			};
			/** Implements a general resource unloading operation
			*/
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
					if (state != RS_PendingUnload)
						return;
					res->setState(RS_Unloading);
					res->unload();
					res->setState(RS_Unloaded);

					res->OnUnloaded();
				}

				virtual OperationType getType() const { return RESOP_Unload; }
			};
			/** A utility used for calculating generation number for resources based on
			 * the visit history.
			 */
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
				void UpdateGeneration(float time);

				bool IsGenerationOutOfTime(float time);

			};

			friend class ResourceHandle<Resource>;

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


			void setState(ResourceState st)
			{
				m_lock.lock();
				m_state = st;
				m_lock.unlock();
			}

		};
	};
};
#endif