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

#include "VertexDeclaration.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			VertexDeclaration::VertexDeclaration(const List<VertexElement> &e)
				: m_elements(e)
			{ } 

			VertexDeclaration::~VertexDeclaration()
			{

			}
			
			const VertexElement* VertexDeclaration::FindElementBySemantic(VertexElementUsage semantic, int32 index) const
			{
				if (index==-1)
				{
					for (const VertexElement& ve : m_elements)
					{
						if (ve.getUsage() == semantic)
							return &ve;
					}
				}
				else
				{
					for (const VertexElement& ve : m_elements)
					{
						if (ve.getUsage() == semantic && 
							ve.getIndex() == index)
							return &ve;
					}
				}

				return nullptr;
			}

			int VertexDeclaration::GetVertexSize() const
			{
				int size = 0;

				for (int32 i = 0; i < m_elements.getCount(); i++)
				{
					size += m_elements[i].getSize();
				}

				return size;
			}

			void VertexDeclaration::CopyTo(VertexElementList& target) const
			{
				target = m_elements;
			}

		}
	}
}