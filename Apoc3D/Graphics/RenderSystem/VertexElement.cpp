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

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			VertexElement* VertexElement::FindElementBySemantic(const List<VertexElement>& elem, VertexElementUsage semantic)
			{
				for (int i=0;i<elem.getCount();i++)
				{
					if (elem[i].getUsage() == semantic)
						return &elem[i];
				}
				return 0;
			}

			bool VertexElement::Compare(const List<VertexElement> &e1, const List<VertexElement> &e2)
			{
				if (e1.getCount()==e2.getCount())
				{
					for (int32 i=0;i<e1.getCount();i++)
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

			int VertexElement::GetTypeSize(VertexElementFormat type)
			{
				int t = static_cast<int>(type);
				switch (t)
				{
				case (int)VEF_NormalizedByte4:
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

			int VertexElement::GetTypeCount(VertexElementFormat type)
			{
				switch (type)
				{
				case VEF_NormalizedByte4:
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
				}

				// keep the compiler happy
				return 0;
			}
		}
	}
}