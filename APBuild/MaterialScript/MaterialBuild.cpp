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

#include "BuildSystem.h"

namespace APBuild
{
	struct PalletColor
	{
		String Name;
		Color4 Color;
	};

	struct Pallet
	{
		String Name;
		List<PalletColor> Colors;

		const Color4* FindColor(const String& name) const;
	};

	bool hasLetters(const String& str);

	Color4 ResolveColor4(const String& text, const HashMap<String, Pallet*>& pallets);
	void ParseMaterialTree(HashMap<String, MaterialData*>& table, const MaterialData* baseMtrl, const String& baseMtrlName, const ConfigurationSection* sect, const HashMap<String, Pallet*>& pallets);
	void ParseMaterialCustomParams(MaterialData& data, const String& value, const HashMap<String, Pallet*>& pallets);

	void MaterialBuild::Build(const String& hierarchyPath, ConfigurationSection* sect)
	{
		String srcFile = sect->getAttribute(L"SourceFile");
		String destination = sect->getAttribute(L"DestinationLocation");
		String desinationToken = sect->getAttribute(L"DestinationToken");


		BuildSystem::EnsureDirectory(destination);

		Configuration config;
		XMLConfigurationFormat::Instance.Load(FileLocation(srcFile), &config);

		ConfigurationSection* palSect = config[L"Pallet"];

		HashMap<String, Pallet*> palColors;
		
		for (ConfigurationSection* subSect : palSect->getSubSections())
		{
			const String& palName = subSect->getName();

			Pallet* p = new Pallet();
			p->Name = palName;

			palColors.Add(palName, p);

			for (ConfigurationSection* ss : subSect->getSubSections())
			{
				PalletColor pc;

				pc.Color = ResolveColor4(ss->getValue(), palColors);
				pc.Name = ss->getName();
				p->Colors.Add(pc);
			}

		}

		
		// inherit structure
		// every node is expected to be a material definition where mtrl attribs are as XML attribs
		ConfigurationSection* mSect = config[L"Materials"];
		HashMap<String, MaterialData*> mtrlTable;

		for (ConfigurationSection* ss : mSect->getSubSections())
		{
			ParseMaterialTree(mtrlTable, 0, L"", ss, palColors);
		}


		Configuration tokenFile(L"MtrlToken");
		for (const String& key : mtrlTable.getKeyAccessor())
		{
			tokenFile.Add(new ConfigurationSection(key));
		}
		//tokenFile->Save(desinationToken);
		XMLConfigurationFormat::Instance.Save(&tokenFile, FileOutStream(desinationToken));
		

		for (auto e : mtrlTable)
		{
			String destPath = PathUtils::Combine(destination, e.Key);
			destPath.append(L".mtrl");

			MaterialData* md = e.Value;

			md->Save(FileOutStream(destPath));

			delete md;
			//BuildSystem::LogInformation(e.getCurrentKey(), L">");
		}
		mtrlTable.Clear();

		palColors.DeleteValuesAndClear();

		BuildSystem::LogEntryProcessed(desinationToken, hierarchyPath);
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
		
		newNode->Parse(sect, baseMtrlName, [&pallets](const String& colorDesc)->Color4
		{
			return ResolveColor4(colorDesc, pallets);
		});

		// go into sub sections
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ParseMaterialTree(table, newNode, name, ss, pallets);
		}

		table.Add(name, newNode);
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

					if (StringUtils::StartsWith(rightPart, L"(") && StringUtils::EndsWith(rightPart, L")"))
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

					Color4 source;
					if (hasLetters(sIndx))
					{
						StringUtils::Trim(sIndx);

						const Color4* found = result->FindColor(sIndx);
						if (found)
							source = *found;
						else
						{
							BuildSystem::LogError(L"Color " + sIndx + L" not found in " + palName, L"");
							return Color4();
						}
					}
					else
					{
						int index = StringUtils::ParseInt32(sIndx);
						source = result->Colors[index].Color;
					}
					
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
					BuildSystem::LogError(String(L"Pallet not found ") + palName, L"");
			}
			
			BuildSystem::LogError(String(L"Cannot parse ") + text, L"");
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
					
					Pallet* result;
					if (pallets.TryGetValue(palName, result))
					{
						if (hasLetters(sIndx))
						{
							const Color4* found = result->FindColor(sIndx);
							if (found)
								return *found;
							else
							{
								BuildSystem::LogError(L"Color " + sIndx + L" not found in " + palName, L"");
								return Color4();
							}
						}
						else
						{
							int index = StringUtils::ParseInt32(sIndx);
							return result->Colors[index].Color;
						}	
					}
					else
						BuildSystem::LogError(String(L"Pallet not found ") + palName, L"");
				}
				else
					BuildSystem::LogError(String(L"Cannot parse ") + text, L"");
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
			BuildSystem::LogError(String(L"Cannot parse ") + text, L"");
			return Color4();
		}
	}



	const Color4* Pallet::FindColor(const String& name) const
	{
		for (const auto& e : Colors)
		{
			if (e.Name == name)
				return &e.Color;
		}
		return nullptr;
	}

	bool hasLetters(const String& str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			int c = toupper(str[i]);
			if (c >= 'A' && c <= 'Z')
				return true;
		}
		return false;
	}
}