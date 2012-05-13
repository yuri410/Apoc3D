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

#include "SNetCommonTypes.h"

#include "Config/ConfigurationSection.h"

using namespace Apoc3D::Config;

namespace APDesigner
{
	void ShaderNetInputNode::Parse(ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");
	}

	void ShaderNetOutputNode::Parse(ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");
	}

	class AtomDataFormatConvHelper
	{
	public:
		AtomDataFormatConvHelper()
			: CastTable(50, &m_comparer1), InvCastTable(50, &m_comparer2)
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
		}

		StringEuqlityComparer m_comparer1;
		Int32EqualityComparer m_comparer2;

		FastMap<String, ShaderAtomDataFormat> CastTable;
		FastMap<int, String> InvCastTable;
	private:
		void AddPair(const String& name, ShaderAtomDataFormat usage)
		{
			CastTable.Add(name, usage);
			InvCastTable.Add((int)usage, name);
		}
	};
	static AtomDataFormatConvHelper ShaderAtomDataFormatConverter;


	ShaderAtomDataFormat ShaderNetUtils::Parse(const String& value)
	{
		ShaderAtomDataFormat result;
		if (ShaderAtomDataFormatConverter.CastTable.TryGetValue(value, result))
			return result;
		return ATOMDATA_Other;
	}

	String ShaderNetUtils::ToString(ShaderAtomDataFormat value)
	{
		String result;
		if (ShaderAtomDataFormatConverter.InvCastTable.TryGetValue(value, result))
		{
			return result;
		}

		return L"unknown";
	}
}