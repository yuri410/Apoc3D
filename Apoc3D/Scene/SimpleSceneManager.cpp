/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "apoc3d/Graphics/RenderOperation.h"
#include "apoc3d/Scene/SceneRenderer.h"
#include "apoc3d/Math/Ray.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{

		void SimpleSceneNode::AddObject(SceneObject* sceObj)
		{
			m_attached.Add(sceObj);
			sceObj->NotifyParentNode( this );
			
		}

		void SimpleSceneNode::RemoveObject(SceneObject* const obj)
		{
			m_attached.Remove(obj);
			obj->NotifyParentNode(0);
		}

		SimpleSceneManager::SimpleSceneManager(void)
		{
			m_defaultNode = new SimpleSceneNode();

		}


		SimpleSceneManager::~SimpleSceneManager(void)
		{
			delete m_defaultNode;
		}

		void SimpleSceneManager::AddObject(SceneObject* const sceObj)
		{
			SceneManager::AddObject(sceObj);
			m_defaultNode->AddObject(sceObj);
			sceObj->OnAddedToScene(this);
		}

		bool SimpleSceneManager::RemoveObject(SceneObject* const sceObj)
		{			
			m_defaultNode->RemoveObject(sceObj);
			sceObj->OnRemovedFromScene(this);
			return SceneManager::RemoveObject(sceObj);
		}

		void SimpleSceneManager::PrepareVisibleObjects(Camera* camera, BatchData* batchData)
		{
			//batchData->Clear();
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
		SceneObject* SimpleSceneManager::FindObject(const Ray& ray, IObjectFilter* filter)
		{
			SceneObject* result = 0;
			float nearest = FLT_MAX;
			for (int i =0; i<m_defaultNode->getCount();i++)
			{
				SceneObject* obj = m_defaultNode->operator[](i);
				
				if ((filter && filter->Check(obj) || !filter) &&
					obj->IntersectsSelectionRay(ray))
				{
					float dist = Vector3Utils::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
					if (dist<nearest)
					{
						nearest = dist;
						result = obj;
					}
				}
			}
			return result;
		}
	}
}