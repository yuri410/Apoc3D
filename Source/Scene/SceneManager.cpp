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

#include "SceneObject.h"
#include "..\Graphics\RenderOperationBuffer.h"
#include "..\Graphics\RenderOperation.h"
#include "..\Graphics\Material.h"
#include "..\Graphics\GeometryData.h"
#include "..\GameTime.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		void BatchData::AddVisisbleObject(SceneObject* obj, int level)
		{
			m_objectCount++;
			RenderOperationBuffer* buffer = obj->GetRenderOperation(level);

			if (buffer)
			{
				for (int k=0;k<buffer->getCount();k++)
				{
					RenderOperation op = buffer->get(k);

					Material* mtrl = op.getMaterial();
					GeometryData* geoData = op.getGeomentryData();

					if (mtrl)
					{
						BatchHandle mtrlHandle = mtrl->getBatchHandle();
						m_mtrlList[mtrlHandle] = mtrl;
						m_priTable[mtrl->getPriority()]->
							operator[](mtrlHandle)->
							operator[](geoData->getBatchHandle())->push_back(op);

					}
				}
			}
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

		void SceneManager::AddObject(SceneObject* const obj)
		{
			m_objects.push_back(obj);
		} 
		void SceneManager::RemoveObject(SceneObject* const obj)
		{
			ObjectList::const_iterator pos = std::find(m_objects.begin(), m_objects.end(), obj);

			if (pos != m_objects.end())
			{
				m_objects.erase(pos);
				return true;
			}
			return false;
		}

		void SceneManager::Update(const GameTime* const &time)
		{
			for (uint32 i = 0;i<m_objects.size();i++)
			{
				m_objects[i]->Update(time);
			}
		}
	};
};