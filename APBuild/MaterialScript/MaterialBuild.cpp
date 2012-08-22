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
#include "MaterialBuild.h"
#include "Math/Color.h"
#include "Collections/FastList.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"
#include "Utility/StringUtils.h"
#include "IOLib/MaterialData.h"
#include "IOLib/Streams.h"
#include "CompileLog.h"
#include "BuildEngine.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;

namespace APBuild
{
	class Pallet
	{
	public:
		String Name;
		FastList<Color4> Colors;
	};

	Color4 ResolveColor4(const String& text, const FastMap<String, Pallet*>& pallets);
	void ParseMaterialTree(FastMap<String, MaterialData*>& table, MaterialData* baseMtrl, const String& baseMtrlName, const ConfigurationSection* sect, const FastMap<String, Pallet*>& pallets);

	void MaterialBuild::Build(ConfigurationSection* sect)
	{
		String srcFile = sect->getAttribute(L"SourceFile");
		String destination = sect->getAttribute(L"DestinationLocation");
		String desinationToken = sect->getAttribute(L"DestinationToken");


		EnsureDirectory(PathUtils::GetDirectory(destination));

		FileLocation* fl = new FileLocation(srcFile);
		XMLConfiguration* config = new XMLConfiguration(fl);

		ConfigurationSection* palSect = config->get(L"Pallet");

		FastMap<String, Pallet*> palColors;
		
		for (ConfigurationSection::SubSectionEnumerator e = palSect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			String palName = *e.getCurrentKey();

			Pallet* p = new Pallet();
			p->Name = palName;
			
			ConfigurationSection* subSect = *e.getCurrentValue();
			for (ConfigurationSection::SubSectionEnumerator e1 = subSect->GetSubSectionEnumrator(); e1.MoveNext();)
			{
				std::vector<String> vals = StringUtils::Split((*e1.getCurrentValue())->getValue(), L",");

				assert(vals.size() == 3 || vals.size() == 4);

				if (vals.size() == 4)
				{
					p->Colors.Add(Color4(
						StringUtils::ParseInt32(vals[0]),
						StringUtils::ParseInt32(vals[1]),
						StringUtils::ParseInt32(vals[2]), 
						StringUtils::ParseInt32(vals[3])));
				}
				else
				{
					p->Colors.Add(Color4(StringUtils::ParseInt32(vals[0]),StringUtils::ParseInt32(vals[1]),StringUtils::ParseInt32(vals[2])));
				}
			}

			palColors.Add(palName, p);
		}

		
		// inherit structure
		// every node is expected to be a material definition where mtrl attribs are as XML attribs
		ConfigurationSection* mSect = config->get(L"Materials");
		FastMap<String, MaterialData*> mtrlTable;

		for (ConfigurationSection::SubSectionEnumerator e = mSect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(mtrlTable, 0, L"", *e.getCurrentValue(), palColors);
		}

		delete config;
		delete fl;

		XMLConfiguration* tokenFile = new XMLConfiguration(L"MtrlToken");
		for (FastMap<String, MaterialData*>::Enumerator e = mtrlTable.GetEnumerator();e.MoveNext();)
		{
			ConfigurationSection* s = new ConfigurationSection(*e.getCurrentKey());
			tokenFile->Add(s);
		}
		tokenFile->Save(desinationToken);
		delete tokenFile;
		CompileLog::WriteInformation(desinationToken, L">");


		for (FastMap<String, MaterialData*>::Enumerator e = mtrlTable.GetEnumerator();e.MoveNext();)
		{
			String destPath = PathUtils::Combine(destination, *e.getCurrentKey());
			destPath.append(L".mtrl");

			MaterialData* md = *e.getCurrentValue();
			FileOutStream* fos = new FileOutStream(destPath);
			md->Save(fos);

			delete md;
			CompileLog::WriteInformation(desinationToken, L">");
		}
		mtrlTable.Clear();

		for (FastMap<String, Pallet*>::Enumerator e = palColors.GetEnumerator();e.MoveNext();)
		{
			delete *e.getCurrentValue();
		}
		palColors.Clear();
	}

	void ParseMaterialTree(FastMap<String, MaterialData*>& table, const MaterialData* baseMtrl, const String& baseMtrlName, const ConfigurationSection* sect, const FastMap<String, Pallet*>& pallets)
	{
		MaterialData* newNode;
		
		if (baseMtrl)
			newNode = new MaterialData(*baseMtrl);
		else
		{
			newNode = new MaterialData();
			newNode->SetDefaults();
		}
		// build a name
		String name = baseMtrlName;
		if (name.size())
		{
			name.append(L"_");
		}
		name.append(sect->getName());
		
		// parse attributes
		String temp;

		sect->tryGetAttribute(L"ExternalRefName", newNode->ExternalRefName);
		sect->TryGetAttributeInt(L"Priority", newNode->Priority);
		sect->TryGetAttributeBool(L"IsBlendTransparent", newNode->IsBlendTransparent);
		sect->TryGetAttributeBool(L"AlphaTestEnabled", newNode->AlphaTestEnabled);
		sect->TryGetAttributeUInt(L"AlphaReference", newNode->AlphaReference);
		sect->TryGetAttributeBool(L"DepthWriteEnabled", newNode->DepthWriteEnabled);
		sect->TryGetAttributeBool(L"DepthTestEnabled", newNode->DepthTestEnabled);

		if (sect->tryGetAttribute(L"PassFlags", temp))			newNode->PassFlags = StringUtils::ParseUInt64Bin(temp);
		if (sect->tryGetAttribute(L"SourceBlend", temp))		newNode->SourceBlend = GraphicsCommonUtils::ParseBlend(temp);
		if (sect->tryGetAttribute(L"DestinationBlend", temp))	newNode->DestinationBlend = GraphicsCommonUtils::ParseBlend(temp);
		if (sect->tryGetAttribute(L"BlendFunction", temp))		newNode->BlendFunction = GraphicsCommonUtils::ParseBlendFunction(temp);
		if (sect->tryGetAttribute(L"Cull", temp))				newNode->Cull = GraphicsCommonUtils::ParseCullMode(temp);

		if (sect->tryGetAttribute(L"Ambient", temp))			newNode->Ambient = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Diffuse", temp))			newNode->Diffuse = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Emissive", temp))			newNode->Emissive = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Specular", temp))			newNode->Specular = ResolveColor4(temp, pallets);
		sect->TryGetAttributeSingle(L"Power", newNode->Power);


		//newNode->SourceBlend = GraphicsCommonUtils::ParseBlend(sect->getAttribute(L"SourceBlend"));
		//newNode->DestinationBlend = GraphicsCommonUtils::ParseBlend(sect->getAttribute(L"DestinationBlend"));
		//newNode->BlendFunction = GraphicsCommonUtils::ParseBlendFunction(sect->getAttribute(L"BlendFunction"));
		//newNode->IsBlendTransparent = sect->GetAttributeBool(L"IsBlendTransparent");
		//newNode->Cull = GraphicsCommonUtils::ParseCullMode(sect->getAttribute(L"Cull"));
		//newNode->AlphaTestEnabled = sect->GetAttributeBool(L"AlphaTestEnabled");
		//newNode->AlphaReference = sect->GetAttributeUInt(L"AlphaReference");
		//newNode->DepthWriteEnabled = sect->GetAttributeBool(L"DepthWriteEnabled");
		//newNode->DepthTestEnabled = sect->GetAttributeBool(L"DepthTestEnabled");

		//newNode->Ambient = ResolveColor4(sect->getAttribute(L"Ambient"), pallets);
		//newNode->Diffuse = ResolveColor4(sect->getAttribute(L"Diffuse"), pallets);
		//newNode->Emissive = ResolveColor4(sect->getAttribute(L"Emissive"), pallets);
		//newNode->Specular = ResolveColor4(sect->getAttribute(L"Specular"), pallets);
		//newNode->Power = sect->GetAttributeSingle(L"Power");



		// go into sub sections
		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(table, newNode, name, *e.getCurrentValue(), pallets);
		}

		table.Add(name, newNode);
	}

	Color4 ResolveColor4(const String& text, const FastMap<String, Pallet*>& pallets)
	{
		std::vector<String> vals = StringUtils::Split(text, L",");

		switch (vals.size())
		{
		case 1:
			{
				String::size_type posL = text.find_first_of('[');
				String::size_type posR = text.find_first_of(']');

				if (posL != String::npos && posR != String::npos)
				{
					String palName = text.substr(0, posL);
					StringUtils::Trim(palName);

					String sIndx = text.substr(posL+1,posR-posL-1);
					int index = StringUtils::ParseInt32(sIndx);

					Pallet* result;
					if (pallets.TryGetValue(palName, result))
					{
						return result->Colors[index];
					}
					else
						CompileLog::WriteError(String(L"Pallet not found ") + palName, L"");
				}
				else
					CompileLog::WriteError(String(L"Cannot parse ") + text, L"");
			}
			return Color4();
		case 3:
			return Color4(
				StringUtils::ParseInt32(vals[0]),
				StringUtils::ParseInt32(vals[1]),
				StringUtils::ParseInt32(vals[2]));
		case 4:
			return Color4(
				StringUtils::ParseInt32(vals[0]),
				StringUtils::ParseInt32(vals[1]),
				StringUtils::ParseInt32(vals[2]), 
				StringUtils::ParseInt32(vals[3]));
		default:
			CompileLog::WriteError(String(L"Cannot parse ") + text, L"");
			return Color4();
		}
	}
}