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

#pragma once

#include "..\Common.h"
#include "Streaming\AsyncProcessor.h"

using namespace Apoc3D::Core::Streaming;

namespace Apoc3D
{
	namespace Core
	{
		enum _Export ResourceState
		{
			RS_Unloaded = 0,
			RS_Loaded = 1,
			RS_Loading = 2,			
			RS_Unloading = 3,
			RS_Pending = 4
		};

		/* Represent the implementation for resource loading/unloading */
		class _Export ResourceProcessor
		{
		public:
			virtual void Process(Resource* res) const = 0;
		};
		/* Implements a resource loading algorithm */
		class _Export ResourceLoader : ResourceProcessor
		{
		public:
			virtual void Process(Resource* res) const = 0;
		};

		class _Export Resource
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

			ResourceLoader* m_resLoader;
			uint32 m_state;

			const String m_hashString;

			ResourceLoadOperation* m_loadOp;
			ResourceUnloadOperation* m_unloadOp;

			ResourceManager* m_manager;


			ResourceEventHandler me_Loaded;
			ResourceEventHandler me_Unloaded;
		protected:
			virtual void load();
			virtual void unload() = 0;

		public: 
			typedef Resource ResTempHelper;   

			ResourceEventHandler* eventLoaded();
			ResourceEventHandler* eventUnloaded();

			Resource();
			Resource(ResourceManager* manager, const String& hashString);
			Resource(ResourceManager* manager, const String& hashString, ResourceLoader* loader);


			virtual uint32 getSize() = 0;

			void Use();
			
			void Load();
			void Unload();

			const String& getHashString() const { return m_hashString; }
			uint32 getState() const { return m_state; }
			bool getIsManaged() const { return m_manager; }

			void _Ref() { assert(!getIsManaged()); m_refCount++; }
			void _Unref() { assert(!getIsManaged()); m_refCount--; }
		};
	};
};
#endif