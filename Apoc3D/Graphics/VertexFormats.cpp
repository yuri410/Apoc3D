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

#include "VertexFormats.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		FastList<VertexElement> VertexPIBNT1::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector4, VEU_BlendIndices) );
			elements.Add( VertexElement(elements[1].getOffset() + elements[1].getSize(), VEF_Vector4, VEU_BlendWeight) );
			elements.Add( VertexElement(elements[2].getOffset() + elements[2].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[3].getOffset() + elements[3].getSize(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const FastList<VertexElement> VertexPIBNT1::Elements = GetElements();

		String VertexPIBNT1::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) + 
				L"N: " + Vector4Utils::ToParsableString(BlendIndices) +
				L"N: " + Vector4Utils::ToParsableString(BlendWeight) +
				L"N: " + Vector3Utils::ToParsableString(Normal) +
				L"uv0: " + Vector2Utils::ToParsableString(TexCoord1);
		}

		//======================================================================================

		FastList<VertexElement> VertexPN::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );

			return elements;
		}
		const FastList<VertexElement> VertexPN::Elements = GetElements();

		String VertexPN::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) + L"N: " + Vector3Utils::ToParsableString(Normal);
		}

		//======================================================================================
		FastList<VertexElement> VertexPNT1::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const FastList<VertexElement> VertexPNT1::Elements = GetElements();

		String VertexPNT1::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) + L"N: " + Vector3Utils::ToParsableString(Normal) +
				L"uv0: " + Vector2Utils::ToParsableString(TexCoord1);
		} 

		//======================================================================================
		FastList<VertexElement> VertexPNT2::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );
			elements.Add( VertexElement(elements[2].getSize() + elements[2].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 1) );

			return elements;
		}
		const FastList<VertexElement> VertexPNT2::Elements = GetElements();

		String VertexPNT2::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) + L"N: " + Vector3Utils::ToParsableString(Normal) +
				L"uv0: " + Vector2Utils::ToParsableString(TexCoord1) + L"uv1: " + Vector2Utils::ToParsableString(TexCoord2);
		}

		//======================================================================================
		FastList<VertexElement> VertexPT1::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const FastList<VertexElement> VertexPT1::Elements = GetElements();

		String VertexPT1::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) +
				L"uv0: " + Vector2Utils::ToParsableString(TexCoord1);
		}

		//======================================================================================
		FastList<VertexElement> VertexPT2::GetElements()
		{
			FastList<VertexElement> elements;

			elements.Add( VertexElement(0, VEF_Vector3, VEU_Position) );
			elements.Add( VertexElement(elements[0].getSize(), VEF_Vector3, VEU_Normal) );
			elements.Add( VertexElement(elements[1].getSize() + elements[1].getOffset(), VEF_Vector2, VEU_TextureCoordinate, 0) );

			return elements;
		}
		const FastList<VertexElement> VertexPT2::Elements = GetElements();

		String VertexPT2::GetHashString() const
		{
			return L"Pos: " + Vector3Utils::ToParsableString(Position) +
				L"uv0: " + Vector2Utils::ToParsableString(TexCoord1) + L"uv1: " + Vector2Utils::ToParsableString(TexCoord2);
		} 
		//======================================================================================
	}
}