/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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