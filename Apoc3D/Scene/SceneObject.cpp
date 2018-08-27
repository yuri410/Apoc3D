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

#include "SceneObject.h"

#include "SceneNode.h"
#include "apoc3d/Graphics/Model.h"

namespace Apoc3D
{
	namespace Scene
	{
		void Entity::UpdateTransform()
		{
			Matrix::CreateTranslation(m_transformation, m_position);
			Matrix temp;
			Matrix::Multiply(temp, m_orientation, m_transformation);
			m_transformation = temp;

			m_BoundingSphere.Center = m_position + BoundingSphereOffset;
			RequiresNodeUpdate = true;
		}

		void Entity::Update(const GameTime* time)
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
			if (!Visible)
				return 0;

			while (!m_models[lod] && lod>0)
			{
				lod--;
			}
			if (m_models[lod])
			{
				return m_models[lod]->GetRenderOperation(lod);
			}
			return 0;
		}

		StaticObject::StaticObject(const Vector3& position, const Matrix& orientation)
		{
			m_position = position;
			m_orientation = orientation;
			UpdateTransform();
		}

		DynamicObject::DynamicObject(const Vector3& position, const Matrix& orientation)
		{
			m_position = position;
			m_orientation = orientation;
			UpdateTransform();
		}
		void DynamicObject::UpdateTransform()
		{
			Entity::UpdateTransform();
		}

	}
}