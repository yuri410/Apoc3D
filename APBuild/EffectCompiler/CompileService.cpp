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

#include "CompileService.h"

#include "BuildConfig.h"
#include "BuildSystem.h"

#include "CompilerService_SM3.h"

//#include "Library/hlslparser/"

#include "Library/hlslparser/GLSLGenerator.h"
#include "Library/hlslparser/HLSLParser.h"
#include "Library/hlslparser/HLSLTree.h"

#include <dxsdk/d3d9.h>
#include <dxsdk/d3dx9.h>
#include <dxsdk/d3d11.h>
#include <dxsdk/d3dx11.h>
#include <dxsdk/d3dcompiler.h>

namespace APBuild
{
	const char* getHLSLCompilerProfileName(EffectProfileData& pd, ShaderType type);
	const char* getHLSLCompilerProfileNameFromGLSLVersion(EffectProfileData& pd, ShaderType type);

	void WriteCompileError(const std::string& logs, const String& sourceFile);
	void FillEffectProfileCodeData(EffectProfileData& pd, const char* data, const int32 length, ShaderType type);

	void ParseEffectParameter(const String& srcFile, const List<String>& semantics, EffectParameter& ep);

	//////////////////////////////////////////////////////////////////////////

	void ParseEffectParameter(const String& srcFile, const List<String>& semantics, EffectParameter& ep)
	{
		int32 validSemanticCount = 0;
	
		String addrU, addrV, addrW;
		String borderColor;
		String magflt, minflt, mipflt;
		String lodbias, maxMip, maxAnis;
		String bidText;
		
		struct { const wchar_t* source; String& target; } const fieldInfo[] =
		{
			{ L"def_tex_", ep.DefaultTextureName },
			{ L"ss_grp_", ep.SamplerStateOverridenGroupName },
			{ L"mtrl_param_", ep.CustomMaterialParamName },

			{ L"bid_", bidText },

			{ L"ss_addu_", addrU },
			{ L"ss_addv_", addrV },
			{ L"ss_addw_", addrW },

			{ L"ss_bordercolor_", borderColor },

			{ L"ss_mag_", magflt },
			{ L"ss_min_", minflt },
			{ L"ss_mip_", mipflt },

			{ L"ss_lodbias_", lodbias },
			{ L"ss_maxmip_", maxMip },
			{ L"ss_maxanis_", maxAnis }
		};

		for (const String& usageText : semantics)
		{
			for (auto& e : fieldInfo)
			{
				if (StringUtils::StartsWith(usageText, e.source, true))
				{
					e.target = usageText.substr(wcslen(e.source));
					validSemanticCount++;
				}
			}
		}

		if (bidText.size())
		{
			ep.Usage = EPUSAGE_InstanceBlob;
			ep.InstanceBlobIndex = StringUtils::ParseInt32(bidText);
		}
		else if (ep.DefaultTextureName.size())
		{
			ep.DefaultTextureName += L".tex";
			ep.Usage = EPUSAGE_DefaultTexture;
		}
		else if (ep.CustomMaterialParamName.size())
		{
			ep.Usage = EPUSAGE_CustomMaterialParam;
		}

		for (int32 i = 0; i < semantics.getCount(); i++)
		{
			const String& usageText = semantics[i];

			if (ep.Usage == EPUSAGE_Unknown)
			{
				if (!ep.SupportsParamUsage(usageText))
					break;

				ep.Usage = EffectParameter::ParseParamUsage(usageText);

				validSemanticCount++;
			}
		}


		struct { String& src; TextureAddressMode& target; } const addrFields[] =
		{
			{ addrU, ep.SamplerState.AddressU },
			{ addrV, ep.SamplerState.AddressV },
			{ addrW, ep.SamplerState.AddressW },
		};

		for (auto& e : addrFields)
		{
			if (e.src.size() && !TextureAddressModeConverter.TryParse(e.src, e.target))
				BuildSystem::LogWarning(srcFile, L"Invalid address mode " + e.src + L" for param " + ep.Name);
		}

		struct { String& src; TextureFilter& target; } const fltFields[] =
		{
			{ magflt, ep.SamplerState.MagFilter },
			{ minflt, ep.SamplerState.MinFilter },
			{ mipflt, ep.SamplerState.MipFilter },
		};

		for (auto& e : fltFields)
		{
			if (e.src.size() && !TextureFilterConverter.TryParse(e.src, e.target))
				BuildSystem::LogWarning(srcFile, L"Invalid texture filtering " + e.src + L" for param " + ep.Name);
		}

		if (borderColor.size()) ep.SamplerState.BorderColor = StringUtils::ParseUInt32Hex(borderColor);

		if (lodbias.size()) ep.SamplerState.MipMapLODBias = StringUtils::ParseInt32(lodbias);
		if (maxMip.size()) ep.SamplerState.MaxMipLevel = StringUtils::ParseInt32(maxMip);
		if (maxAnis.size()) ep.SamplerState.MaxAnisotropy = StringUtils::ParseInt32(maxAnis);


		if (ep.Usage == EPUSAGE_Unknown && validSemanticCount < semantics.getCount())
		{
			BuildSystem::LogWarning(srcFile, 
				StringUtils::IntToString(semantics.getCount() - validSemanticCount) + L" invalid semantic(s) found for param " + ep.Name);
		}
	}

	void ParseEffectParametersFromHLSL(const String& srcFile, ShaderType type, EffectProfileData& pd, M4::HLSLTree& tree, const List<String>& allNames)
	{
		M4::Allocator allocator;
		M4::Array<M4::HLSLDeclaration*> decls(&allocator);

		tree.FillGlobalDeclaration(decls);

		for (int32 i = 0; i < decls.GetSize();i++)
		{
			M4::HLSLDeclaration* d = decls[i];

			while (d)
			{
				//if (d->semanticCount>0)
				{
					String name = StringUtils::toPlatformWideString(d->name);

					if (allNames.Contains(name))
					{
						List<String> semantics;

						if (d->semanticCount>0)
						{
							semantics.ResizeDiscard(d->semanticCount);

							for (int32 j = 0; j < d->semanticCount; j++)
								semantics.Add(StringUtils::toPlatformWideString(d->semantic[j]));
						}
						
						EffectParameter ep(name);
						ep.ProgramType = type;
						
						ParseEffectParameter(srcFile, semantics, ep);

						pd.Parameters.Add(ep);
					}
				}
				d = d->nextDeclaration;
			}
		}
	}


	bool CompileShader(const String& src, const String& entryPoint, EffectProfileData& profData, ShaderType type, 
		bool debugEnabled, bool noOptimization, bool parseParamsFromSource, const List<std::pair<std::string, std::string>>* defines)
	{
		List<std::pair<std::string, std::string>> definesCopy;
		if (defines)
			definesCopy = *defines;
		
		//switch (type)
		//{
		//	case ShaderType::Vertex:
		//		definesCopy.Add({ "VS", "1" });
		//		break;
		//	case ShaderType::Pixel:
		//		definesCopy.Add({ "PS", "1" });
		//		break;
		//	case ShaderType::Geometry:
		//		definesCopy.Add({ "GS", "1" });
		//		break;
		//}

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
				if (definesCopy.getCount() > 0)
				{
					macros = new D3D10_SHADER_MACRO[definesCopy.getCount() + 1];
					memset(macros, 0, (definesCopy.getCount() + 1) * sizeof(D3D10_SHADER_MACRO));

					for (int32 i = 0; i < definesCopy.getCount(); i++)
					{
						macros[i].Name = definesCopy[i].first.c_str();
						macros[i].Definition = definesCopy[i].second.c_str();
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

				FillEffectProfileCodeData(profData, (const char*)pBlobOut->GetBufferPointer(), (int)pBlobOut->GetBufferSize(), type);

				pBlobOut->Release();
				return true;
			}
			else
			{
				ConstantTable* constantHelper;
				char* newShaderCode;
				int newShaderCodeSize;

				if (!CompileAsHLSLDX9(src, entryPoint, pfName, debugEnabled, noOptimization, definesCopy, constantHelper, newShaderCode, newShaderCodeSize))
					return false;

				if (parseParamsFromSource)
				{
					std::string ncode;
					PreprocessShaderCode(src, definesCopy, ncode);

					std::string codeFileName = StringUtils::toPlatformNarrowString(PathUtils::GetFileName(src));

					M4::Allocator allocator;
					M4::HLSLTree tree(&allocator);
					M4::HLSLParser parser(&allocator, codeFileName.c_str(), ncode.c_str(), ncode.length());

					if (!parser.Parse(&tree))
					{
						WriteCompileError("Parse error.\n", src);
						return false;
					}

					List<String> usedParams;
					constantHelper->GetAllConstantNames(usedParams);
					ParseEffectParametersFromHLSL(src, type, profData, tree, usedParams);
				}

				FillEffectProfileCodeData(profData, (const char*)newShaderCode, newShaderCodeSize, type);

				delete constantHelper;
				delete[] newShaderCode;

				return true;
			}
		}
		else if (profData.MatchImplType(EffectProfileData::Imp_GLSL))
		{
			if (profData.MajorVer == 1 && profData.MinorVer < 5)
			{
				if (type != ShaderType::Pixel && type != ShaderType::Vertex)
				{
					WriteCompileError("Shader type not supported under glsl_1_4.", src);
					return false;
				}

				std::string ncode;
				PreprocessShaderCode(src, definesCopy, ncode);

				std::string codeFileName = StringUtils::toPlatformNarrowString(PathUtils::GetFileName(src));

				M4::Allocator allocator;
				M4::HLSLTree tree(&allocator);
				M4::HLSLParser parser(&allocator, codeFileName.c_str(), ncode.c_str(), ncode.length());

				if (!parser.Parse(&tree)) 
				{
					WriteCompileError("Parse error.\n", src);
					return false;
				}
				
				if (parseParamsFromSource)
				{
					// compile as hlsl to get constant table
					ConstantTable* constantHelper;
					char* newShaderCode;
					int newShaderCodeSize;

					const char* pfName = getHLSLCompilerProfileNameFromGLSLVersion(profData, type);

					if (!CompileAsHLSLDX9(src, entryPoint, pfName, debugEnabled, noOptimization, definesCopy, constantHelper, newShaderCode, newShaderCodeSize))
						return false;

					List<String> usedParams;
					constantHelper->GetAllConstantNames(usedParams);
					ParseEffectParametersFromHLSL(src, type, profData, tree, usedParams);

					delete constantHelper;
					delete[] newShaderCode;
				}

				M4::GLSLGenerator generator(&allocator);
				if (!generator.Generate(&tree, type == ShaderType::Vertex ? M4::GLSLGenerator::Target_VertexShader : M4::GLSLGenerator::Target_FragmentShader,
					StringUtils::toPlatformNarrowString(entryPoint).c_str()))
				{
					WriteCompileError("Generation error.\n", src);
					return false;
				}

				std::string glslCode = generator.GetResult();

				int   codeLength = (int)glslCode.size();
				int   compressedSize = LZ4_compressBound(codeLength);
				char* compressedData = new char[compressedSize + sizeof(int)*2];

				compressedSize = LZ4_compressHC2(glslCode.c_str(), compressedData + sizeof(int)*2, codeLength, 16);

				i32_mb_le(compressedSize, compressedData);
				i32_mb_le(codeLength, compressedData + sizeof(int));

				FillEffectProfileCodeData(profData, (const char*)compressedData, compressedSize, type);

				delete[] compressedData;

				return true;
			}
			else
			{
				WriteCompileError("Profile not supported.", src);
			}
		}

		WriteCompileError("Profile not supported.", src);
		return false;
	}

	
	void FillEffectProfileCodeData(EffectProfileData& pd, const char* data, const int32 length, ShaderType type)
	{
		if (type == ShaderType::Vertex)
		{
			pd.VSLength = length;
			pd.VSCode = new char[pd.VSLength];

			memcpy(pd.VSCode, data, pd.VSLength);
		}
		else if (type == ShaderType::Pixel)
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

		for (int32 i = 0; i < errs.getCount(); i++)
		{
			BuildSystem::LogError(errs[i], sourceFile);
		}
	}

	const char* getHLSLCompilerProfileName(EffectProfileData& pd, ShaderType type)
	{
		if (pd.MajorVer == 5 && pd.MinorVer == 0)
		{
			if (type == ShaderType::Vertex)
				return "vs_5_0";
			else if (type == ShaderType::Pixel)
				return "ps_5_0";
			else if (type == ShaderType::Geometry)
				return "gs_5_0";
		}
		if (pd.MajorVer == 4 && pd.MinorVer == 0)
		{
			if (type == ShaderType::Vertex)
				return "vs_4_0";
			else if (type == ShaderType::Pixel)
				return "ps_4_0";
			else if (type == ShaderType::Geometry)
				return "gs_4_0";
		}
		if (pd.MajorVer == 3 && pd.MinorVer == 0)
		{
			if (type == ShaderType::Vertex)
				return "vs_3_0";
			else if (type == ShaderType::Pixel)
				return "ps_3_0";
		}
		if (pd.MajorVer == 2 && pd.MinorVer == 0)
		{
			if (type == ShaderType::Vertex)
				return "vs_2_0";
			else if (type == ShaderType::Pixel)
				return "ps_2_0";
		}

		if (type == ShaderType::Vertex)
			return "vs_1_0";
		else if (type == ShaderType::Pixel)
			return "ps_1_1";

		return "";
	}
	
	const char* getHLSLCompilerProfileNameFromGLSLVersion(EffectProfileData& pd, ShaderType type)
	{
		if (pd.MajorVer >= 4 && pd.MinorVer >= 2)
		{
			if (type == ShaderType::Vertex)
				return "vs_5_0";
			else if (type == ShaderType::Pixel)
				return "ps_5_0";
			else if (type == ShaderType::Geometry)
				return "gs_5_0";
		}
		else if (pd.MajorVer >= 3 && pd.MinorVer >= 3)
		{
			if (type == ShaderType::Vertex)
				return "vs_4_0";
			else if (type == ShaderType::Pixel)
				return "ps_4_0";
			else if (type == ShaderType::Geometry)
				return "gs_4_0";
		}
		else if (pd.MajorVer >= 1 && pd.MinorVer >= 3)
		{
			if (type == ShaderType::Vertex)
				return "vs_3_0";
			else if (type == ShaderType::Pixel)
				return "ps_3_0";
		}
		else
		{
			if (type == ShaderType::Vertex)
				return "vs_2_0";
			else if (type == ShaderType::Pixel)
				return "ps_2_0";
		}
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
		else if (profText == L"sm_4_1")
		{
			majorVer = 4;
			minorVer = 1;
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