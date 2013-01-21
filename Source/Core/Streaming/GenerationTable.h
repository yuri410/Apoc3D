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

#ifndef APOC3D_GENERATIONTABLE_H
#define APOC3D_GENERATIONTABLE_H

#include "Common.h"
#include "Collections/FastList.h"
#include "Collections/ExistTable.h"
#include "tthread/fast_mutex.h"
#include "tthread/tinythread.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace tthread;

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			/**
			 *  A GenerationTable puts resources with different gen numbers into different tables,
			 *  and works on resource collecting and gen number updating based on the them.
			 */
			class APAPI GenerationTable
			{
			public:
				static const int32 MaxGeneration = 4;

				static const float GenerationLifeTime[];

			public:
				fast_mutex m_genLock;

				GenerationTable(ResourceManager* mgr);
				~GenerationTable();


				void SubTask_GenUpdate();
				void SubTask_Collect();


				//ExistTable<Resource*>& getGeneration(int index) const { return m_generations[index]; }

				void AddResource(Resource* res);
				void RemoveResource(Resource* res);

				/**
				 *  Remove the resource from the table with oldGeneration number in m_generations, and add to the new table of newGeneration.
				 */
				void UpdateGeneration(int oldGeneration, int newGeneration, Resource* resource);

				void ShutDown() 
				{
					m_isShutdown = true;

					//m_thread->join();
				}

			private:
				/**
				 *  An array of tables represents that resources with gen i are in i+1 th table.
				 */
				ExistTable<Resource*>* m_generations;
				/**
				 *  A list of all pointers to resource objects.
				 */
				FastList<Resource*> m_generationList;

				//thread* m_thread;

				ResourceManager* m_manager;

				bool m_isShutdown;


				static void ThreadEntry(void* args)
				{
					GenerationTable* table = static_cast<GenerationTable*>(args);
					table->GenerationUpdate_Main();
				}

				void GenerationUpdate_Main();
				bool CanUnload(Resource* res) const;
			};
		}
		
	}
}

#endif