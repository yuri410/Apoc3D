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