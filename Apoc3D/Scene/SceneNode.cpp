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