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


#include "CompileService.h"

#include "BuildConfig.h"
#include "BuildSystem.h"

#include "CompilerService_SM3.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

namespace APBuild
{
	const char* getHLSLCompilerProfileName(EffectProfileData& pd, ShaderType type);
	
	void WriteCompileError(const std::string& logs, const String& sourceFile);
	void FillEffectProfileData(EffectProfileData& pd, const char* data, const int32 length, ShaderType type);

	bool CompileShader(const String& src, const String& entryPoint, EffectProfileData& profData, ShaderType type, 
		bool debugEnabled, bool noOptimization, const List<std::pair<std::string, std::string>>* defines)
	{
		if (profData.MatchImplType(EffectProfileData::Imp_HLSL))
		{
			const char* pfName = getHLSLCompilerProfileName(profData, type);

			if (profData.MajorVer > 3)
			{
				DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
				if (debugEnabled)
				{
					dwShaderFlags |= D3DCOMPILE_DEBUG;
				}
				else
				{
					if (noOptimization)
					{
						dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_NO_PRESHADER | D3DCOMPILE_IEEE_STRICTNESS;
					}
					else
					{
						dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
					}
				}

				D3D10_SHADER_MACRO* macros = nullptr;
				if (defines)
				{
					macros = new D3D10_SHADER_MACRO[defines->getCount() + 1];
					memset(macros, 0, (defines->getCount() + 1) * sizeof(D3D10_SHADER_MACRO));

					for (int32 i = 0; i < defines->getCount(); i++)
					{
						macros[i].Name = defines->operator[](i).first.c_str();
						macros[i].Definition = defines->operator[](i).second.c_str();
					}
				}

				ID3DBlob* pBlobOut;
				ID3DBlob* pErrorBlob;
				HRESULT hr = D3DX11CompileFromFile(src.c_str(), macros, NULL, StringUtils::toPlatformNarrowString(entryPoint).c_str(), pfName,
					dwShaderFlags, 0, NULL, &pBlobOut, &pErrorBlob, NULL);

				delete[] macros;

				if (FAILED(hr))
				{
					if (pErrorBlob != NULL)
					{
						std::string errmsg = (const char*)pErrorBlob->GetBufferPointer();
						WriteCompileError(errmsg, src);
						pErrorBlob->Release();
					}
					else
					{
						BuildSystem::LogError(L"Failed due to unknown problem.", src);
					}
					return false;
				}

				FillEffectProfileData(profData, (const char*)pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), type);

				pBlobOut->Release();
				return true;
			}
			else
			{
				ID3DXBuffer* error = 0;
				ID3DXBuffer* shader = 0;

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

				//flags |= D3DXCONSTTABLE_LARGEADDRESSAWARE;

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
				
				HRESULT hr = D3DXCompileShaderFromFile(src.c_str(), macros, 0,
					StringUtils::toPlatformNarrowString(entryPoint).c_str(), pfName,
					flags, &shader, &error, NULL);

				delete[] macros;


				if (FAILED(hr))
				{
					if (error)
					{
						std::string errmsg(reinterpret_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize());
						WriteCompileError(errmsg, src);
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

				ShaderModel3ConstantTable constantHelper(constants);
				constants->Release();

				DWORD* newShaderCode;
				int32 newShaderCodeSize;
				ProcessShaderModel3ByteCode(constantHelper,
					(const DWORD*)shader->GetBufferPointer(), shader->GetBufferSize(),
					newShaderCode, newShaderCodeSize);

				shader->Release();


				FillEffectProfileData(profData, (const char*)newShaderCode, newShaderCodeSize, type);

				delete[] newShaderCode;

				return true;
			}
		}

		BuildSystem::LogError(L"Profile not supported.", src);
		return false;
	}

	

	void FillEffectProfileData(EffectProfileData& pd, const char* data, const int32 length, ShaderType type)
	{
		if (type == SHDT_Vertex)
		{
			pd.VSLength = length;
			pd.VSCode = new char[pd.VSLength];

			memcpy(pd.VSCode, data, pd.VSLength);
		}
		else if (type == SHDT_Pixel)
		{
			pd.PSLength = length;
			pd.PSCode = new char[pd.PSLength];

			memcpy(pd.PSCode, data, pd.PSLength);
		}
		else
		{
			pd.GSLength = length;
			pd.GSCode = new char[pd.GSLength];

			memcpy(pd.GSCode, data, pd.GSLength);
		}
	}

	void WriteCompileError(const std::string& errmsg, const String& sourceFile)
	{
		List<String> errs;
		StringUtils::Split(StringUtils::toPlatformWideString(errmsg), errs, L"\n\r");

		for (int32 i=0;i<errs.getCount();i++)
		{
			BuildSystem::LogError(errs[i], sourceFile);
		}
		
	}
	const char* getHLSLCompilerProfileName(EffectProfileData& pd, ShaderType type)
	{
		if (pd.MajorVer == 5 && pd.MinorVer == 0)
		{
			if (type == SHDT_Vertex)
				return "vs_5_0";
			else if (type == SHDT_Pixel)
				return "ps_5_0";
			else if (type == SHDT_Geometry)
				return "gs_5_0";
		}
		if (pd.MajorVer == 4 && pd.MinorVer == 0)
		{
			if (type == SHDT_Vertex)
				return "vs_4_0";
			else if (type == SHDT_Pixel)
				return "ps_4_0";
			else if (type == SHDT_Geometry)
				return "gs_4_0";
		}
		if (pd.MajorVer == 3 && pd.MinorVer == 0)
		{
			if (type == SHDT_Vertex)
				return "vs_3_0";
			else if (type == SHDT_Pixel)
				return "ps_3_0";
		}
		if (pd.MajorVer == 2 && pd.MinorVer == 0)
		{
			if (type == SHDT_Vertex)
				return "vs_2_0";
			else if (type == SHDT_Pixel)
				return "ps_2_0";
		}

		if (type == SHDT_Vertex)
			return "vs_1_0";
		else if (type == SHDT_Pixel)
			return "ps_1_1";

		return "";
	}

	bool ParseShaderProfileString(const String& profText, std::string& implType, int& majorVer, int& minorVer)
	{
		if (profText == L"sm_1_0")
		{
			majorVer = 1;
			minorVer = 0;
			implType = EffectProfileData::Imp_HLSL;
			return true;
		}
		else if (profText == L"sm_2_0")
		{
			majorVer = 2;
			minorVer = 0;
			implType = EffectProfileData::Imp_HLSL;
			return true;
		}
		else if (profText == L"sm_3_0")
		{
			majorVer = 3;
			minorVer = 0;
			implType = EffectProfileData::Imp_HLSL;
			return true;
		}
		else if (profText == L"sm_4_0")
		{
			majorVer = 4;
			minorVer = 0;
			implType = EffectProfileData::Imp_HLSL;
			return true;
		}
		else if (profText == L"sm_5_0")
		{
			majorVer = 5;
			minorVer = 0;
			implType = EffectProfileData::Imp_HLSL;
			return true;
		}
		else if (profText == L"glsl_1_1")
		{
			majorVer = 1;
			minorVer = 1;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_1_2")
		{
			majorVer = 1;
			minorVer = 2;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_1_3")
		{
			majorVer = 1;
			minorVer = 3;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_1_4")
		{
			majorVer = 1;
			minorVer = 4;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_1_5")
		{
			majorVer = 1;
			minorVer = 5;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_3_3")
		{
			majorVer = 3;
			minorVer = 3;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_4_0")
		{
			majorVer = 4;
			minorVer = 0;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_4_1")
		{
			majorVer = 4;
			minorVer = 1;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_4_2")
		{
			majorVer = 4;
			minorVer = 2;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		else if (profText == L"glsl_4_3")
		{
			majorVer = 4;
			minorVer = 3;
			implType = EffectProfileData::Imp_GLSL;
			return true;
		}
		return false;
	}
}