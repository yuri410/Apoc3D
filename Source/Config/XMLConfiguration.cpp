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
#include "XmlConfiguration.h"

#include "Vfs/ResourceLocation.h"
#include "IOLib/Streams.h"
#include "ConfigurationSection.h"
#include "Utility/StringUtils.h"
#include "VFS/PathUtils.h"

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		String getElementName(const TiXmlElement* elem)
		{
			string str = elem->ValueStr();

			return StringUtils::toWString(str);
		}
		String getNodeText(const TiXmlText* text)
		{
			string str = text->ValueStr();
			return StringUtils::toWString(str);
		}
		String getAttribName(const TiXmlAttribute* attrib)
		{
			string str = attrib->NameTStr();
			return StringUtils::toWString(str);
		}
		String getAttribValue(const TiXmlAttribute* attrib)
		{
			string str = attrib->ValueStr();
			return StringUtils::toWString(str);
		}

		void XMLConfiguration::BuildNode(const TiXmlNode* node, ConfigurationSection* parent)
		{
			int type = node->Type();

			switch (type)
			{
			case TiXmlNode::TINYXML_ELEMENT:
				{
					const TiXmlElement* elem = node->ToElement();

					String strName = getElementName(elem);
					
					ConfigurationSection* section = new ConfigurationSection(strName);


					for (const TiXmlAttribute* i = elem->FirstAttribute(); i!=0; i=i->Next())
					{
						section->AddAttribute(getAttribName(i), getAttribValue(i));
					}


					for (const TiXmlNode* i = node->FirstChild(); i!=0; i=i->NextSibling())
					{
						BuildNode(i, section);
					}
					if (parent)
					{
						// parent add section
						parent->AddSection(section);
					}
					else
					{
						m_sections.insert(ChildTable::value_type(strName, section));
					}
				}
				break;
			case TiXmlNode::TINYXML_TEXT:
				{
					const TiXmlText* text = node->ToText();
					
					String strText = getNodeText(text);;
					
					parent->SetValue(strText);
				}
				break;
			}
		}
		void XMLConfiguration::BuildXml(const TiXmlDocument* doc)
		{
			const TiXmlNode* i = doc->FirstChildElement();
			if (i)
			{
				for (const TiXmlNode* j = i->FirstChild(); j!=0; j=j->NextSibling())
				{
					BuildNode(j, 0);
				}
			}
		}


		XMLConfiguration::XMLConfiguration(const ResourceLocation* rl)
			: Configuration(rl->getName())
		{
			TiXmlDocument doc;
			
			Stream* strm = rl->GetReadStream();
			
			char* buffer = new char[(uint)strm->getLength()];
			strm->Read(buffer, strm->getLength());

			doc.Parse(buffer);

			strm->Close();
			delete strm;
			delete buffer;

			BuildXml(&doc);
		}

		Configuration* XMLConfiguration::Clone() const
		{
			return 0;
		}
		void XMLConfiguration::Merge(Configuration* config)
		{

		}

	}
}