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
#ifndef RESOUECEMANAGER_H
#define RESOURCEMANAGER_H

#include "Common.h"

using namespace Apoc3D::Core::Streaming;

namespace Apoc3D
{
	namespace Core
	{
		template class APAPI std::unordered_map<String, Resource*>;
		typedef std::unordered_map<String, Resource*> ResHashTable;

		class APAPI ResourceManager
		{
		private:
			ResHashTable m_hashTable;

			int64 m_totalCacheSize;
			int64 m_curUsedCache;


			GenerationTable* m_generationTable;
			AsyncProcessor* m_asyncProc;

			bool m_isShutDown;

			void Resource_Loaded(Resource* res);
			void Resource_Unloaded(Resource* res);

		protected:
		public:
			
			/** Notifies the resource manager a new resource is created, and should be managed.
			*/
			void NotifyNewResource(Resource* res);
			/** Notifies the resource manager a resource is release, and should be removed from management.
			*/
			void NotifyReleaseResource(Resource* res);

			GenerationTable* getTable() const { return m_generationTable; }

			int64 getTotalCacheSize() const { return m_totalCacheSize; }
			void setTotalCacheSize(int64 size) { m_totalCacheSize = size; }

			int64 getUsedCacheSize() const { return m_curUsedCache; }
			

			ResourceManager(int64 cacheSize, bool useAsync = true);
			~ResourceManager();

			void Shutdown();

			void AddTask(ResourceOperation* op) const;


			bool IsIdle() const;
			void WaitForIdle() const;
			int GetCurrentOperationCount() const;

			Resource* Exists(const String& hashString);

			//SINGLETON_DECL_HEARDER(ResourceManager);

		};
	}
}

#endif