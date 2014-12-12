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
#include "ABCConfigurationFormat.h"

#include "ConfigurationSection.h"

#include "apoc3d/Config/ConfigurationManager.h"
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

const int32 FileID = 'ApBC';

namespace Apoc3D
{
	namespace Config
	{
		ABCConfigurationFormat ABCConfigurationFormat::Instance;

		void ABCConfigurationFormat::Load(const ResourceLocation& rl, Configuration* config)
		{
			BinaryReader br(rl);

			int id = br.ReadInt32();

			if (id == FileID)
			{
				br.ReadTaggedDataBlock([this, config](TaggedDataReader* root)
				{
					BuildHierarchy(config, root);
				}, false);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_System, L"Invalid Apoc3D Binary Config format " + rl.getName(), LOGLVL_Error);
			}
		}
		void ABCConfigurationFormat::Save(Configuration* config, Stream& strm)
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(FileID);

			bw.WriteTaggedDataBlock([this, config](TaggedDataWriter* root)
			{
				for (ConfigurationSection* sect : config->getSubSections())
				{
					root->AddEntry(StringUtils::UTF16toUTF8(sect->getName()), [this, sect](BinaryWriter* bw2)
					{
						SaveNode(sect, bw2);
					});
				}
			});
		}

		void ABCConfigurationFormat::SaveNode(ConfigurationSection* section, BinaryWriter* bw)
		{
			// 2 tagged to one
			bw->WriteTaggedDataBlock([section](TaggedDataWriter* localValues)
			{
				localValues->AddEntryString("Value", section->getValue());

				if (section->getAttributeCount() > 0)
				{
					localValues->AddEntry("Attributes", [section](BinaryWriter* bw2)
					{
						bw2->WriteInt32(static_cast<int32>(section->getAttributeCount()));
						for (auto e : section->getAttributes())
						{
							bw2->WriteString(e.Key);
							bw2->WriteString(e.Value);
						}
					});
				}
			});

			//////////////////////////////////////////////////////////////////////////

			bw->WriteTaggedDataBlock([section, this](TaggedDataWriter* subTreeData)
			{
				for (ConfigurationSection* sect : section->getSubSections())
				{
					subTreeData->AddEntry(StringUtils::UTF16toUTF8(sect->getName()), [sect, this](BinaryWriter* bw2)
					{
						SaveNode(sect, bw2);
					});
				}
			});
		}


		void ABCConfigurationFormat::BuildHierarchy(Configuration* config, TaggedDataReader* doc)
		{
			// get sub section names
			List<std::string> tagNames;
			doc->FillTagList(tagNames); 

			for (const std::string& sn : tagNames)
			{	
				doc->ProcessData(sn, [this, config, sn](BinaryReader* br)
				{
					BuildNode(config, sn, br, nullptr);
				});
			}
		}

		void ABCConfigurationFormat::BuildNode(Configuration* config, const std::string& sectionName, BinaryReader* br, ConfigurationSection* parent)
		{
			String wSectName = StringUtils::UTF8toUTF16(sectionName);
			ConfigurationSection* section = new ConfigurationSection(wSectName);

			br->ReadTaggedDataBlock([section](TaggedDataReader* localValues)
			{
				String svalue;
				localValues->GetDataString("Value", svalue);
				section->SetValue(svalue);

				localValues->TryProcessData("Attributes", [section](BinaryReader* br2)
				{
					int count = br2->ReadInt32();
					for (int i = 0; i < count; i++)
					{
						String key = br2->ReadString();
						String val = br2->ReadString();
						section->AddAttributeString(key, val);
					}
				});
			});

			br->ReadTaggedDataBlock([section, this, config](TaggedDataReader* subTreeData)
			{
				List<std::string> tagNames;
				subTreeData->FillTagList(tagNames);

				for (const std::string& sn : tagNames)
				{
					subTreeData->ProcessData(sn, [this, config, sn, section](BinaryReader* br2)
					{
						BuildNode(config, sn, br2, section);
					});
				}
			});


			if (parent)
			{
				// parent add section
				parent->AddSection(section);
			}
			else
			{
				config->Add(wSectName, section);
			}
		}
	}
}