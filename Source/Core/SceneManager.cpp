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

#include "SceneManager.h"


namespace Apoc3D
{
	namespace Core
	{
		void BatchData::Add(const RenderOperationBuffer* op)
		{
			m_objectCount++;

		}
		void BatchData::Clear()
		{
			for (PriorityTable::iterator i = m_priTable.begin();i!=m_priTable.end();++i)
			{
				MaterialTable* mtrlTbl = i->second;
				for (MaterialTable::iterator j = mtrlTbl->begin(); j!=mtrlTbl->end();j++)
				{
					//Material* mtrl = m_mtrlList[j->first];
					GeometryTable* geoTbl = j->second;

					geoTbl->clear();
				}
				mtrlTbl->clear();
			}
			m_priTable.clear();
		}

		SceneManager::SceneManager(void)
		{
		}


		SceneManager::~SceneManager(void)
		{
		}

		void SceneManager::AddObject(const SceneObject* obj)
		{
			m_objects.push_back(obj);
		} 
		bool SceneManager::RemoveObject(const SceneObject* obj)
		{
			ObjectList::const_iterator pos = std::find(m_objects.begin(), m_objects.end(), obj);

			if (pos != m_objects.end())
			{
				m_objects.erase(pos);
				return true;
			}
			return false;
		}
	};
};