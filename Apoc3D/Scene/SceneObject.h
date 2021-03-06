#pragma once
#ifndef APOC3D_SCENEOBJECT_H
#define APOC3D_SCENEOBJECT_H

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

#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Graphics/Renderable.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		class APAPI SceneObject : public Renderable
		{
			RTTI_BASE;
		public:
			SceneObject(const bool hasSubObjs = false) 
				: m_hasSubObjects(hasSubObjs), m_parentNode(0), RequiresNodeUpdate(false)
			{
				m_transformation.LoadIdentity();
				m_BoundingSphere.Center = Vector3::Zero;
				m_BoundingSphere.Radius = 0;
			}

			virtual ~SceneObject(){}

			void NotifyParentNode(SceneNode* node)
			{
				m_parentNode = node;
			}

			virtual void PrepareVisibleObjects(const Camera* const camera, int32 level, BatchData* data) { }

			virtual void Update(const AppTime* time) = 0;

			virtual void OnAddedToScene(SceneManager* sceneMgr) { }
			virtual void OnRemovedFromScene(SceneManager* sceneMgr) { }

			virtual bool IntersectsSelectionRay(const Ray& ray)
			{
				return m_BoundingSphere.IntersectsRay(ray, nullptr);
			}

			virtual bool IsDynamicObject() const { return false; }

			bool hasSubObjects() const { return m_hasSubObjects; }
			SceneNode* getSceneNode() const { return m_parentNode; }

			const Matrix& getTrasformation() const { return m_transformation; }
			const BoundingSphere& getBoundingSphere() const { return m_BoundingSphere; }

			bool RequiresNodeUpdate;

		protected:
			Matrix m_transformation;

			BoundingSphere m_BoundingSphere;

		private:
			bool m_hasSubObjects;
			SceneNode* m_parentNode;

		};

		class APAPI Entity : public SceneObject
		{
			RTTI_DERIVED(Entity, SceneObject);
		public:
			bool Visible;
			Vector3 BoundingSphereOffset;

			void setRadius(float r) { m_BoundingSphere.Radius = r; }
			Model* getModel(int lod) { return m_models[lod]; }
			void setModel(int lod, Model* mdl) { m_models[lod] = mdl; }

			const Matrix& getOrientation() const { return m_orientation; }
			void setOrientation(const Matrix& ori) 
			{
				m_orientation = ori;
				m_isTransformDirty = true;
			}
			const Vector3& getPosition() const { return m_position; }
			void setPosition(const Vector3& pos)
			{
				m_position = pos;
				m_BoundingSphere.Center = pos + BoundingSphereOffset;
				m_isTransformDirty = true;
			}

			virtual void UpdateTransform();

			virtual RenderOperationBuffer* GetRenderOperation(int lod);

			virtual void Update(const AppTime* time);

		protected:
			Entity()
				: m_position(Vector3::Zero), Visible(true)
			{
				memset(m_models, 0, sizeof(Model*)*3);
				memset(&BoundingSphereOffset, 0, sizeof(BoundingSphereOffset));
				m_orientation.LoadIdentity();
			}

			Vector3 m_position;
			Matrix m_orientation;

			Model* m_models[3];
			bool m_isTransformDirty;

		};

		class APAPI StaticObject : public Entity
		{
			RTTI_DERIVED(StaticObject, Entity);
		public:
			StaticObject() { }
			StaticObject(const Vector3& position, const Matrix& orientation);


		};
		class APAPI DynamicObject : public Entity
		{
			RTTI_DERIVED(DynamicObject, Entity);
		public:
			DynamicObject() { }
			DynamicObject(const Vector3& position, const Matrix& orientation);

			virtual void UpdateTransform();

			virtual bool IsDynamicObject() const { return true; }
		};
	};
};
#endif