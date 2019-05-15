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

#include "SceneObject.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		SceneManager::SceneManager()
		{
		}


		SceneManager::~SceneManager()
		{
		}

		void SceneManager::AddObject(SceneObject* const obj)
		{
			m_objects.Add(obj);
		} 
		bool SceneManager::RemoveObject(SceneObject* const obj)
		{
			return m_objects.Remove(obj);
		}

		void SceneManager::Update(const AppTime* time)
		{
			for (int32 i = 0;i<m_objects.getCount();i++)
			{
				m_objects[i]->Update(time);
			}
		}
	};
};