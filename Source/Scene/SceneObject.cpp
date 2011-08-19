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
#include "SceneObject.h"

#include "SceneNode.h"
#include "Graphics/Model.h"

namespace Apoc3D
{
	namespace Scene
	{
		void Entity::UpdateTransform()
		{
			Matrix::CreateTranslation(m_transformation, m_position);
			Matrix::Multiply(m_transformation, m_orientation, m_transformation);
			RequiresNodeUpdate = true;
		}

		void Entity::Update(const GameTime* const time)
		{
			if (m_models[0])
			{
				m_models[0]->Update(time);
			}

			if (m_isTransformDirty)
			{
				UpdateTransform();
				m_isTransformDirty = false;
			}
		}

		RenderOperationBuffer* Entity::GetRenderOperation(int lod)
		{
			while (!m_models[lod] && lod>=0)
			{
				lod--;
			}
			if (m_models[lod])
			{
				return m_models[lod]->GetRenderOperation(lod);
			}
			return 0;
		}

		void DynamicObject::UpdateTransform()
		{
			Entity::UpdateTransform();
			
			m_BoundingSphere.Center = Vector3Utils::Add(m_position, BoundingSphereOffset);

			RequiresNodeUpdate = true;
		}

	}
}