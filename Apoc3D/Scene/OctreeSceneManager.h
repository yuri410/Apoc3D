#pragma once
#ifndef APOC3D_OCTREESCENEMANAGER_H
#define APOC3D_OCTREESCENEMANAGER_H

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

#include "SceneManager.h"
#include "SceneNode.h"

#include "apoc3d/Collections/LinkedList.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Math/OctreeBox.h"
#include "apoc3d/Math/BoundingSphere.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		class APAPI OctreeSceneNode : public SceneNode
		{
		public:
			/*
			000 = OCTE_NxNyNz
			001 = OCTE_NxNyPz,
			010 = OCTE_NzPyNz,
			011 = OCTE_NxPyPz,
			100 = OCTE_PxNyNz,
			101 = OCTE_PxNyPz,
			110 = OCTE_PxPyNz,
			111 = OCTE_PxPyPz,
			*/
			enum Extend
			{
				OCTE_NxNyNz=0,
				OCTE_NxNyPz,
				OCTE_NzPyNz,
				OCTE_NxPyPz,
				OCTE_PxNyNz,
				OCTE_PxNyPz,
				OCTE_PxPyNz,
				OCTE_PxPyPz,
				OCTE_Count = 8
			};



			OctreeSceneNode(OctreeSceneManager* mgr, OctreeSceneNode* parent, const OctreeBox& volume);
			~OctreeSceneNode();

			virtual void AddObject(SceneObject* sceObj);
			virtual void RemoveObject(SceneObject* sceObj);

			

			Vector3 GetMax() const
			{
				Vector3 ext = Vector3::Set(m_boundingVolume.Length*0.5f);
				return m_boundingVolume.Center +  ext;
			}
			Vector3 GetMin() const
			{
				Vector3 ext = Vector3::Set(m_boundingVolume.Length*-0.5f);
				return m_boundingVolume.Center + ext;
			}

			const BoundingSphere& getBoundingSphere() const { return m_boundingSphere; }
			OctreeSceneNode* getNode(Extend e) const { return m_childTable[e]; }
			
		private:
			
			bool RemoveAttachedObject(SceneObject* obj);
			bool RemoveObjectInternal(SceneObject* obj);

			OctreeSceneManager* m_manager;

			OctreeSceneNode* m_parent;
			OctreeSceneNode* m_childTable[8];
			OctreeBox m_boundingVolume;
			BoundingSphere m_boundingSphere;

			Extend GetExtend(const Vector3& pos) const;
			static Vector3 OffsetVectorTable[8];
		};

		class APAPI OctreeSceneManager : public SceneManager
		{
		public:
			OctreeSceneManager(const OctreeBox& range, float minBVSize);
			~OctreeSceneManager();

			virtual void AddObject(SceneObject* const obj);
			virtual bool RemoveObject(SceneObject* const obj);

			virtual void PrepareVisibleObjects(Camera* camera, BatchData* batchData);

			virtual void Update(const GameTime* time);

			virtual SceneObject* FindObject(const Ray& ray, IObjectFilter* filter);


			float getMinimumBVSize() const { return m_minimumBVSize; }

			bool QualifiesFarObject(const SceneObject* obj) const;

		private:
			LinkedList<SceneObject*> m_dynObjs;
			LinkedList<SceneObject*> m_farObjs;

			Queue<OctreeSceneNode*> m_bfsQueue;

			OctreeBox m_range;
			Vector3 m_min;
			Vector3 m_max;

			float m_minimumBVSize;

			OctreeSceneNode* m_octRootNode;

			void AddStaticObject(SceneObject* obj);

		};
	}
}
#endif