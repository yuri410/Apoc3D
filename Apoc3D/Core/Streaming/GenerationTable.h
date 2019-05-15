#pragma once

#ifndef APOC3D_GENERATIONTABLE_H
#define APOC3D_GENERATIONTABLE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

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
				HashSet<Resource*>* m_generations;
				/**
				 *  A list of all pointers to resource objects.
				 */
				List<Resource*> m_generationList;
				
				ResourceManager* m_manager;

				bool m_isShutdown;
				std::mutex m_genLock;

				bool CanUnload(Resource* res) const;
			};
		}
		
	}
}

#endif