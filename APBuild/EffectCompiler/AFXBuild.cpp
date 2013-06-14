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

#include "AFXBuild.h"

#include "../BuildConfig.h"
#include "../BuildEngine.h"
#include "../CompileLog.h"
#include "CompileService.h"

#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
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


#include <d3d9.h>
#include <d3dx9.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{
	void Parse(const AFXBuildConfig& config, ConfigurationSection* ps, EffectParameter& ep);

	void AFXBuild::Build(const ConfigurationSection* sect)
	{
		AFXBuildConfig config;
		config.Parse(sect);
		
		if (!File::FileExists(config.VS))
		{
			CompileLog::WriteError(config.VS, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.PS))
		{
			CompileLog::WriteError(config.PS, L"Could not find source file.");
			return;
		}
		if (config.GS.size() && !File::FileExists(config.GS))
		{
			CompileLog::WriteError(config.GS, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.PListFile))
		{
			CompileLog::WriteError(config.PListFile, L"Could not find param list file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		FileLocation* fl = new FileLocation(config.PListFile);
		Configuration* plist = XMLConfigurationFormat::Instance.Load(fl);

		EffectData data;
		data.Name = config.Name;
		data.ProfileCount = config.Targets.getCount();
		data.Profiles = new EffectProfileData[data.ProfileCount];
		
		for (int i=0;i<config.Targets.getCount();i++)
		{
			EffectProfileData& prof = data.Profiles[i];

			std::string impType;
			if (ParseShaderProfileString(config.Targets[i], impType, prof.MajorVer, prof.MinorVer))
			{
				prof.SetImplType(impType);

				if (!CompileShader(config.VS, config.EntryPointVS, prof, SHDT_Vertex, config.IsDebug))
					return;
				if (!CompileShader(config.PS, config.EntryPointPS, prof, SHDT_Pixel, config.IsDebug))
					return;

				if (config.GS.size())
				{
					if (!CompileShader(config.GS, config.EntryPointGS, prof, SHDT_Geometry, config.IsDebug))
						return;
				}

				ShaderType shaderTypes[3] = { SHDT_Vertex, SHDT_Pixel, SHDT_Geometry };
				ConfigurationSection* shaderParams[3];
				ConfigurationSection* profContainer = plist->get(config.Targets[i]);
				if (profContainer)
				{
					shaderParams[0] = profContainer->getSection(L"VS");
					shaderParams[1] = profContainer->getSection(L"PS");
					shaderParams[2] = profContainer->getSection(L"GS");
				}
				else
				{
					shaderParams[0] = plist->get(L"VS");
					shaderParams[1] = plist->get(L"PS");
					shaderParams[2] = plist->get(L"GS");
				}
				
				for (int j=0;j<3;j++)
				{
					ConfigurationSection* sect = shaderParams[j];
					if (!sect)
						continue;

					for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator(); iter.MoveNext();)
					{
						ConfigurationSection* psect = *iter.getCurrentValue();
						EffectParameter ep(psect->getName());

						Parse(config, psect, ep);

						ep.ProgramType = shaderTypes[j];
						ep.SamplerState.Parse(psect);
						prof.Parameters.Add(ep);
					}
				}
			}
			else
			{
				CompileLog::WriteError(L"Target profile is not supported "+ config.Targets[i], config.Name);
				return;
			}
		}

		delete fl;
		delete plist;

		data.SortProfiles();

		FileOutStream* fos = new FileOutStream(config.DestFile);
		data.Save(fos);
	}

	void Parse(const AFXBuildConfig& config, ConfigurationSection* ps, EffectParameter& ep)
	{
		ep.Usage = EffectParameter::ParseParamUsage(ps->getAttribute(L"Usage"));
		if (ep.Usage == EPUSAGE_CustomMaterialParam)
		{
			if (!ps->hasAttribute(L"CustomUsage"))
			{
				CompileLog::WriteError(config.PListFile, L"Could not find CustomUsage for param " + ep.Name);
			}
			else
			{
				ep.CustomMaterialParamName = ps->getAttribute(L"CustomUsage");
			}
		}
		else if (ep.Usage == EPUSAGE_InstanceBlob)
		{
			if (!ps->hasAttribute(L"BlobIndex"))
			{
				CompileLog::WriteError(config.PListFile, L"Could not find BlobIndex for param " + ep.Name);
			}
			else
			{
				ep.InstanceBlobIndex = ps->GetAttributeInt(L"BlobIndex");
			}
			
		}
	}
}