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

#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/VFS/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"

#include "apoc3d/Library/tinyxml.h"
#include "apoc3d/Library/ConvertUTF.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		XMLConfigurationFormat XMLConfigurationFormat::Instance = XMLConfigurationFormat();

		String getElementName(const TiXmlElement* elem, bool isAscii);
		String getNodeText(const TiXmlText* text, bool isAscii);
		String getAttribName(const TiXmlAttribute* attrib, bool isAscii);
		String getAttribValue(const TiXmlAttribute* attrib, bool isAscii);

		Configuration* XMLConfigurationFormat::Load(const ResourceLocation* rl)
		{
			Configuration* config = new Configuration(rl->getName());

			TiXmlDocument doc;

			Stream* strm = rl->GetReadStream();

			TiXmlEncoding encoding = TIXML_ENCODING_UNKNOWN;

			doc.Load(strm, encoding);
			
			bool isAscii = encoding != TIXML_ENCODING_UTF8;

			BuildXml(config, &doc, isAscii);

			return config;
		}
		void XMLConfigurationFormat::Save(Configuration* config, Stream* strm)
		{
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
			doc.LinkEndChild( decl );  

			TiXmlElement* root = new TiXmlElement("Root");
			doc.LinkEndChild(root);

			for (Configuration::ChildTable::Enumerator iter = config->GetEnumerator();iter.MoveNext();)
			{
				ConfigurationSection* sect = *iter.getCurrentValue();
				TiXmlElement* elem = new TiXmlElement(StringUtils::toString(sect->getName()));
				root->LinkEndChild(elem);
				SaveNode(elem, sect);
			}

			doc.Save(strm);
			//doc.SaveFile(StringUtils::toString(filePath));
		}

		void XMLConfigurationFormat::BuildNode(Configuration* config, const TiXmlNode* node, ConfigurationSection* parent, bool isAscii)
		{
			int type = node->Type();

			switch (type)
			{
			case TiXmlNode::TINYXML_ELEMENT:
				{
					const TiXmlElement* elem = node->ToElement();

					String strName = getElementName(elem, isAscii);
					
					ConfigurationSection* section = new ConfigurationSection(strName);


					for (const TiXmlAttribute* i = elem->FirstAttribute(); i!=0; i=i->Next())
					{
						section->AddAttributeString(getAttribName(i, isAscii), getAttribValue(i, isAscii));
					}


					for (const TiXmlNode* i = node->FirstChild(); i!=0; i=i->NextSibling())
					{
						BuildNode(config, i, section, isAscii);
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
					
					String strText = getNodeText(text, isAscii);
					
					parent->SetValue(strText);
				}
				break;
			}
		}
		void XMLConfigurationFormat::BuildXml(Configuration* config, const TiXmlDocument* doc, bool isAscii)
		{
			const TiXmlNode* i = doc->FirstChildElement();
			if (i)
			{
				for (const TiXmlNode* j = i->FirstChild(); j!=0; j=j->NextSibling())
				{
					BuildNode(config, j, nullptr, isAscii);
				}
			}
		}

		void XMLConfigurationFormat::SaveNode(TiXmlNode* node, ConfigurationSection* parent)
		{
			for (ConfigurationSection::AttributeEnumerator iter = parent->GetAttributeEnumrator();iter.MoveNext();)
			{
				node->ToElement()->SetAttribute(StringUtils::toString(*iter.getCurrentKey()), StringUtils::toString(*iter.getCurrentValue()));
			}
			
			for (ConfigurationSection::SubSectionEnumerator iter = parent->GetSubSectionEnumrator();iter.MoveNext();)
			{
				ConfigurationSection* s = *iter.getCurrentValue();
				TiXmlElement* elem = new TiXmlElement(StringUtils::toString(s->getName()));
				if (s->getValue().size())
				{
					TiXmlText* txt = new TiXmlText(StringUtils::toString(s->getValue()));
					elem->LinkEndChild(txt);
				}
				//elem->SetText(StringUtils::toString(s->getValue()));
				node->LinkEndChild(elem);
				SaveNode(elem, s);
			}
		}

		String getElementName(const TiXmlElement* elem, bool isAscii)
		{
			std::string str = elem->ValueStr();

			if (isAscii) return String(str.begin(), str.end());

			return StringUtils::UTF8toUTF16(str);
		}
		String getNodeText(const TiXmlText* text, bool isAscii)
		{
			std::string str = text->ValueStr();

			if (isAscii) return String(str.begin(), str.end());

			return StringUtils::UTF8toUTF16(str);
		}
		String getAttribName(const TiXmlAttribute* attrib, bool isAscii)
		{
			std::string str = attrib->NameTStr();

			if (isAscii) return String(str.begin(), str.end());

			return StringUtils::UTF8toUTF16(str);
		}
		String getAttribValue(const TiXmlAttribute* attrib, bool isAscii)
		{
			std::string str = attrib->ValueStr();

			if (isAscii) return String(str.begin(), str.end());

			return StringUtils::UTF8toUTF16(str);
		}
	}
}