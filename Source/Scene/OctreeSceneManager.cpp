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

#include "OctreeSceneManager.h"

#include "SceneObject.h"
#include "Graphics\RenderOperationBuffer.h"
#include "Graphics\RenderOperation.h"
#include "Graphics\Material.h"
#include "Graphics\GeometryData.h"
#include "Core\GameTime.h"
#include "Graphics\Camera.h"
#include "Math\Frustum.h"
#include "Math\Ray.h"
#include "SceneRenderer.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		Vector3 OctreeSceneNode::OffsetVectorTable[8] = 
		{ 
			Vector3Utils::LDVector(-1,-1,-1), Vector3Utils::LDVector(-1,-1,1), 
			Vector3Utils::LDVector(-1,1,-1),  Vector3Utils::LDVector(-1,1,1), 
			Vector3Utils::LDVector(1,-1,-1),  Vector3Utils::LDVector(1,-1,1),
			Vector3Utils::LDVector(1,1,-1),   Vector3Utils::LDVector(1,1,1)
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

					Vector3 offset = Vector3Utils::Multiply(OffsetVectorTable[ext], ofLen);
					box.Center = Vector3Utils::Add(m_boundingVolume.Center, offset);

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
					BoundingSphere::Intersects(m_childTable[i]->m_boundingSphere, obj->getBoundingSphere()))
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
			int i = (_V3X(pos) - _V3X(m_boundingVolume.Center)) > 0 ? 1 : 0;
			int j = (_V3Y(pos) - _V3Y(m_boundingVolume.Center)) > 0 ? 1 : 0;
			int k = (_V3Z(pos) - _V3Z(m_boundingVolume.Center)) > 0 ? 1 : 0;
			return static_cast<Extend>( (i<<2) | (j<<1) | k );
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		static const int WorldLodLevelCount = 4;
		static const float LodThresold[WorldLodLevelCount] = { 1.4f, 2.5, 4.0f, 5.5f };
		int GetLevel(const BoundingSphere& sphere, const Vector3& pos)
		{
			float dist = Vector3Utils::Distance(sphere.Center, pos);

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


		OctreeSceneManager::~OctreeSceneManager(void)
		{
			delete m_octRootNode;	
		}

		void OctreeSceneManager::AddObject(SceneObject* const sceObj)
		{
			SceneManager::AddObject(sceObj);

			if (sceObj->IsDynamicObject())
			{
				m_dynObjs.push_back(sceObj);
			}
			else
			{
				AddStaticObject(sceObj);
			}

			sceObj->OnAddedToScene(this);
		}
		void OctreeSceneManager::AddStaticObject(SceneObject* sceObj)
		{
			list<SceneObject*>::iterator iter = find(m_farObjs.begin(), m_farObjs.end(), sceObj);

			if (!QualifiesFarObject(sceObj))
			{
				m_octRootNode->AddObject(sceObj);
				if (iter != m_farObjs.end())
				{
					m_farObjs.erase(iter);
				}
			}
			else
			{
				if (iter == m_farObjs.end())
				{
					m_farObjs.push_back(sceObj);
				}
			}
		}
		bool OctreeSceneManager::RemoveObject(SceneObject* const sceObj)
		{
			
			if (sceObj->IsDynamicObject())
			{
				m_dynObjs.remove(sceObj);
			}
			else
			{
				m_octRootNode->RemoveObject(sceObj);
				
				m_farObjs.remove(sceObj);
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

			return (_V3X(pos) < _V3X(m_min) || _V3X(pos) > _V3X(m_max) ||
					_V3Z(pos) < _V3Z(m_min) || _V3Z(pos) > _V3Z(m_max) ||
					_V3Y(pos) < _V3Y(m_min) || _V3Y(pos) > _V3Y(m_max));
		}
		void OctreeSceneManager::PrepareVisibleObjects(Camera* camera, BatchData* batchData)
		{
			const Frustum& frus = camera->getFrustum();

			m_bfsQueue.Enqueue(m_octRootNode);

			Vector3 camPos = camera->getInvViewMatrix().GetTranslation();

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
					const ObjectList& objs = node->getAttachedObjects();
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
			for (list<SceneObject*>::iterator iter = m_farObjs.begin();iter!=m_farObjs.end();iter++)
			{
				SceneObject* obj = *iter;
				if (frus.Intersects(obj->getBoundingSphere()))
				{
					int level = GetLevel(obj->getBoundingSphere(), camPos);

					batchData->AddVisisbleObject(obj, level);
				}
			}
			for (list<SceneObject*>::iterator iter = m_dynObjs.begin();iter!=m_dynObjs.end();iter++)
			{
				SceneObject* obj = *iter;
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
			float nearest = Math::MaxFloat;
			assert(m_bfsQueue.getCount()==0);

			m_bfsQueue.Enqueue(m_octRootNode);
			while (m_bfsQueue.getCount())
			{
				OctreeSceneNode* node = m_bfsQueue.Dequeue();
				
				float d;
				if (BoundingSphere::Intersects(node->getBoundingSphere(), ray, d))
				{
					for (int i=0;i<OctreeSceneNode::OCTE_Count;i++)
					{
						if (node->getNode(static_cast<OctreeSceneNode::Extend>(i)))
						{
							m_bfsQueue.Enqueue(node->getNode(static_cast<OctreeSceneNode::Extend>(i)));
						}
					}
					const ObjectList& objs = node->getAttachedObjects();
					for (int i=0;i<objs.getCount();i++)
					{
						SceneObject* obj = objs[i];
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
				}
			}

			for (list<SceneObject*>::iterator iter = m_farObjs.begin();iter!=m_farObjs.end();iter++)
			{
				SceneObject* obj = *iter;
				if ((filter && filter->Check(obj) || !filter) && 
					obj->IntersectsSelectionRay(ray))
				{
					float dist = Vector3Utils::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
					if (dist < nearest)
					{
						nearest = dist;
						result = obj;
					}
				}
			}
			for (list<SceneObject*>::iterator iter = m_dynObjs.begin();iter!=m_dynObjs.end();iter++)
			{
				SceneObject* obj = *iter;
				if ((filter && filter->Check(obj) || !filter) && 
					obj->IntersectsSelectionRay(ray))
				{
					float dist = Vector3Utils::DistanceSquared(obj->getBoundingSphere().Center, ray.Position);
					if (dist < nearest)
					{
						nearest = dist;
						result = obj;
					}
				}
			}
			return result;
		}

		void OctreeSceneManager::Update(const GameTime* const &time)
		{
			const FastList<SceneObject*>& objects = getAllObjects();
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