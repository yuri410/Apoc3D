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

#include "SceneNode.h"
#include "SceneObject.h"

namespace Apoc3D
{
	namespace Scene
	{
		SceneNode::SceneNode()
		{
		}

		SceneNode::~SceneNode()
		{
			for (int i=0;i<m_attached.getCount();i++)
			{
				if (m_attached[i]->getSceneNode() == this)
				{
					m_attached[i]->NotifyParentNode(nullptr);
				}
			}
		}

		void SceneNode::AddObject(SceneObject* sceObj)
		{
			m_attached.Add(sceObj);
			sceObj->NotifyParentNode(this);
		}
		void SceneNode::RemoveObject(SceneObject* sceObj)
		{
			m_attached.Remove(sceObj);
			sceObj->NotifyParentNode(nullptr);
		}
	};
};