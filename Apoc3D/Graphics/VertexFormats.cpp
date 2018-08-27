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

#include "VertexFormats.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		List<VertexElement> VertexPIBNT1::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector4, VEU_BlendIndices) );
			elements.Add( VertexElement(elements[1].getOffset() + elements[1].getSize(), VEF_Vector4, VEU_BlendWeight) );
			elements.Add( VertexElement(elements[2].getOffset() + elements[2].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[3].getOffset() + elements[3].getSize(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const List<VertexElement> VertexPIBNT1::Elements = GetElements();

		String VertexPIBNT1::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() + 
				L"BIdx: " + BlendIndices.ToParsableString() +
				L"BWgt: " + BlendWeight.ToParsableString() +
				L"N: " + Normal.ToParsableString() +
				L"uv0: " + TexCoord1.ToParsableString();
		}

		//======================================================================================

		List<VertexElement> VertexPN::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );

			return elements;
		}
		const List<VertexElement> VertexPN::Elements = GetElements();

		String VertexPN::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() + L"N: " + Normal.ToParsableString();
		}

		//======================================================================================
		List<VertexElement> VertexPNT1::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const List<VertexElement> VertexPNT1::Elements = GetElements();

		String VertexPNT1::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() + L"N: " + Normal.ToParsableString() +
				L"uv0: " + TexCoord1.ToParsableString();
		} 

		//======================================================================================
		List<VertexElement> VertexPNT2::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );
			elements.Add( VertexElement(elements[2].getSize() + elements[2].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 1) );

			return elements;
		}
		const List<VertexElement> VertexPNT2::Elements = GetElements();

		String VertexPNT2::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() + L"N: " + Normal.ToParsableString() +
				L"uv0: " + TexCoord1.ToParsableString() + L"uv1: " + TexCoord2.ToParsableString();
		}

		//======================================================================================
		List<VertexElement> VertexPT1::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const List<VertexElement> VertexPT1::Elements = GetElements();

		String VertexPT1::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() +
				L"uv0: " + TexCoord1.ToParsableString();
		}

		//======================================================================================
		List<VertexElement> VertexPT2::GetElements()
		{
			List<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const List<VertexElement> VertexPT2::Elements = GetElements();

		String VertexPT2::GetHashString() const
		{
			return L"Pos: " + Position.ToParsableString() +
				L"uv0: " + TexCoord1.ToParsableString() + L"uv1: " + TexCoord2.ToParsableString();
		} 
		//======================================================================================
	}
}