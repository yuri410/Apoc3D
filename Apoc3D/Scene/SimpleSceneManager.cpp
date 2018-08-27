/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

		SimpleSceneManager::SimpleSceneManager()
		{
			m_defaultNode = new SimpleSceneNode();

		}


		SimpleSceneManager::~SimpleSceneManager()
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
					float dist = Vector3::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
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