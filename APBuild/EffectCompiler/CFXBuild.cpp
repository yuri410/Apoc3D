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

#include "../BuildConfig.h"
#include "../BuildEngine.h"
#include "../CompileLog.h"
#include "CompileService.h"

#include "apoc3d/Collections/CollectionsCommon.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/ExistTable.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/EffectData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"


#include <d3d9.h>
#include <d3dx9.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{
	void CFXBuild::Build(const ConfigurationSection* sect)
	{
		CFXBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SrcVSFile))
		{
			CompileLog::WriteError(config.SrcVSFile, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.SrcPSFile))
		{
			CompileLog::WriteError(config.SrcPSFile, L"Could not find source file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));


		EffectData data;
		data.Name = config.Name;
		data.ProfileCount = 1;
		data.Profiles = new EffectProfileData[1];
		
		std::string impType;
		if (ParseShaderProfileString(config.Profile, impType, data.Profiles->MajorVer, data.Profiles->MinorVer))
		{
			data.Profiles->SetImplType(impType);

			if (!CompileShader(config.SrcVSFile, config.EntryPointVS, data.Profiles[0], SHDT_Vertex, false))
				return;
			if (!CompileShader(config.SrcPSFile, config.EntryPointPS, data.Profiles[0], SHDT_Pixel, false))
				return;
		}
		else
		{
			CompileLog::WriteError(L"Profile not supported." + config.Profile, config.Name);
		}

		data.IsCFX=true;
		data.SortProfiles();

		FileOutStream* fos = new FileOutStream(config.DestFile);
		data.Save(fos);
	}
}