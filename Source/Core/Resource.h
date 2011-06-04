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
			RS_Pending = 4
		};
		

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
					if (res->m_state != RS_Pending)
						return;
					res->m_state = RS_Loading;
					res->load();
					res->m_state = RS_Loaded;
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
					if (res->m_state != RS_Pending)
						return;
					res->m_state = RS_Unloading;
					res->unload();
					res->m_state = RS_Unloaded;
				}
			};
		
			class GenerationCalculator
			{
			public:
				volatile int Generation;
			private:
				GenerationTable* m_table;
				volatile FastQueue<float> m_timeQueue;
			public:
				GenerationCalculator(GenerationTable* table);

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

		protected:
			virtual void load() = 0;
			virtual void unload() = 0;

			Resource() 
				: m_refCount(0), m_manager(0), m_resLoader(0), m_resUnloader(0), m_state(RS_Unloaded)
			{
			}
			Resource(ResourceManager* manager, const String& hashString);

			void LoadSync();
		public: 
			typedef Resource ResHandleTemplateConstraint;   

			int getReferenceCount() const { return m_refCount; }

			//Resource(ResourceManager* manager, const String& hashString, ResourceLoader* loader);

			virtual ~Resource();

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
					if (--m_refCount == 0)
					{
						//delete this;
					}
				}
			}
		};
	};
};
#endif