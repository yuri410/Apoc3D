#pragma once
#ifndef APOC3D_VERTEXDECLARATION_H
#define APOC3D_VERTEXDECLARATION_H

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

#include "VertexElement.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{			
			typedef List<VertexElement> VertexElementList;

			class APAPI VertexDeclaration
			{
			public:
				VertexDeclaration(const List<VertexElement> &e);

				virtual ~VertexDeclaration();

				/**
				 *  Finds a VertexElement with the given semantic, and index if there is more than
				 *  one element with the same semantic.
				 */
				const VertexElement* FindElementBySemantic(VertexElementUsage semantic) const
				{
					return FindElementBySemantic(semantic, -1);
				}

				/**
				 *  Finds a VertexElement with the given semantic, and index if there is more than
				 *  one element with the same semantic.
				 */
				const VertexElement* FindElementBySemantic(VertexElementUsage semantic, int32 index) const;
				
				int GetVertexSize() const;														/** Gets the vertex size defined by this declaration. */

				void CopyTo(VertexElementList& target) const;

				const VertexElement& operator[](int32 i) const { return m_elements[i]; }

				const VertexElement& getElement(int32 i) const  { return m_elements[i]; }		/** Gets the VertexElement at the specified index. */
				int getElementCount() const { return m_elements.getCount(); }					/** Gets the number of elements in the declaration. */

				const VertexElement* begin() const { return m_elements.begin(); }
				const VertexElement* end() const { return m_elements.end(); }

			protected:
				VertexElementList m_elements;

			};
		}
	}
}
#endif