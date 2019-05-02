#pragma once
#ifndef APOC3D_VERTEXELEMENT_H
#define APOC3D_VERTEXELEMENT_H

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

#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APAPI VertexElement
			{
			public:
				VertexElement();
				VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic);
				VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic, int index);

				/** Gets the offset in the buffer that this element starts at. */
				int getOffset() const { return m_offset; }
				
				/** Gets the type of element. */
				VertexElementFormat getType() const { return m_type; }
				
				/** Gets the meaning of the element. */
				VertexElementUsage getUsage() const { return m_semantic; }
				
				/** Gets index of the item, only applicable for some elements like texture coords */
				int getIndex() const { return m_index; }
				int getSize() const { return GetTypeSize(m_type); }


				bool operator ==(const VertexElement& another) const;
				bool operator !=(const VertexElement& another) const { return !(this->operator ==(another)); }


				static VertexElement* FindElementBySemantic(const List<VertexElement>& elem, VertexElementUsage semantic);
				
				static bool Compare(const List<VertexElement>& e1, const List<VertexElement>& e2);

				/**
				 *  Calculate the size of a specified vertex element format
				 */
				static int GetTypeSize(VertexElementFormat type);
		
				/**
				 *  Utility method which returns the count of values in a given type.
				 */
				static int GetTypeCount(VertexElementFormat type);

			private:
				int m_offset;
				VertexElementFormat m_type;
				VertexElementUsage m_semantic;
				int m_index;
			};
		}
	}
}

#endif