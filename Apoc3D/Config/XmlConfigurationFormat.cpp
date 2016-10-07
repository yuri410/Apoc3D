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
#include "XmlConfigurationFormat.h"

#include "ConfigurationSection.h"
#include "ConfigurationManager.h"

#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/VFS/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"

#include "apoc3d/Library/tinyxml.h"
#include "apoc3d/Library/ConvertUTF.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		XMLConfigurationFormat XMLConfigurationFormat::Instance;

		void XMLConfigurationFormat::Load(const ResourceLocation& rl, Configuration* config)
		{
			TiXmlDocument doc;

			Stream* strm = rl.GetReadStream();

			doc.Load(*strm, TIXML_ENCODING_UNKNOWN);
			
			delete strm;

			BuildXml(config, &doc);
		}
		void XMLConfigurationFormat::Save(Configuration* config, Stream& strm)
		{
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
			doc.LinkEndChild(decl);

			TiXmlElement* root = new TiXmlElement("Root");
			doc.LinkEndChild(root);

			for (ConfigurationSection* sect : config->getSubSections())
			{
				TiXmlElement* elem = new TiXmlElement(sect->getName());
				root->LinkEndChild(elem);
				SaveNode(elem, sect);
			}

			doc.Save(strm);
		}

		void XMLConfigurationFormat::BuildNode(Configuration* config, const TiXmlNode* node, ConfigurationSection* parent, const TiXmlDocument& doc)
		{
			int type = node->Type();

			switch (type)
			{
			case TiXmlNode::TINYXML_ELEMENT:
				{
					const TiXmlElement* elem = node->ToElement();

					String strName = doc.GetUTF16ElementName(elem);
					
					ConfigurationSection* section = new ConfigurationSection(strName);


					for (const TiXmlAttribute* i = elem->FirstAttribute(); i!=0; i=i->Next())
					{
						section->AddAttributeString(doc.GetUTF16AttribName(i), doc.GetUTF16AttribValue(i));
					}


					for (const TiXmlNode* i = node->FirstChild(); i!=0; i=i->NextSibling())
					{
						BuildNode(config, i, section, doc);
					}
					if (parent)
					{
						// parent add section
						parent->AddSection(section);
					}
					else
					{
						config->Add(strName, section);
					}
				}
				break;
			case TiXmlNode::TINYXML_TEXT:
				{
					const TiXmlText* text = node->ToText();
					
					String strText = doc.GetUTF16NodeText(text);
					
					parent->SetValue(strText);
				}
				break;
			}
		}
		void XMLConfigurationFormat::BuildXml(Configuration* config, const TiXmlDocument* doc)
		{
			const TiXmlNode* i = doc->FirstChildElement();
			if (i)
			{
				for (const TiXmlNode* j = i->FirstChild(); j != 0; j = j->NextSibling())
				{
					BuildNode(config, j, nullptr, *doc);
				}
			}
		}

		void XMLConfigurationFormat::SaveNode(TiXmlNode* node, ConfigurationSection* parent)
		{
			for (auto e : parent->getAttributes())
			{
				node->ToElement()->SetAttribute(e.Key, e.Value);
			}
			
			for (ConfigurationSection* s : parent->getSubSections())
			{
				TiXmlElement* elem = new TiXmlElement(s->getName());
				if (s->getValue().size())
				{
					TiXmlText* txt = new TiXmlText(s->getValue());
					elem->LinkEndChild(txt);
				}

				node->LinkEndChild(elem);
				SaveNode(elem, s);
			}
			
		}

		List<String> XMLConfigurationFormat::GetSupportedFileSystemExtensions()
		{
			return{ L"xml" };
		}
	}
}