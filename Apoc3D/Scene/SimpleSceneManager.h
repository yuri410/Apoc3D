#pragma once
#ifndef APOC3D_SIMPLESCENEMANAGER_H
#define APOC3D_SIMPLESCENEMANAGER_H

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

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		class APAPI SimpleSceneNode : public SceneNode
		{
		public:
			SimpleSceneNode(){}
			~SimpleSceneNode(){}

			virtual void AddObject(SceneObject* sceObj);
			virtual void RemoveObject(SceneObject* sceObj);
		};

		class APAPI SimpleSceneManager : public SceneManager
		{
		private:
			SimpleSceneNode* m_defaultNode;

		public:
			SimpleSceneManager();
			~SimpleSceneManager();

			virtual void AddObject(SceneObject* const obj);
			virtual bool RemoveObject(SceneObject* const obj);

			virtual void PrepareVisibleObjects(Camera* camera, BatchData* batchData);
			virtual SceneObject* FindObject(const Ray& ray, IObjectFilter* filter);
		};
	}
}
#endif