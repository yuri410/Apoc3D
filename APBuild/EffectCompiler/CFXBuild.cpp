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

#include "CFXBuild.h"

#include "BuildConfig.h"
#include "BuildSystem.h"
#include "CompileService.h"


using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{
	void CFXBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		ProjectResCustomEffect config(nullptr, nullptr);
		config.Parse(sect);

		if (!File::FileExists(config.VS))
		{
			BuildSystem::LogError(config.VS, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.PS))
		{
			BuildSystem::LogError(config.PS, L"Could not find source file.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		List<std::pair<std::string, std::string>> defines(config.Defines.getCount());
		for (const auto& e : config.Defines)
		{
			defines.Add(
			{ StringUtils::toPlatformNarrowString(e.first), StringUtils::toPlatformNarrowString(e.second) }
			);
		}

		EffectData data;
		data.Name = sect->getName();
		data.Profiles.ReserveDiscard(config.Targets.getCount());

		for (int i = 0; i < config.Targets.getCount(); i++)
		{
			EffectProfileData& proData = data.Profiles[i];

			std::string impType;
			if (ParseShaderProfileString(config.Targets[i], impType, proData.MajorVer, proData.MinorVer))
			{
				proData.SetImplType(impType);

				if (!CompileShader(config.VS, config.EntryPointVS, data.Profiles[0], ShaderType::Vertex, config.IsDebug, config.NoOptimization, false, &defines))
					return;
				if (!CompileShader(config.PS, config.EntryPointPS, data.Profiles[0], ShaderType::Pixel, config.IsDebug, config.NoOptimization, false, &defines))
					return;
			}
			else
			{
				BuildSystem::LogError(L"Profile not supported." + config.Targets[i], data.Name);
				return;
			}
		}
		
		data.IsCFX = true;
		data.SortProfiles();

		data.Save(FileOutStream(config.DestFile));

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}
}