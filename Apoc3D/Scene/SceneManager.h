#pragma once
#ifndef APOC3D_SCENEMANAGER_H
#define APOC3D_SCENEMANAGER_H

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

#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		/** 
		 *  A callback interface used for ray-based selection check.
		 */
		class APAPI IObjectFilter
		{
		public:
			/**
			 *  Check if an intersected object should be accepted
			 */
			virtual bool Check(SceneObject* obj) = 0;
			virtual bool Check(SceneNode* node) { return true; }
		};


		/**
		 *  SceneManager keeps tracks of all scene objects.
		 */
		class APAPI SceneManager
		{
		public:
			SceneManager();
			virtual ~SceneManager();
		
			/**
			 *  Adds a new scene object into scene
			 */
			virtual void AddObject(SceneObject* const obj);
			/**
			 *  Removes a scene object from scene
			 */
			virtual bool RemoveObject(SceneObject* const obj);

			virtual void PrepareVisibleObjects(Camera* camera, BatchData* batchData) = 0;

			virtual void Update(const GameTime* time);

			virtual SceneObject* FindObject(const Ray& ray, IObjectFilter* filter) = 0;

			const List<SceneObject*>& getAllObjects() const { return m_objects; }
		private:
			List<SceneObject*> m_objects;
		protected:

		};
	};
};

#endif