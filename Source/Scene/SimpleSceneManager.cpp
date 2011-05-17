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

#include "SimpleSceneManager.h"

#include "SceneObject.h"

#include "..\Graphics\RenderOperation.h"
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		SimpleSceneManager::SimpleSceneManager(void)
		{
			m_defaultNode = new SimpleSceneNode();
		}


		SimpleSceneManager::~SimpleSceneManager(void)
		{
		}

		void SimpleSceneManager::AddObject(SceneObject* const sceObj)
		{
			SceneManager::AddObject(sceObj);
			m_defaultNode->AddObject(sceObj);
		}

		bool SimpleSceneManager::RemoveObject(SceneObject* const sceObj)
		{			
			m_defaultNode->RemoveObject(sceObj);
			return SceneManager::RemoveObject(sceObj);
		}

		void SimpleSceneManager::PrepareVisibleObjects(Camera* camera, BatchData* batchData)
		{
			batchData->Clear();
			for (int i =0; i<m_defaultNode->getCount();i++)
			{
				SceneObject* obj = m_defaultNode->operator[](i);
				if (obj->hasSubObjects())
				{
					obj->PrepareVisibleObjects(camera, 0, batchData);
				}

				batchData->AddVisisbleObject(obj, 0);
			}
		}
	}
}