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
			int VertexDeclaration::getElementCount() const
			{
				return static_cast<int>(elements.size());
			}
			const VertexElement& VertexDeclaration::getElement(int index) const
			{
				return elements[index];
			}

			VertexDeclaration::VertexDeclaration(const FastList<VertexElement> &e)
			{
				for (int i=0;i<e.getCount();i++)
				{
					elements.push_back(e[i]);
				}
			}
			VertexDeclaration::VertexDeclaration(const vector<VertexElement> &e)
			{
				for (size_t i=0;i<e.size();i++)
				{
					elements.push_back(e[i]);
				}
			}
			bool VertexDeclaration::FindElementBySemantic(VertexElementUsage semantic, int index, VertexElement& result) const
			{
				for (size_t i = 0; i < elements.size(); i++)
				{
					const VertexElement &element = elements[i];

					// do they match?
					if (element.getUsage() == semantic && element.getIndex() == index)
					{
						result = element;
						return true;
					}
				}

				return false;
			}
			int VertexDeclaration::GetVertexSize() const
			{
				int size = 0;

				for (size_t i = 0; i < elements.size(); i++)
				{
					size += elements[i].getSize();
				}

				// return the size
				return size;
			}
		}
	}
}