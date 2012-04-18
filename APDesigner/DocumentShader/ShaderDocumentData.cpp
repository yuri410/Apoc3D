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

#include "ShaderDocumentData.h"

#include "Config/XmlConfiguration.h"
#include "Config/ConfigurationSection.h"

using namespace Apoc3D::Config;

namespace APDesigner
{
	void ShaderDocumentData::Load(const String& filePath)
	{
		XMLConfiguration* config = new XMLConfiguration(filePath);

		ConfigurationSection* sect = config->get(L"Basic");
		MajorSMVersion = sect->GetInt(L"MajorSMVersion");
		MinorSMVersion = sect->GetInt(L"MinorSMVersion");

		sect = config->get(L"Nodes");
		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator();e.MoveNext();)
		{
			Nodes.Add(
				(*e.getCurrentValue())->getValue()
			);
		}

		sect = config->get(L"Inputs");



		sect = config->get(L"Outputs");



		sect = config->get(L"Links");


		delete config;
	}
	void ShaderDocumentData::Save(const String& filePath)
	{

	}
}