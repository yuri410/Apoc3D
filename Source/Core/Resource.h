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
#include "Streaming\AsyncProcessor.h"

using namespace Apoc3D::Core::Streaming;

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
		
		template<class T>
		class APAPI ResourceHandle
		{
		private:
			T* m_resource;

			bool m_isDummy;
		public:
			ResourceHandle(T* resource)
				: m_resource(resource)
			{
			}


		};
		///* Represent the implementation for resource loading/unloading */
		//class APAPI ResourceProcessor
		//{
		//public:
		//	virtual void Process(Resource* res) const = 0;
		//};

		///* Implements a resource loading algorithm */
		//class APAPI ResourceLoader : ResourceProcessor
		//{
		//public:
		//	virtual void Process(Resource* res) const = 0;
		//};

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
			
			int m_refCount;

			//ResourceLoader* m_resLoader;
			uint32 m_state;

			const String m_hashString;

			ResourceLoadOperation* m_resLoader;
			ResourceUnloadOperation* m_resUnloader;

			ResourceManager* m_manager;

			//ResourceEventHandler me_Loaded;
			//ResourceEventHandler me_Unloaded;
		protected:
			virtual void load() = 0;
			virtual void unload() = 0;

		public: 
			typedef Resource ResTempHelper;   

			//ResourceEventHandler* eventLoaded();
			//ResourceEventHandler* eventUnloaded();

			Resource() 
				: m_refCount(0), m_manager(0), m_resLoader(0), m_resUnloader(0), m_state(RS_Unloaded)
			{
			}
			Resource(ResourceManager* manager, const String& hashString);
			//Resource(ResourceManager* manager, const String& hashString, ResourceLoader* loader);

			virtual ~Resource();

			virtual uint32 getSize() = 0;

			void Touch();
			void Load();
			void Unload();

			const String& getHashString() const { return m_hashString; }
			uint32 getState() const { return m_state; }
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