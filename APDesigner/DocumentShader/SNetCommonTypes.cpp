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

#include "SNetCommonTypes.h"

namespace APDesigner
{
	void ShaderNetVaryingNode::Parse(ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");

		Stage = ShaderNetUtils::ParseVaryingStage(sect->getAttribute(L"Stage"));

		String strType = sect->getAttribute(L"Type");
		Type = ShaderNetUtils::ParseVaryingType(strType);
		if (Type == SNVT_OTHER)
		{
			CustomVaryingType = strType;
		}

	}

	void ShaderNetConstantNode::Parse(ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");

		String usage = sect->getAttribute(L"Usage");
		EffectParamUsage u = EffectParameter::ParseParamUsage(usage);
		Usage = u;
		if (Usage != EPUSAGE_Unknown)
		{
			CustomUsage = usage;
		}
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class EnumConvHelper
	{
	public:
		EnumConvHelper()
			: CastTable(50), InvCastTable(50),
			CastTable_SNVT(50), InvCastTable_SNVT(50)
		{
			AddPair(L"int1", ATOMDATA_Int1);
			AddPair(L"int2", ATOMDATA_Int2);
			AddPair(L"int3", ATOMDATA_Int3);
			AddPair(L"int4", ATOMDATA_Int4);
			
			AddPair(L"int2x1", ATOMDATA_Int2x1);
			AddPair(L"int2x2", ATOMDATA_Int2x2);
			AddPair(L"int2x3", ATOMDATA_Int2x3);
			AddPair(L"int2x4", ATOMDATA_Int2x4);

			AddPair(L"int3x1", ATOMDATA_Int3x1);
			AddPair(L"int3x2", ATOMDATA_Int3x2);
			AddPair(L"int3x3", ATOMDATA_Int3x3);
			AddPair(L"int3x4", ATOMDATA_Int3x4);

			AddPair(L"int4x1", ATOMDATA_Int4x1);
			AddPair(L"int4x2", ATOMDATA_Int4x2);
			AddPair(L"int4x3", ATOMDATA_Int4x3);
			AddPair(L"int4x4", ATOMDATA_Int4x4);


			AddPair(L"float1", ATOMDATA_Float1);
			AddPair(L"float2", ATOMDATA_Float2);
			AddPair(L"float3", ATOMDATA_Float3);
			AddPair(L"float4", ATOMDATA_Float4);

			AddPair(L"float2x1", ATOMDATA_Float2x1);
			AddPair(L"float2x2", ATOMDATA_Float2x2);
			AddPair(L"float2x3", ATOMDATA_Float2x3);
			AddPair(L"float2x4", ATOMDATA_Float2x4);

			AddPair(L"float3x1", ATOMDATA_Float3x1);
			AddPair(L"float3x2", ATOMDATA_Float3x2);
			AddPair(L"float3x3", ATOMDATA_Float3x3);
			AddPair(L"float3x4", ATOMDATA_Float3x4);

			AddPair(L"float4x1", ATOMDATA_Float4x1);
			AddPair(L"float4x2", ATOMDATA_Float4x2);
			AddPair(L"float4x3", ATOMDATA_Float4x3);
			AddPair(L"float4x4", ATOMDATA_Float4x4);

			// need to support short form
			CastTable.Add(L"int", ATOMDATA_Int1);
			CastTable.Add(L"float", ATOMDATA_Float1);

			// =======================================================

			AddPair(L"COLOR0",SNVT_COLOR0);
			AddPair(L"COLOR1",SNVT_COLOR1);

			AddPair(L"TEXCOORD0",SNVT_TEXCOORD0);
			AddPair(L"TEXCOORD1",SNVT_TEXCOORD1);
			AddPair(L"TEXCOORD2",SNVT_TEXCOORD2);
			AddPair(L"TEXCOORD3",SNVT_TEXCOORD3);
			AddPair(L"TEXCOORD4",SNVT_TEXCOORD4);
			AddPair(L"TEXCOORD5",SNVT_TEXCOORD5);
			AddPair(L"TEXCOORD6",SNVT_TEXCOORD6);
			AddPair(L"TEXCOORD7",SNVT_TEXCOORD7);

			AddPair(L"DEPTH0",SNVT_DEPTH0);
			AddPair(L"VFACE",SNVT_VFACE);

			AddPair(L"POSITION0",SNVT_POSITION0);
			AddPair(L"POSITION1",SNVT_POSITION1);
			AddPair(L"POSITIONT",SNVT_POSITIONT);

			AddPair(L"NORMAL0",SNVT_NORMAL0);
			AddPair(L"NORMAL1",SNVT_NORMAL1);

			AddPair(L"BINORMAL0",SNVT_BINORMAL0);
			AddPair(L"BINORMAL1",SNVT_BINORMAL1);

			AddPair(L"TANGENT0",SNVT_TANGENT0);
			AddPair(L"TANGENT1",SNVT_TANGENT1);

			AddPair(L"PSIZE0",SNVT_PSIZE0);
			AddPair(L"PSIZE1",SNVT_PSIZE1);

			AddPair(L"BLENDINDICES0",SNVT_BLENDINDICES0);
			AddPair(L"BLENDINDICES1",SNVT_BLENDINDICES1);
			AddPair(L"BLENDWEIGHT0",SNVT_BLENDWEIGHT0);
			AddPair(L"BLENDWEIGHT1",SNVT_BLENDWEIGHT1);

			AddPair(L"OTHER",SNVT_OTHER);

		}

		HashMap<String, ShaderAtomDataFormat> CastTable;
		HashMap<int, String> InvCastTable;

		HashMap<String, ShaderNetVaryingType> CastTable_SNVT;
		HashMap<int, String> InvCastTable_SNVT;


	private:
		void AddPair(const String& name, ShaderAtomDataFormat usage)
		{
			CastTable.Add(name, usage);
			InvCastTable.Add((int)usage, name);
		}
		void AddPair(const String& name, ShaderNetVaryingType type)
		{
			CastTable_SNVT.Add(name, type);
			InvCastTable_SNVT.Add((int)type, name);
		}
	} static EnumConverter;


	ShaderAtomDataFormat ShaderNetUtils::ParseAtomDataFormat(const String& value)
	{
		ShaderAtomDataFormat result;
		if (EnumConverter.CastTable.TryGetValue(value, result))
			return result;
		return ATOMDATA_Other;
	}

	String ShaderNetUtils::ToString(ShaderAtomDataFormat value)
	{
		String result;
		if (EnumConverter.InvCastTable.TryGetValue(value, result))
		{
			return result;
		}

		return L"unknown";
	}

	ShaderNetVaryingStage ShaderNetUtils::ParseVaryingStage(const String& value)
	{
		String v = value;
		StringUtils::ToLowerCase(v);

		if (v==L"prevs")
		{
			return SNVStage_PreVS;
		}else if (v==L"preps")
		{
			return SNVStage_PrePS;
		}else if (v==L"final")
		{
			return SNVStage_Final;
		}
		return SNVStage_Final;
	}
	String ShaderNetUtils::ToString(ShaderNetVaryingStage value)
	{
		switch (value)
		{
		case SNVStage_PrePS:
			return L"PrePS";
		case SNVStage_PreVS:
			return L"PreVS";
		case SNVStage_Final:
			return L"Final";
		}
		return L"Final";
	}

	ShaderNetVaryingType ShaderNetUtils::ParseVaryingType(const String& value)
	{
		String v = value;
		StringUtils::ToLowerCase(v);
		ShaderNetVaryingType result;
		if (EnumConverter.CastTable_SNVT.TryGetValue(value, result))
			return result;
		return SNVT_OTHER;
	}
	String ShaderNetUtils::ToString(ShaderNetVaryingType value)
	{
		String result;
		if (EnumConverter.InvCastTable_SNVT.TryGetValue(value, result))
		{
			return result;
		}

		return L"OTHER";
	}
}