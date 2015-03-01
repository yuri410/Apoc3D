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
#include "CompilerService_SM3.h"
#include "BuildSystem.h"

namespace APBuild
{
	struct ShaderModel3ConstantTable : public ConstantTable
	{
		ShaderModel3ConstantTable(ID3DXConstantTable* constants);
	};

	ShaderModel3ConstantTable::ShaderModel3ConstantTable(ID3DXConstantTable* constants)
	{
		D3DXCONSTANTTABLE_DESC desc;
		HRESULT hr = constants->GetDesc(&desc);
		assert(SUCCEEDED(hr));

		for (uint32 i=0;i<desc.Constants;i++)
		{
			D3DXHANDLE handle = constants->GetConstant(NULL, i);

			D3DXCONSTANT_DESC descs;
			UINT count = 1;
			constants->GetConstantDesc(handle, &descs, &count);

			ShaderConstant constant;


			constant.Name = StringUtils::toPlatformWideString(descs.Name);
			constant.IsSampler = false;

			switch (descs.RegisterSet)
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

			constant.RegisterIndex = descs.RegisterIndex;
			constant.RegisterCount = descs.RegisterCount;

			switch (descs.Class)
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

			switch (descs.Type)
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

			constant.Rows = descs.Rows;
			constant.Columns = descs.Columns;
			constant.Elements = descs.Elements;
			constant.StructMembers = descs.StructMembers;
			constant.SizeInBytes = descs.Bytes;
			constant.SamplerIndex = constants->GetSamplerIndex(handle);
			
			m_table.Add(constant.Name, constant);
		}
	}

	void ProcessShaderModel3ByteCode(ShaderModel3ConstantTable& ct, const DWORD* byteCode, int32 codeSize, DWORD*& newByteCode, int32& newCodeSize)
	{
		MemoryOutStream newCodeBuffer(4096);
		BinaryWriter bw(&newCodeBuffer, false);

		const DWORD* ptr = byteCode;

		while (*ptr != D3DSIO_END)
		{
			DWORD cur = *ptr;

			if ((cur & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
			{
				DWORD commentBlobSize = (cur & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;

				ptr += commentBlobSize+1;
			}
			else
			{
				bw.WriteUInt32(cur);
				ptr++;
			}

		}

		MemoryOutStream metaDataBuffer(1024);
		{
			BinaryWriter bw2(&metaDataBuffer, false);
			ct.Write(&bw2);

			// align to dword
			int32 alignment = ((int32)metaDataBuffer.getLength() + 3) & ~0x03;
			alignment -= (int32)metaDataBuffer.getLength();

			for (int32 i = 0; i < alignment; i++)
				bw2.WriteByte(0);
		}
		DWORD commentSize = (DWORD)metaDataBuffer.getLength() / sizeof(DWORD) + 2; // 2 = 1x blob name(APBM) + 1x crc
		DWORD commentDword = D3DSHADER_COMMENT(commentSize);

		bw.WriteUInt32(commentDword);
		bw.WriteUInt32('APBM');

		bw.WriteUInt32(CalculateCRC32(metaDataBuffer.getDataPointer(), (int32)metaDataBuffer.getLength()));
		bw.Write(metaDataBuffer.getDataPointer(), metaDataBuffer.getLength());
		
		bw.WriteUInt32(D3DSIO_END);


		int32 sizeInDwords = (int32)newCodeBuffer.getLength()/sizeof(DWORD);
		newByteCode = new DWORD[sizeInDwords];
		newCodeSize = (int32)newCodeBuffer.getLength();
		memcpy(newByteCode, newCodeBuffer.getDataPointer(), newCodeSize);
	}

	bool CompileAsHLSLDX9(const String& src, const String& entryPoint, const char* pfName, bool debugEnabled, bool noOptimization,
		const List<std::pair<std::string, std::string>>* defines, ConstantTable*& _constantTable, char*& codePtr, int32& codeSize)
	{
		DWORD flags = D3DXSHADER_PACKMATRIX_ROWMAJOR;
		if (debugEnabled)
		{
			flags |= D3DXSHADER_DEBUG;
		}
		else
		{
			if (noOptimization)
			{
				flags |= D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_IEEE_STRICTNESS;
			}
			else
			{
				flags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
			}
		}

		D3DXMACRO* macros = nullptr;
		if (defines && defines->getCount())
		{
			macros = new D3DXMACRO[defines->getCount() + 1];
			memset(macros, 0, (defines->getCount() + 1) * sizeof(D3DXMACRO));

			for (int32 i = 0; i < defines->getCount(); i++)
			{
				macros[i].Name = defines->operator[](i).first.c_str();
				macros[i].Definition = defines->operator[](i).second.c_str();
			}
		}

		ID3DXBuffer* error = 0;
		ID3DXBuffer* shader = 0;

		HRESULT hr = D3DXCompileShaderFromFile(src.c_str(), macros, 0,
			StringUtils::toPlatformNarrowString(entryPoint).c_str(), pfName,
			flags, &shader, &error, NULL);

		delete[] macros;

		if (FAILED(hr))
		{
			if (error)
			{
				std::string errmsg(reinterpret_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize());
				BuildSystem::LogError(StringUtils::toPlatformWideString(errmsg), src);
				error->Release();
			}
			else
			{
				BuildSystem::LogError(L"Shader compiling failed due to unknown problem.", src);
			}
			return false;
		}

		ID3DXConstantTable* constants;
		hr = D3DXGetShaderConstantTableEx((const DWORD*)shader->GetBufferPointer(), D3DXCONSTTABLE_LARGEADDRESSAWARE, &constants);
		if (FAILED(hr))
		{
			BuildSystem::LogError(L"Unable to obtain constant information from shader.", src);
			shader->Release();
			return false;
		}

		ShaderModel3ConstantTable* constantTable = new ShaderModel3ConstantTable(constants);
		constants->Release();

		DWORD* newShaderCode;
		int32 newShaderCodeSize;
		ProcessShaderModel3ByteCode(*constantTable,
			(const DWORD*)shader->GetBufferPointer(), shader->GetBufferSize(),
			newShaderCode, newShaderCodeSize);

		shader->Release();

		codePtr = (char*)(newShaderCode);
		codeSize = newShaderCodeSize;
		_constantTable = constantTable;

		return true;
	}

}