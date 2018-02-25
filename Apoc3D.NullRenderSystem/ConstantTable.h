#pragma once

/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#ifndef APOC3D_NRS_CONSTANTTABLE_H
#define APOC3D_NRS_CONSTANTTABLE_H

#include "NRSCommon.h"
#include "apoc3d/Core/Plugin.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			enum ShaderRegisterSetType : byte
			{
				SREG_Bool,
				SREG_Int4,
				SREG_Float4,
				SREG_Sampler
			};
			enum ShaderConstantClass : byte
			{
				SCC_Scalar,
				SCC_Vector,
				SCC_Matrix_Rows,
				SCC_Matrix_Columns,
				SCC_Object,
				SCC_Struct
			};
			enum ShaderConstantType : byte
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
				uint16 RegisterIndex;
				uint16 RegisterCount;
				byte SamplerIndex;
				bool IsSampler;

				ShaderRegisterSetType RegisterSet;
				ShaderConstantClass Class;
				ShaderConstantType Type;
				byte Rows;
				byte Columns;
				uint16 Elements;
				uint16 StructMembers;
				uint32 SizeInBytes;
			};

			class PLUGINAPI ConstantTable
			{
			public:
				ConstantTable(const byte* bytes);
				virtual ~ConstantTable();

				inline const ShaderConstant& getConstant(const String& name) const;
				inline const ShaderConstant* tryGetConstant(const String& name) const { return m_table.TryGetValue(name); }

				void Read(BinaryReader* br);
				void Write(BinaryWriter* bw);

				void GetAllConstantNames(List<String>& list) const { return m_table.FillKeys(list); }

			protected:
				ConstantTable() { }

				void ReadShaderComment(char* data, int32 size);

				HashMap<String, ShaderConstant> m_table;

				NO_INLINE static void ThrowKeyNotFoundEx(const String& name);
			};

			const ShaderConstant& ConstantTable::getConstant(const String& name) const
			{
				ShaderConstant* sc = m_table.TryGetValue(name);
				
				if (sc)
				{
					return *sc;
				}
				ThrowKeyNotFoundEx(name);
				throw; // keep the compiler happy
			}

		}
	}
}


#endif