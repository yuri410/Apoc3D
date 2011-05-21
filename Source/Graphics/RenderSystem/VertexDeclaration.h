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
#ifndef VERTEXDECLARATION_H
#define VERTEXDECLARATION_H

#include "Common.h"
#include "Graphics/GraphicsCommon.h"
#include "VertexElement.h"

using namespace Apoc3D::Core;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APOC3D_API VertexDeclaration
			{
			protected:
				vector<const VertexElement> elements;

				VertexDeclaration(const vector<VertexElement> &e)
				{
					for (size_t i=0;i<e.size();i++)
					{
						elements.push_back(e[i]);
					}
				}

			public:
				/*  Gets the number of elements in the declaration.
				*/
				int getElementCount() const
				{
					return static_cast<int>(elements.size());
				}

				/* Finds a VertexElement with the given semantic, and index if there is more than 
				 * one element with the same semantic. 
				*/
				bool FindElementBySemantic(VertexElementUsage semantic, VertexElement& result) const
				{
					FindElementBySemantic(semantic, 0, result);
				}

				/* Finds a VertexElement with the given semantic, and index if there is more than 
				 * one element with the same semantic. 
				*/
				virtual bool FindElementBySemantic(VertexElementUsage semantic, int index, VertexElement& result) const
				{
					for (int i = 0; i < elements.size(); i++)
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

				/* Gets the VertexElement at the specified index.
				*/
				const VertexElement &getElement(int index) const
				{
					return elements[index];
				}

				/* Gets the vertex size defined by this declaration.
				*/
				virtual int GetVertexSize() const
				{
					int size = 0;

					for (size_t i = 0; i < elements.size(); i++)
					{
						size += elements[i].getSize();
					}

					// return the size
					return size;
				}
			};
		}
	}
}
#endif