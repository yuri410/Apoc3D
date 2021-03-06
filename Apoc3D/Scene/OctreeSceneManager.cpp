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

#include "OctreeSceneManager.h"

#include "SceneObject.h"
#include "apoc3d/Graphics/RenderOperationBuffer.h"
#include "apoc3d/Graphics/RenderOperation.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Graphics/Camera.h"
#include "apoc3d/Math/Frustum.h"
#include "apoc3d/Math/Ray.h"
#include "SceneRenderer.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		Vector3 OctreeSceneNode::OffsetVectorTable[8] = 
		{ 
			Vector3(-1,-1,-1), Vector3(-1,-1,1), 
			Vector3(-1,1,-1),  Vector3(-1,1,1), 
			Vector3(1,-1,-1),  Vector3(1,-1,1),
			Vector3(1,1,-1),   Vector3(1,1,1)
		};

		OctreeSceneNode::OctreeSceneNode(OctreeSceneManager* mgr, OctreeSceneNode* parent, const OctreeBox& volume)
			: m_manager(mgr), m_parent(parent), m_boundingVolume(volume)
		{
			memset(m_childTable, 0, sizeof(m_childTable));
			volume.GetBoundingSphere(m_boundingSphere);

		}
		OctreeSceneNode::~OctreeSceneNode()
		{
			for (int i=0;i<OCTE_Count;i++)
			{
				if (m_childTable[i])
				{
					delete m_childTable[i];
				}
			}
		}

		void OctreeSceneNode::AddObject(SceneObject* obj)
		{
			// check the obj's bv size, and decide if this is the level to add it.
			// if obj's size is small enough, put it in to child nodes
			const float ofLen = m_boundingVolume.Length / 4.0f;

			if (ofLen > m_manager->getMinimumBVSize() && obj->getBoundingSphere().Radius <= (1.0 / 4.0f) * m_boundingVolume.Length)
			{
				// if the child node is more suitable, add obj to the child node 
				
				Extend ext = GetExtend(obj->getBoundingSphere().Center);

				if (!m_childTable[ext])
				{
					OctreeBox box;
					box.Length = m_boundingVolume.Length*0.5f;

					Vector3 offset = OffsetVectorTable[ext] * ofLen;
					box.Center = m_boundingVolume.Center + offset;

					m_childTable[ext] = new OctreeSceneNode(m_manager,this, box);
				}

				m_childTable[ext]->AddObject(obj);
				// change the extend if necessary

			}
			else
			{
				SceneNode::AddObject(obj);
			}
		}

		void OctreeSceneNode::RemoveObject(SceneObject* const obj)
		{
			if (m_parent) 
			{
				OctreeSceneNode* node = (OctreeSceneNode*)obj->getSceneNode();

				node->RemoveObjectInternal(obj);
			}
			else
			{
				RemoveObjectInternal(obj);
			}
		}
		bool OctreeSceneNode::RemoveObjectInternal(SceneObject* obj)
		{
			for (int i=0;i<OCTE_Count;i++)
			{
				if (m_childTable[i] &&
					m_childTable[i]->m_boundingSphere.Intersects(obj->getBoundingSphere()))
				{
					if (m_childTable[i]->RemoveObjectInternal(obj))
					{
						return true;
					}
				}
			}
			return RemoveAttachedObject(obj);
		}
		bool OctreeSceneNode::RemoveAttachedObject(SceneObject* obj)
		{
			return m_attached.Remove(obj);
		}


		OctreeSceneNode::Extend OctreeSceneNode::GetExtend(const Vector3& pos) const
		{
			int i = (pos.X - m_boundingVolume.Center.X) > 0 ? 1 : 0;
			int j = (pos.Y - m_boundingVolume.Center.Y) > 0 ? 1 : 0;
			int k = (pos.Z - m_boundingVolume.Center.Z) > 0 ? 1 : 0;
			return static_cast<Extend>( (i<<2) | (j<<1) | k );
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		const int WorldLodLevelCount = 4;
		const float LodThresold[WorldLodLevelCount] = { 1.4f, 2.5, 4.0f, 5.5f };
		int GetLevel(const BoundingSphere& sphere, const Vector3& pos)
		{
			float dist = Vector3::Distance(sphere.Center, pos);

			for (int i = 0; i < WorldLodLevelCount; i++)
			{
				if (dist <= sphere.Radius * LodThresold[i])
				{
					return i;
				}
			}
			return WorldLodLevelCount;
		}

		OctreeSceneManager::OctreeSceneManager(const OctreeBox& range, float minBVSize)
			: m_minimumBVSize(minBVSize), m_range(range)
		{
			m_octRootNode = new OctreeSceneNode(this, 0, range);
			m_min = m_octRootNode->GetMin();
			m_max = m_octRootNode->GetMax();
		}


		OctreeSceneManager::~OctreeSceneManager()
		{
			delete m_octRootNode;	
		}

		void OctreeSceneManager::AddObject(SceneObject* const sceObj)
		{
			SceneManager::AddObject(sceObj);

			if (sceObj->IsDynamicObject())
			{
				m_dynObjs.PushBack(sceObj);
			}
			else
			{
				AddStaticObject(sceObj);
			}

			sceObj->OnAddedToScene(this);
		}
		void OctreeSceneManager::AddStaticObject(SceneObject* sceObj)
		{
			LinkedList<SceneObject*>::Iterator iter = m_farObjs.Find(sceObj);

			if (!QualifiesFarObject(sceObj))
			{
				m_octRootNode->AddObject(sceObj);
				if (iter != m_farObjs.end())
				{
					m_farObjs.RemoveAt(iter);
				}
			}
			else
			{
				if (iter == m_farObjs.end())
				{
					m_farObjs.PushBack(sceObj);
				}
			}
		}
		bool OctreeSceneManager::RemoveObject(SceneObject* const sceObj)
		{
			
			if (sceObj->IsDynamicObject())
			{
				m_dynObjs.Remove(sceObj);
			}
			else
			{
				m_octRootNode->RemoveObject(sceObj);
				
				m_farObjs.Remove(sceObj);
				//list<SceneObject*>::iterator iter = find(m_farObjs.begin(), m_farObjs.end(), sceObj);
				//if (iter != m_farObjs.end())
				//{
				//	m_farObjs.erase(iter);
				//}
			}

			sceObj->OnRemovedFromScene(this);

			return SceneManager::RemoveObject(sceObj);
		} 
		bool OctreeSceneManager::QualifiesFarObject(const SceneObject* obj) const
		{
			Vector3 pos = obj->getBoundingSphere().Center;

			return (pos.X < m_min.X || pos.X > m_max.X ||
					pos.Y < m_min.Y || pos.Y > m_max.Y ||
					pos.Z < m_min.Z || pos.Z > m_max.Z);
		}
		void OctreeSceneManager::PrepareVisibleObjects(Camera* camera, BatchData* batchData)
		{
			const Frustum& frus = camera->getFrustum();

			m_bfsQueue.Enqueue(m_octRootNode);

			Vector3 camPos = camera->getInvViewMatrix().GetTranslation();

			// do board first pass a the octree
			while (m_bfsQueue.getCount())
			{
				OctreeSceneNode* node = m_bfsQueue.Dequeue();
				//Vector3 center = node->getBoundingSphere().Center;
				
				if (frus.Intersects(node->getBoundingSphere()))
				{
					for (int i=0;i<OctreeSceneNode::OCTE_Count;i++)
					{
						OctreeSceneNode* subNode = node->getNode(static_cast<OctreeSceneNode::Extend>(i));
						if (subNode)
						{
							m_bfsQueue.Enqueue(subNode);
						}
					}
					const SceneObjectList& objs = node->getAttachedObjects();
					for (int i=0;i<objs.getCount();i++)
					{
						if (frus.Intersects(objs[i]->getBoundingSphere()))
						{
							if (objs[i]->hasSubObjects())
							{
								objs[i]->PrepareVisibleObjects(camera, 0, batchData);
							} 
							int level = GetLevel(objs[i]->getBoundingSphere(), camPos);

							batchData->AddVisisbleObject(objs[i], level);
						}
						
					}
				}
			}
			for (SceneObject* obj : m_farObjs)
			{
				if (frus.Intersects(obj->getBoundingSphere()))
				{
					int level = GetLevel(obj->getBoundingSphere(), camPos);

					batchData->AddVisisbleObject(obj, level);
				}
			}
			for (SceneObject* obj : m_dynObjs)
			{
				if (frus.Intersects(obj->getBoundingSphere()))
				{
					int level = GetLevel(obj->getBoundingSphere(), camPos);

					batchData->AddVisisbleObject(obj, level);
				}
			}

		}

		SceneObject* OctreeSceneManager::FindObject(const Ray& ray, IObjectFilter* filter)
		{
			SceneObject* result = 0;
			float nearest = FLT_MAX;
			assert(m_bfsQueue.getCount()==0);

			m_bfsQueue.Enqueue(m_octRootNode);
			while (m_bfsQueue.getCount())
			{
				OctreeSceneNode* node = m_bfsQueue.Dequeue();
				
				if (node->getBoundingSphere().IntersectsRay(ray, nullptr))
				{
					for (int i=0;i<OctreeSceneNode::OCTE_Count;i++)
					{
						if (node->getNode(static_cast<OctreeSceneNode::Extend>(i)))
						{
							m_bfsQueue.Enqueue(node->getNode(static_cast<OctreeSceneNode::Extend>(i)));
						}
					}
					const SceneObjectList& objs = node->getAttachedObjects();
					for (int i=0;i<objs.getCount();i++)
					{
						SceneObject* obj = objs[i];
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
				}
			}

			for (SceneObject* obj : m_farObjs)
			{
				if ((filter && filter->Check(obj) || !filter) && 
					obj->IntersectsSelectionRay(ray))
				{
					float dist = Vector3::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
					if (dist < nearest)
					{
						nearest = dist;
						result = obj;
					}
				}
			}
			for (SceneObject* obj : m_dynObjs)
			{
				if ((filter && filter->Check(obj) || !filter) && 
					obj->IntersectsSelectionRay(ray))
				{
					float dist = Vector3::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
					if (dist < nearest)
					{
						nearest = dist;
						result = obj;
					}
				}
			}
			return result;
		}

		void OctreeSceneManager::Update(const AppTime* time)
		{
			const List<SceneObject*>& objects = getAllObjects();
			for (int32 i = 0;i<objects.getCount();i++)
			{
				objects[i]->Update(time);

				if (objects[i]->IsDynamicObject() && objects[i]->RequiresNodeUpdate)
				{
					m_octRootNode->RemoveObject(objects[i]);
					AddStaticObject(objects[i]);
					objects[i]->RequiresNodeUpdate = false;
				}
			}
		}
	};
};