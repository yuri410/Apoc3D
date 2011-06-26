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

#include "SceneManager.h"

#include "SceneObject.h"
#include "Graphics\RenderOperationBuffer.h"
#include "Graphics\RenderOperation.h"
#include "Graphics\Material.h"
#include "Graphics\GeometryData.h"
#include "Core\GameTime.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Scene
	{
		SceneManager::SceneManager(void)
		{
		}


		SceneManager::~SceneManager(void)
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

		void SceneManager::Update(const GameTime* const &time)
		{
			for (int32 i = 0;i<m_objects.getCount();i++)
			{
				m_objects[i]->Update(time);
			}
		}
	};
};