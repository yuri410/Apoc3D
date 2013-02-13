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
#include "ABCConfigurationFormat.h"

#include "ConfigurationSection.h"
#include "Configuration.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/VFS/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

const int FileID = ('A' << 24) | ('p' << 16) | ('B' << 8) | 'C';

namespace Apoc3D
{
	namespace Config
	{
		ABCConfigurationFormat ABCConfigurationFormat::Instance = ABCConfigurationFormat();

		Configuration* ABCConfigurationFormat::Load(const ResourceLocation* rl)
		{
			Configuration* config = new Configuration(rl->getName());

			BinaryReader* br = new BinaryReader(rl->GetReadStream());

			int id = br->ReadInt32();

			if (id == FileID)
			{
				TaggedDataReader* root = br->ReadTaggedDataBlock();

				BuildHierarchy(config, root);

				root->Close();
				delete root;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_System, L"Invalid Apoc3D Binary Config format " + rl->getName(), LOGLVL_Error);
			}

			br->Close();
			delete br;
			return config;
		}
		void ABCConfigurationFormat::Save(Configuration* config, Stream* strm)
		{
			BinaryWriter* bw = new BinaryWriter(strm);

			bw->Write(static_cast<int>(FileID));

			TaggedDataWriter* root = new TaggedDataWriter(true);

			for (Configuration::ChildTable::Enumerator e = config->GetEnumerator(); e.MoveNext();)
			{
				ConfigurationSection* sect = *e.getCurrentValue();
				BinaryWriter* bw2 = root->AddEntry(sect->getName());

				SaveNode(sect, bw2);

				bw2->Close();
				delete bw2;
			}

			bw->Write(root);
			delete root;

			bw->Close();
			delete bw;
		}

		void ABCConfigurationFormat::SaveNode(ConfigurationSection* section, BinaryWriter* bw)
		{
			// 2 tagged to one
			TaggedDataWriter* localValues = new TaggedDataWriter(true);
			{
				localValues->AddEntryString(L"Value", section->getValue());
				
				if (section->getAttributeCount()>0)
				{
					BinaryWriter* bw2 = localValues->AddEntry(L"Attributes");
					bw2->Write(static_cast<int32>(section->getAttributeCount()));
					for (ConfigurationSection::AttributeEnumerator e = section->GetAttributeEnumrator();e.MoveNext();)
					{
						bw2->Write(*e.getCurrentKey());
						bw2->Write(*e.getCurrentValue());
					}
					bw2->Close();
					delete bw2;
				}
			}
			bw->Write(localValues);
			delete localValues;

			//////////////////////////////////////////////////////////////////////////

			TaggedDataWriter* subTreeData = new TaggedDataWriter(true);
			{
				for (ConfigurationSection::SubSectionEnumerator e = section->GetSubSectionEnumrator(); e.MoveNext();)
				{
					ConfigurationSection* sect = *e.getCurrentValue();
					BinaryWriter* bw2 = subTreeData->AddEntry(sect->getName());

					SaveNode(sect, bw2);

					bw2->Close();
					delete bw2;
				}
			}
			bw->Write(subTreeData);
			delete subTreeData;
		}


		void ABCConfigurationFormat::BuildHierarchy(Configuration* config, TaggedDataReader* doc)
		{
			// get sub section names
			List<String> tagNames;
			doc->FillTagList(tagNames); 

			for (int i=0;i<tagNames.getCount();i++)
			{
				BinaryReader* br = doc->GetData(tagNames[i]);

				BuildNode(config, tagNames[i], br, nullptr);

				br->Close();
				delete br;
			}
		}

		void ABCConfigurationFormat::BuildNode(Configuration* config, const String& sectionName, BinaryReader* br, ConfigurationSection* parent)
		{
			ConfigurationSection* section = new ConfigurationSection(sectionName);

			TaggedDataReader* localValues = br->ReadTaggedDataBlock();
			{
				String svalue;
				localValues->GetDataString(L"Value", svalue);
				section->SetValue(svalue);
				
				BinaryReader* br2 = localValues->TryGetData(L"Attributes");
				if (br2)
				{
					int count = br2->ReadInt32();
					for (int i=0;i<count;i++)
					{
						String key = br2->ReadString();
						String val = br2->ReadString();
						section->AddAttributeString(key, val);
					}
					br2->Close();
					delete br2;
				}
			}
			localValues->Close();
			delete localValues;

			TaggedDataReader* subTreeData = br->ReadTaggedDataBlock();
			{
				List<String> tagNames;
				subTreeData->FillTagList(tagNames); 

				for (int i=0;i<tagNames.getCount();i++)
				{
					BinaryReader* br2 = subTreeData->GetData(tagNames[i]);

					BuildNode(config, tagNames[i], br2, section);

					br2->Close();
					delete br2;
				}
			}
			subTreeData->Close();
			delete subTreeData;

			if (parent)
			{
				// parent add section
				parent->AddSection(section);
			}
			else
			{
				config->Add(sectionName, section);
			}
		}
	}
}