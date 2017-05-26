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