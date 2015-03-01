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
		CFXBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SrcVSFile))
		{
			BuildSystem::LogError(config.SrcVSFile, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.SrcPSFile))
		{
			BuildSystem::LogError(config.SrcPSFile, L"Could not find source file.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));


		EffectData data;
		data.Name = config.Name;
		data.Profiles.ReserveDiscard(1);
		
		EffectProfileData& proData = data.Profiles[0];

		std::string impType;
		if (ParseShaderProfileString(config.Profile, impType, proData.MajorVer, proData.MinorVer))
		{
			proData.SetImplType(impType);

			if (!CompileShader(config.SrcVSFile, config.EntryPointVS, data.Profiles[0], ShaderType::Vertex, false, false, nullptr, nullptr))
				return;
			if (!CompileShader(config.SrcPSFile, config.EntryPointPS, data.Profiles[0], ShaderType::Pixel, false, false, nullptr, nullptr))
				return;
		}
		else
		{
			BuildSystem::LogError(L"Profile not supported." + config.Profile, config.Name);
		}

		data.IsCFX=true;
		data.SortProfiles();

		data.Save(FileOutStream(config.DestFile));

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}
}