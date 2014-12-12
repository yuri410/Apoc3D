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

#include "ShaderDataIO.h"

namespace APDesigner
{
	void ShaderDocumentData::Load(const String& filePath)
	{
		Configuration config;
		XMLConfigurationFormat::Instance.Load(FileLocation(filePath), &config);
		
		ConfigurationSection* sect = config[L"Basic"];
		MajorSMVersion = sect->GetInt(L"MajorSMVersion");
		MinorSMVersion = sect->GetInt(L"MinorSMVersion");

		sect = config[L"Nodes"];
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			Nodes.Add(ss->getValue());
		}

		sect = config[L"Varyings"];
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ShaderNetVaryingNode inp;
			inp.Parse(ss);
			VaryingNodes.Add(inp);
		}

		sect = config[L"Constants"];
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ShaderNetConstantNode oup;
			oup.Parse(ss);
			ConstantNodes.Add(oup);
		}

		sect = config[L"Links"];
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ShaderAtomLinkInfo lnk;
			lnk.Parse(ss);
			Links.Add(lnk);
		}

	}

	void ShaderDocumentData::Save(const String& filePath)
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ShaderAtomLinkInfo::Parse(ConfigurationSection* sect)
	{
		SourceNodeIndex = sect->GetAttributeInt(L"Source");
		TargetNodeIndex = sect->GetAttributeInt(L"Target");

		SourcePortID = TargetPortID = -1;
		sect->TryGetAttributeInt(L"SourcePortID", SourcePortID);
		sect->TryGetAttributeInt(L"TargetPortID", TargetPortID);

		VaryingNodeIndex = ConstNodeIndex = -1;
		sect->TryGetAttributeInt(L"VaryingNodeIndex", VaryingNodeIndex);
		sect->TryGetAttributeInt(L"ConstNodeIndex", ConstNodeIndex);

		
	}

}