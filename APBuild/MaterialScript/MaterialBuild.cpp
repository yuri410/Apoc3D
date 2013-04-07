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
#include "MaterialBuild.h"

#include "apoc3d/Math/Color.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/IOLib/Streams.h"

#include "../CompileLog.h"
#include "../BuildEngine.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::Graphics::EffectSystem;

namespace APBuild
{
	class Pallet
	{
	public:
		String Name;
		FastList<Color4> Colors;
	};

	Color4 ResolveColor4(const String& text, const HashMap<String, Pallet*>& pallets);
	void ParseMaterialTree(HashMap<String, MaterialData*>& table, const MaterialData* baseMtrl, const String& baseMtrlName, const ConfigurationSection* sect, const HashMap<String, Pallet*>& pallets);
	void ParseMaterialCustomParams(MaterialData& data, const String& value, const HashMap<String, Pallet*>& pallets);

	void MaterialBuild::Build(ConfigurationSection* sect)
	{
		String srcFile = sect->getAttribute(L"SourceFile");
		String destination = sect->getAttribute(L"DestinationLocation");
		String desinationToken = sect->getAttribute(L"DestinationToken");


		EnsureDirectory(destination);

		FileLocation* fl = new FileLocation(srcFile);
		Configuration* config = XMLConfigurationFormat::Instance.Load(fl);

		ConfigurationSection* palSect = config->get(L"Pallet");

		HashMap<String, Pallet*> palColors;
		
		for (ConfigurationSection::SubSectionEnumerator e = palSect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			String palName = *e.getCurrentKey();

			Pallet* p = new Pallet();
			p->Name = palName;

			palColors.Add(palName, p);

			ConfigurationSection* subSect = *e.getCurrentValue();
			for (ConfigurationSection::SubSectionEnumerator e1 = subSect->GetSubSectionEnumrator(); e1.MoveNext();)
			{
				Color4 r = ResolveColor4((*e1.getCurrentValue())->getValue(), palColors);
				p->Colors.Add(r);
			}

		}

		
		// inherit structure
		// every node is expected to be a material definition where mtrl attribs are as XML attribs
		ConfigurationSection* mSect = config->get(L"Materials");
		HashMap<String, MaterialData*> mtrlTable;

		for (ConfigurationSection::SubSectionEnumerator e = mSect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(mtrlTable, 0, L"", *e.getCurrentValue(), palColors);
		}

		delete config;
		delete fl;

		Configuration* tokenFile = new Configuration(L"MtrlToken");
		for (HashMap<String, MaterialData*>::Enumerator e = mtrlTable.GetEnumerator();e.MoveNext();)
		{
			ConfigurationSection* s = new ConfigurationSection(*e.getCurrentKey());
			tokenFile->Add(s);
		}
		//tokenFile->Save(desinationToken);
		XMLConfigurationFormat::Instance.Save(tokenFile, new FileOutStream(desinationToken));
		delete tokenFile;

		CompileLog::WriteInformation(desinationToken, L">");


		for (HashMap<String, MaterialData*>::Enumerator e = mtrlTable.GetEnumerator();e.MoveNext();)
		{
			String destPath = PathUtils::Combine(destination, *e.getCurrentKey());
			destPath.append(L".mtrl");

			MaterialData* md = *e.getCurrentValue();
			FileOutStream* fos = new FileOutStream(destPath);
			md->Save(fos);

			delete md;
			CompileLog::WriteInformation(*e.getCurrentKey(), L">");
		}
		mtrlTable.Clear();

		for (HashMap<String, Pallet*>::Enumerator e = palColors.GetEnumerator();e.MoveNext();)
		{
			delete *e.getCurrentValue();
		}
		palColors.Clear();
	}




	void ParseMaterialTree(HashMap<String, MaterialData*>& table, const MaterialData* baseMtrl, const String& baseMtrlName, const ConfigurationSection* sect, const HashMap<String, Pallet*>& pallets)
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

		sect->TryGetAttributeBool(L"UsePointSprite", newNode->UsePointSprite);
		sect->tryGetAttribute(L"ExternalRefName", newNode->ExternalRefName);
		sect->TryGetAttributeInt(L"Priority", newNode->Priority);
		sect->TryGetAttributeBool(L"IsBlendTransparent", newNode->IsBlendTransparent);
		sect->TryGetAttributeBool(L"AlphaTestEnabled", newNode->AlphaTestEnabled);
		sect->TryGetAttributeUInt(L"AlphaReference", newNode->AlphaReference);
		sect->TryGetAttributeBool(L"DepthWriteEnabled", newNode->DepthWriteEnabled);
		sect->TryGetAttributeBool(L"DepthTestEnabled", newNode->DepthTestEnabled);

		if (sect->tryGetAttribute(L"PassFlag", temp))
			newNode->PassFlags = StringUtils::ParseUInt64Bin(temp);
		if (sect->tryGetAttribute(L"SourceBlend", temp))		newNode->SourceBlend = GraphicsCommonUtils::ParseBlend(temp);
		if (sect->tryGetAttribute(L"DestinationBlend", temp))	newNode->DestinationBlend = GraphicsCommonUtils::ParseBlend(temp);
		if (sect->tryGetAttribute(L"BlendFunction", temp))		newNode->BlendFunction = GraphicsCommonUtils::ParseBlendFunction(temp);
		if (sect->tryGetAttribute(L"Cull", temp))				newNode->Cull = GraphicsCommonUtils::ParseCullMode(temp);

		if (sect->tryGetAttribute(L"Ambient", temp))			newNode->Ambient = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Diffuse", temp))			newNode->Diffuse = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Emissive", temp))			newNode->Emissive = ResolveColor4(temp, pallets);
		if (sect->tryGetAttribute(L"Specular", temp))			newNode->Specular = ResolveColor4(temp, pallets);
		sect->TryGetAttributeSingle(L"Power", newNode->Power);

		
		if (sect->tryGetAttribute(L"Texture1", temp)) 
		{
			if (newNode->TextureName.Contains(0))
				newNode->TextureName[0] = temp;
			else
				newNode->TextureName.Add(0, temp);
		}
		if (sect->tryGetAttribute(L"Texture2", temp))
		{
			if (newNode->TextureName.Contains(1))
				newNode->TextureName[1] = temp;
			else
				newNode->TextureName.Add(1, temp);
		}
		if (sect->tryGetAttribute(L"Texture3", temp))
		{
			if (newNode->TextureName.Contains(2))
				newNode->TextureName[2] = temp;
			else
				newNode->TextureName.Add(2, temp);
		}
		if (sect->tryGetAttribute(L"Texture4", temp))
		{
			if (newNode->TextureName.Contains(3))
				newNode->TextureName[3] = temp;
			else
				newNode->TextureName.Add(3, temp);
		}

		String customString;
		if (sect->tryGetAttribute(L"Custom", customString))
		{
			ParseMaterialCustomParams(*newNode, customString, pallets);
		}

		String effString;
		if (sect->tryGetAttribute(L"Effect",effString))
		{
			List<String> defs;
			StringUtils::Split(effString, defs, L";");

			for (int32 i=0;i<defs.getCount();i++)
			{
				List<String> lr;
				StringUtils::Split(defs[i], lr, L":");

				int ord = StringUtils::ParseInt32(lr[0].substr(1));
				String name = lr[1];

				if (!newNode->EffectName.Contains(ord-1))
					newNode->EffectName.Add(ord-1, name);
				else
					newNode->EffectName[ord-1] = name;
			}
		}



		// go into sub sections
		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(table, newNode, name, *e.getCurrentValue(), pallets);
		}

		table.Add(name, newNode);
	}

	void ParseMaterialCustomParams(MaterialData& data, const String& value, const HashMap<String, Pallet*>& pallets)
	{
		List<String> vals;
		StringUtils::Split(value, vals, L";");
		for (int32 i=0;i<vals.getCount();i++)
		{
			List<String> vals2;
			StringUtils::Split(vals[i], vals2, L"=");

			String& usageName = vals2[0];
			String& valueStr = vals2[1];
			MaterialCustomParameter mcp;
			mcp.Usage = usageName;

			memset(mcp.Value, 0, sizeof(mcp.Value));

			if (StringUtils::StartsWidth(valueStr, L"(") && StringUtils::EndsWidth(valueStr, L")"))
			{
				String vec = valueStr.substr(1, valueStr.length()-2);
				List<String> vals3;
				StringUtils::Split(vals[i], vals3, L",");	
				
				assert(vals3.getCount() == 2 || vals3.getCount()==4);

				if (vals3.getCount() == 2)
				{
					mcp.Type = CEPT_Vector2;
					float data[2] = { StringUtils::ParseSingle(vals3[0]), StringUtils::ParseSingle(vals3[1]) };
					memcpy(mcp.Value, data, sizeof(data));
				}
				else
				{
					mcp.Type = CEPT_Vector4;
					float data[4] = 
					{
						StringUtils::ParseSingle(vals3[0]), StringUtils::ParseSingle(vals3[1]),
						StringUtils::ParseSingle(vals3[2]), StringUtils::ParseSingle(vals3[3])
					};
					memcpy(mcp.Value, data, sizeof(data));
				}
			}
			else
			{
				StringUtils::Trim(valueStr);
				//StringUtils::ToLowerCase(valueStr);

				if (valueStr==L"true" || valueStr == L"false")
				{
					mcp.Type = CEPT_Boolean;
					mcp.Value[0] = valueStr == L"true" ? 1 : 0;
				}
				else
				{
					String::size_type pos = valueStr.find('.');
					if (pos != String::npos)
					{
						mcp.Type = CEPT_Float;
						float data = StringUtils::ParseSingle(valueStr);
						memcpy(mcp.Value, &data, sizeof(data));
					}
					else
					{
						// check if the value str is numerical 
						bool isNumber = true;
						for (size_t i=0;i<valueStr.size();i++)
						{
							if (i==0 && valueStr[i] == '-')
							{
								continue;
							}
							if (valueStr[i] <'0' || valueStr[i] > '9' || valueStr[i] != ' ')
							{
								isNumber = false;
							}
						}

						if (isNumber)
						{
							mcp.Type = CEPT_Integer;
							int data = StringUtils::ParseInt32(valueStr);
							memcpy(mcp.Value, &data, sizeof(data));
						}
						else
						{
							mcp.Type = CEPT_Vector4;
							Color4 v = ResolveColor4(valueStr, pallets);
							memcpy(mcp.Value, &v, sizeof(v));
						}
						
					}
				}
				
			}

			data.CustomParametrs.Add(mcp.Usage, mcp);
		}
	}

	Color4 ResolveColor4(const String& text, const HashMap<String, Pallet*>& pallets)
	{
		bool hasOperation = text.find('*') != String::npos;

		if (hasOperation)
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
					// now channel operation needs to be done
					String::size_type mPos = text.find_first_of('*');
					String leftPart = text.substr(0, mPos);

					String::size_type ldotPos = leftPart.find_first_of('.');

					byte channelMask = 0;

					if (ldotPos != String::npos)
					{
						String chns = leftPart.substr(ldotPos + 1);

						StringUtils::Trim(chns);
						StringUtils::ToLowerCase(chns);
						for (size_t i=0;i<chns.size();i++)
						{
							switch (chns[i])
							{
							case 'r':
								channelMask |= 8;
								break;
							case 'g':
								channelMask |= 4;
								break;
							case 'b':
								channelMask |= 2;
								break;
							case 'a':
								channelMask |= 1;
								break;
							}
						}
					}
					else
					{
						channelMask = 1+2+4+8;// 1111
					}

					float factor[4] = {1.0f,1.0f,1.0f,1.0f};

					String rightPart = text.substr(mPos +1);
					StringUtils::Trim(rightPart);

					if (StringUtils::StartsWidth(rightPart, L"(") && StringUtils::EndsWidth(rightPart, L")"))
					{
						String facts = rightPart.substr(1, rightPart.size()-2);
						List<String> vals;
						StringUtils::Split(facts, vals, L",");
						assert(vals.getCount()<=4);
						for (int32 i=0;i<vals.getCount();i++)
							factor[i] = StringUtils::ParseSingle(vals[i]);
					}
					else
					{
						float fact = StringUtils::ParseSingle(rightPart);
						for (int i=0;i<4;i++) factor[i] = fact;
					}

					Color4 source = result->Colors[index];
					if ((channelMask & 8) == 8)
						source.Red *= factor[0];
					if ((channelMask & 4) == 4)
						source.Green *= factor[1];
					if ((channelMask & 2) == 2)
						source.Blue *= factor[2];
					if ((channelMask & 1) == 1)
						source.Alpha *= factor[3];
					return source;
				}
				else
					CompileLog::WriteError(String(L"Pallet not found ") + palName, L"");
			}
			
			CompileLog::WriteError(String(L"Cannot parse ") + text, L"");
			return Color4();
		}
		
		List<String> vals;
		StringUtils::Split(text, vals, L",");

		switch (vals.getCount())
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