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

#include "CompilerService_SM3.h"
#include "BuildSystem.h"

#include <dxsdk/d3dx9.h>
#include <dxsdk/d3dx11.h>

#include <dxsdk/d3dcompiler.h>

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
				case D3DXPC_SCALAR: constant.Class = SCC_Scalar; break;
				case D3DXPC_VECTOR: constant.Class = SCC_Vector; break;
				case D3DXPC_MATRIX_ROWS: constant.Class = SCC_Matrix_Rows; break;
				case D3DXPC_MATRIX_COLUMNS: constant.Class = SCC_Matrix_Columns; break;
				case D3DXPC_OBJECT: constant.Class = SCC_Object; break;
				case D3DXPC_STRUCT: constant.Class = SCC_Struct; break;
			}

			switch (descs.Type)
			{
				case D3DXPT_VOID: constant.Type = SCT_VoidPointer; break;
				case D3DXPT_BOOL: constant.Type = SCT_Boolean; break;
				case D3DXPT_INT: constant.Type = SCT_Integer; break;
				case D3DXPT_FLOAT: constant.Type = SCT_Float; break;
				case D3DXPT_STRING: constant.Type = SCT_String; break;
				case D3DXPT_TEXTURE: constant.Type = SCT_Texture; break;
				case D3DXPT_TEXTURE1D: constant.Type = SCT_Texture1D; break;
				case D3DXPT_TEXTURE2D: constant.Type = SCT_Texture2D; break;
				case D3DXPT_TEXTURE3D: constant.Type = SCT_Texture3D; break;
				case D3DXPT_TEXTURECUBE: constant.Type = SCT_TextureCube; break;
				case D3DXPT_SAMPLER: constant.Type = SCT_Sampler; break;
				case D3DXPT_SAMPLER1D: constant.Type = SCT_Sampler1D; break;
				case D3DXPT_SAMPLER2D: constant.Type = SCT_Sampler2D; break;
				case D3DXPT_SAMPLER3D: constant.Type = SCT_Sampler3D; break;
				case D3DXPT_SAMPLERCUBE: constant.Type = SCT_SamplerCube; break;
				case D3DXPT_PIXELSHADER: constant.Type = SCT_PixelShader; break;
				case D3DXPT_VERTEXSHADER: constant.Type = SCT_VertexShader; break;
				case D3DXPT_PIXELFRAGMENT: constant.Type = SCT_PixelFragment; break;
				case D3DXPT_VERTEXFRAGMENT: constant.Type = SCT_VertexFragment; break;
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
		bw.WriteBytes(metaDataBuffer.getDataPointer(), metaDataBuffer.getLength());
		
		bw.WriteUInt32(D3DSIO_END);


		int32 sizeInDwords = (int32)newCodeBuffer.getLength()/sizeof(DWORD);
		newByteCode = new DWORD[sizeInDwords];
		newCodeSize = (int32)newCodeBuffer.getLength();
		memcpy(newByteCode, newCodeBuffer.getDataPointer(), newCodeSize);
	}

	bool CompileAsHLSLDX9(const String& src, const String& entryPoint, const char* pfName, bool debugEnabled, bool noOptimization,
		const List<std::pair<std::string, std::string>>& defines, ConstantTable*& _constantTable, char*& codePtr, int32& codeSize)
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
		if (defines.getCount() > 0)
		{
			macros = new D3DXMACRO[defines.getCount() + 1];
			memset(macros, 0, (defines.getCount() + 1) * sizeof(D3DXMACRO));

			for (int32 i = 0; i < defines.getCount(); i++)
			{
				macros[i].Name = defines[i].first.c_str();
				macros[i].Definition = defines[i].second.c_str();
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

	struct IncludeHandler : ID3D10Include
	{
		String m_basePath;

		IncludeHandler(const String& basePath)
			: m_basePath(basePath) { }


		HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
		{
			String fn = PathUtils::Combine(m_basePath, StringUtils::toPlatformWideString(pFileName));

			if (!File::FileExists(fn))
			{
				return D3D10_ERROR_FILE_NOT_FOUND;
			}

			String cnt = IO::Encoding::ReadAllText(FileLocation(fn), IO::Encoding::TEC_Unknown);

			std::string ncnt = StringUtils::toPlatformNarrowString(cnt);

			// do not change these, it will break d3d preprocessor
			char* result = new char[ncnt.size() + 1];
			memset(result, ncnt.size() + 1, 0);

			memcpy(result, ncnt.c_str(), ncnt.size());

			*ppData = result;
			*pBytes = (UINT)ncnt.size() + 1;

			return S_OK;
		}

		HRESULT __stdcall Close(THIS_ LPCVOID pData)
		{
			char* txt = (char*)pData;
			delete[] txt;

			return S_OK;
		}

	};

	bool PreprocessShaderCode(const String& srcFile, const List<std::pair<std::string, std::string>>& defines, std::string& result)
	{
		D3D10_SHADER_MACRO* macros = nullptr;
		{
			macros = new D3D10_SHADER_MACRO[defines.getCount() + 1];
			memset(macros, 0, (defines.getCount() + 1) * sizeof(D3D10_SHADER_MACRO));

			for (int32 i = 0; i < defines.getCount(); i++)
			{
				macros[i].Name = defines[i].first.c_str();
				macros[i].Definition = defines[i].second.c_str();
			}
		}

		String code = IO::Encoding::ReadAllText(FileLocation(srcFile), Encoding::TEC_Unknown);
		std::string ncode = StringUtils::toPlatformNarrowString(code);

		IncludeHandler includer(PathUtils::GetDirectory(srcFile));
		ID3DBlob* pBlobOut = NULL;
		ID3DBlob* pErrorBlob = NULL;
		HRESULT hr = D3DPreprocess(ncode.c_str(), ncode.size(), NULL, macros, &includer, &pBlobOut, &pErrorBlob);
		delete[] macros;

		if (FAILED(hr))
			return false;

		result = std::string((char*)pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize());
		
		pBlobOut->Release();

		if (pErrorBlob)
			pErrorBlob->Release();

		// replace strange output char 0xcd from D3DPreprocess
		for (char& c : result)
		{
			if (c == (char)0xcd) // convert to need a sign
				c = ' ';
		}

		return true;
	}
}