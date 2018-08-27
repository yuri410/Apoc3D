#pragma once
#ifndef APOC3D_SCENENODE_H
#define APOC3D_SCENENODE_H

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

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Scene
	{
		typedef List<SceneObject*> SceneObjectList;

		/**
		 *  Represents a collection of scene objects in the scene
		 */
		class APAPI SceneNode
		{
		public:
			SceneNode();
			virtual ~SceneNode();

			int32 getCount() { return m_attached.getCount(); }
			SceneObject* operator [](int i) { return m_attached[i]; }

			virtual void AddObject(SceneObject* sceObj);
			virtual void RemoveObject(SceneObject* sceObj);

			const SceneObjectList& getAttachedObjects() const {  return m_attached; }

		protected:
			SceneObjectList m_attached;
		};
	};
};
#endif