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

#include "FXListBuild.h"

#include "BuildConfig.h"
#include "BuildSystem.h"

namespace APBuild
{
	void FXListBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		FXListBuildConfig config;
		config.Parse(sect);

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.Name));

		Configuration* xml = new Configuration(L"FXList");

		for (ConfigurationSection* s : sect->getSubSections())
		{
			ConfigurationSection* ss = new ConfigurationSection(s->getName());
			xml->Add(ss);
		}

		//xml->Save(config.DestFile);
		XMLConfigurationFormat::Instance.Save(xml, FileOutStream(config.DestFile));
		delete xml;

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}
}