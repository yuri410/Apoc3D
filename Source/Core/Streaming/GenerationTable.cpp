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

#include "GenerationTable.h"
#include "Platform/Thread.h"
#include "Core/ResourceManager.h"
#include "Core/Resource.h"

using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			const float GenerationTable::GenerationLifeTime[MaxGeneration] = { 3, 6, 10, 30 };

			GenerationTable::GenerationTable(ResourceManager* mgr)
				: m_manager(mgr), m_isShutdown(false), m_generationList(100)
			{
				m_thread = new thread(&GenerationTable::ThreadEntry, this);
			}


			void GenerationTable::SubTask_GenUpdate()
			{

			}
			void GenerationTable::SubTask_Manage()
			{
				int predictSize = m_manager->getUsedCacheSize();

				if (predictSize>m_manager->getTotalCacheSize())
				{
					for (int i=3;i>1 && predictSize > m_manager->getTotalCacheSize(); i--)
					{
						ExistTable<Resource*>::Enumerator iter = m_generations->GetEnumerator();

						while (iter.MoveNext())
						{
							Resource* r = *iter.getCurrent();

							if (r->getState() == RS_Loaded && r->IsUnloadable())
							{
								predictSize -= r->getSize();
								r->Unload();

								if (predictSize <= m_manager->getTotalCacheSize())
								{
									break;
								}
							}
						}
					}
				}
			}

			void GenerationTable::GenerationUpdate_Main()
			{
				static const int ManageInterval = 10;

				int times = 0;
				while (!m_isShutdown)
				{
					SubTask_GenUpdate();
					if (times++ & ManageInterval == 0)
					{
						SubTask_Manage();
					}
					ApocSleep(100);
				}
			}
		}
	}
}