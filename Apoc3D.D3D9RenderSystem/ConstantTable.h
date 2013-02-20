#pragma once
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

#ifndef APOC3D_D3D9_CONSTANTTABLE_H
#define APOC3D_D3D9_CONSTANTTABLE_H

#include "D3D9Common.h"

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			enum ShaderRegisterSetType
			{
				SREG_Bool,
				SREG_Int4,
				SREG_Float4,
				SREG_Sampler
			};
			enum ShaderConstantClass
			{
				SCC_Scalar,
				SCC_Vector,
				SCC_Matrix_Rows,
				SCC_Matrix_Columns,
				SCC_Object,
				SCC_Struct
			};
			enum ShaderConstantType
			{
				SCT_VoidPointer,
				SCT_Boolean,
				SCT_Integer,
				SCT_Float,
				SCT_String,
				SCT_Texture,
				SCT_Texture1D,
				SCT_Texture2D,
				SCT_Texture3D,
				SCT_TextureCube,
				SCT_Sampler,
				SCT_Sampler1D,
				SCT_Sampler2D,
				SCT_Sampler3D,
				SCT_SamplerCube,
				SCT_PixelShader,
				SCT_VertexShader,
				SCT_PixelFragment,
				SCT_VertexFragment,
				SCT_Unsupported
			};
			struct ShaderConstant
			{
				String Name;
				ShaderRegisterSetType RegisterSet;
				uint32 RegisterIndex;
				uint32 RegisterCount;
				ShaderConstantClass Class;
				ShaderConstantType Type;
				uint32 Rows;
				uint32 Columns;
				uint32 Elements;
				uint32 StructMembers;
				uint32 SizeInBytes;
				int32 SamplerIndex[16];
			};

			class ConstantTable
			{
			private:
				unordered_map<String, ShaderConstant> m_table;
				
				void ThrowKeyNotFoundEx(const String& name) const;
			public:
				ConstantTable(const DWORD* bytes);
				~ConstantTable();

				inline const ShaderConstant& getConstant(const String& name) const;
			};

			const ShaderConstant& ConstantTable::getConstant(const String& name) const
			{				
				unordered_map<String, ShaderConstant>::const_iterator iter = m_table.find(name);
			
				if (iter != m_table.end())
				{
					return iter->second;
				}
				ThrowKeyNotFoundEx(name);
				throw; // keep the compiler happy
			}
		}
	}
}


#endif