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
#ifndef VERTEXELEMENT_H
#define VERTEXELEMENT_H

#include "Common.h"
#include "Graphics\GraphicsCommon.h"

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class _Export VertexElement
			{
			private:
				int m_offset;
				VertexElementFormat m_type;
				VertexElementUsage m_semantic;
				int m_index;
			public:
				/* Gets the offset in the buffer that this element starts at.
				*/
				int getOffset() { return m_offset; }
				/* Gets the type of element.
				*/
				VertexElementFormat getType() { return m_type; }
				/* Gets the meaning of the element.
				*/
				VertexElementUsage getUsage() { return m_semantic; }
				/* Gets index of the item, only applicable for some elements like texture coords
				*/
				int getIndex() { return m_index; }
				int getSize() { return GetTypeSize(m_type); }

				VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic)					
				{
					VertexElement(offset, type, semantic,0);
				}

				VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic, int index)
				{
					m_offset = offset;
					m_type = type;
					m_semantic = semantic;
					m_index = index;

				}

				static bool Compare(const vector<VertexElement> &e1, const vector<VertexElement> &e2)
				{
					if (e1.size()==e2.size())
					{
						for (size_t i=0;i<e1.size();i++)
						{
							if (e1[i]!=e2[i])
							{
								return false;
							}
						}
						return true;
					}
					return false;
				}

				/* Calculate the size of a specified vertex element format
				*/
				static int GetTypeSize(VertexElementFormat type)
				{
					int t = static_cast<int>(type);
					switch (t)
					{
					case (int)VEF_Rg32:
					case (int)VEF_Rgba32:
					case (int)VEF_Color:
						return sizeof(int32);

					case (int)VEF_Single:
						return sizeof(float);

					case (int)VEF_Vector2:
						return sizeof(float) * 2;

					case (int)VEF_Vector3:
						return sizeof(float) * 3;

					case (int)VEF_Vector4:
						return sizeof(float) * 4;

					case (int)VEF_NormalizedShort2:
					case (int)VEF_Short2:
						return sizeof(short) * 2;

					case (int)VEF_NormalizedShort4:
					case (int)VEF_Short4:
						return sizeof(short) * 4;

					case (int)VEF_Byte4:
						return sizeof(byte) * 4;
					case (int)VEF_HalfVector2:
						return sizeof(ushort) * 2;
					case (int)VEF_HalfVector4:
						return sizeof(ushort) * 4;

					}
					return 0;
				}
		
				/* Utility method which returns the count of values in a given type.
				*/
				static int GetTypeCount(VertexElementFormat type)
				{
					switch (type)
					{
					case VEF_Rg32:
					case VEF_Rgba32:
					case VEF_Color:
						return 1;

					case VEF_Single:
						return 1;

					case VEF_Vector2:
						return 2;

					case VEF_Vector3:
						return 3;

					case VEF_Vector4:
						return 4;

					case VEF_NormalizedShort2:
					case VEF_Short2:
						return 2;

					case VEF_NormalizedShort4:
					case VEF_Short4:
						return 4;

					case VEF_Byte4:
						return 4;

					case VEF_HalfVector2:
						return 2;
					case VEF_HalfVector4:
						return 4;
					} // end switch			

					// keep the compiler happy
					return 0;
				}

				friend static bool operator ==(const VertexElement const &left, const VertexElement const &right)
				{
					return left.index == right.index &&
						left.offset == right.offset &&
						left.semantic == right.semantic &&
						left.type == right.type;
				}
				friend static bool operator !=(const VertexElement const &left, const VertexElement const &right)
				{
					return left.index != right.index ||
						left.offset != right.offset ||
						left.semantic != right.semantic ||
						left.type != right.type;
				}

			};
		}
	}
}

#endif