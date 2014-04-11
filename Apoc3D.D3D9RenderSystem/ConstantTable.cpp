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
#include "ConstantTable.h"

#include "apoc3d/ApocException.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			void ConstantTable::ThrowKeyNotFoundEx(const String& name)
			{
				throw AP_EXCEPTION(EX_KeyNotFound, name);
			}
			

			ConstantTable::ConstantTable(const DWORD* bytes)
			{
				
				ID3DXConstantTable* constants;
				HRESULT hr = D3DXGetShaderConstantTableEx(reinterpret_cast<const DWORD*>(bytes), 
					D3DXCONSTTABLE_LARGEADDRESSAWARE, &constants);
				assert(SUCCEEDED(hr));

				D3DXCONSTANTTABLE_DESC desc;
				hr = constants->GetDesc(&desc);
				assert(SUCCEEDED(hr));
				
				for (uint32 i=0;i<desc.Constants;i++)
				{
					D3DXHANDLE handle = constants->GetConstant(NULL, i);

					D3DXCONSTANT_DESC descs[16];
					UINT count = 16;
					constants->GetConstantDesc(handle, descs, &count);
					
					ShaderConstant constant;

					
					constant.Name = StringUtils::toWString(descs[0].Name);
					constant.IsSampler = false;

					switch (descs[0].RegisterSet)
					{
					case D3DXRS_BOOL:
						constant.RegisterSet = SREG_Bool;
						break;
					case D3DXRS_INT4:
						constant.RegisterSet = SREG_Int4;
						break;
					case D3DXRS_FLOAT4:
						constant.RegisterSet = SREG_Float4;
						break;
					case D3DXRS_SAMPLER:
						constant.RegisterSet = SREG_Sampler;
						constant.IsSampler = true;
						break;
					}

					constant.RegisterIndex = descs[0].RegisterIndex;
					constant.RegisterCount = descs[0].RegisterCount;

					switch (descs[0].Class)
					{
					case D3DXPC_SCALAR:
						constant.Class = SCC_Scalar;
						break;
					case D3DXPC_VECTOR:
						constant.Class = SCC_Vector;
						break;
					case D3DXPC_MATRIX_ROWS:
						constant.Class = SCC_Matrix_Rows;
						break;
					case D3DXPC_MATRIX_COLUMNS:
						constant.Class = SCC_Matrix_Columns;
						break;
					case D3DXPC_OBJECT:
						constant.Class = SCC_Object;
						break;
					case D3DXPC_STRUCT:
						constant.Class = SCC_Struct;
						break;
					}

					switch (descs[0].Type)
					{
					case D3DXPT_VOID:
						constant.Type = SCT_VoidPointer;
						break;
					case D3DXPT_BOOL:
						constant.Type = SCT_Boolean;
						break;
					case D3DXPT_INT:
						constant.Type = SCT_Integer;
						break;
					case D3DXPT_FLOAT:
						constant.Type = SCT_Float;
						break;
					case D3DXPT_STRING:
						constant.Type = SCT_String;
						break;
					case D3DXPT_TEXTURE:
						constant.Type = SCT_Texture;
						break;
					case D3DXPT_TEXTURE1D:
						constant.Type = SCT_Texture1D;
						break;
					case D3DXPT_TEXTURE2D:
						constant.Type = SCT_Texture2D;
						break;
					case D3DXPT_TEXTURE3D:
						constant.Type = SCT_Texture3D;
						break;
					case D3DXPT_TEXTURECUBE:
						constant.Type = SCT_TextureCube;
						break;
					case D3DXPT_SAMPLER:
						constant.Type = SCT_Sampler;
						break;
					case D3DXPT_SAMPLER1D:
						constant.Type = SCT_Sampler1D;
						break;
					case D3DXPT_SAMPLER2D:
						constant.Type = SCT_Sampler2D;
						break;
					case D3DXPT_SAMPLER3D:
						constant.Type = SCT_Sampler3D;
						break;
					case D3DXPT_SAMPLERCUBE:
						constant.Type = SCT_SamplerCube;
						break;
					case D3DXPT_PIXELSHADER:
						constant.Type = SCT_PixelShader;
						break;
					case D3DXPT_VERTEXSHADER:
						constant.Type = SCT_VertexShader;
						break;
					case D3DXPT_PIXELFRAGMENT:
						constant.Type = SCT_PixelFragment;
						break;
					case D3DXPT_VERTEXFRAGMENT:
						constant.Type = SCT_VertexFragment;
						break;
					}

					constant.Rows = descs[0].Rows;
					constant.Columns = descs[0].Columns;
					constant.Elements = descs[0].Elements;
					constant.StructMembers = descs[0].StructMembers;
					constant.SizeInBytes = descs[0].Bytes;
					for (int i=0;i<16;i++) constant.SamplerIndex[0] = -1;
					constant.SamplerIndex[0] = constants->GetSamplerIndex(handle);
					//for (uint j=0;j<16;j++)
					//{
					//	if (
					//	constant.SamplerIndex[j] < 
					//	constant.SamplerIndex[j] = -1;
					//}
					m_table.Add(constant.Name, constant);
				}

				constants->Release();
			}

			ConstantTable::~ConstantTable()
			{

			}

		}
	}
}