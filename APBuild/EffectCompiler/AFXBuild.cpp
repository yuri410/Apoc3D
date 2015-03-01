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

#include "BuildConfig.h"
#include "BuildSystem.h"
#include "CompileService.h"

namespace APBuild
{
	void ParseEffectParameter(const String& plistFile, ConfigurationSection* ps, EffectParameter& ep)
	{
		const String& usageText = ps->getAttribute(L"Usage");
		ep.Usage = EffectParameter::ParseParamUsage(usageText);
		if (ep.Usage == EPUSAGE_CustomMaterialParam)
		{
			if (!ps->hasAttribute(L"CustomUsage"))
			{
				BuildSystem::LogError(plistFile, L"Could not find CustomUsage attribute for param " + ep.Name);
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
				BuildSystem::LogError(plistFile, L"Could not find BlobIndex attribute for param " + ep.Name);
			}
			else
			{
				ep.InstanceBlobIndex = ps->GetAttributeInt(L"BlobIndex");
			}
		}

		if (ep.Usage == EPUSAGE_Unknown && usageText.size())
		{
			BuildSystem::LogWarning(plistFile, L"Usage " + usageText + L" is invalid for param " + ep.Name);
		}

		ps->tryGetAttribute(L"SamplerStateOverridenGroupName", ep.SamplerStateOverridenGroupName);
		ps->tryGetAttribute(L"DefaultTextureName", ep.DefaultTextureName);
	}

	void ParseParametersFromPList(EffectProfileData& prof, Configuration& plist, const String& targetName, const String& plistFileName)
	{
		ShaderType shaderTypes[3] = { ShaderType::Vertex, ShaderType::Pixel, ShaderType::Geometry };
		ConfigurationSection* shaderParams[3] = { 0 };
		ConfigurationSection* profContainer = plist[targetName];
		if (profContainer)
		{
			shaderParams[0] = profContainer->getSection(L"VS");
			shaderParams[1] = profContainer->getSection(L"PS");
			shaderParams[2] = profContainer->getSection(L"GS");
		}
		else
		{
			shaderParams[0] = plist[L"VS"];
			shaderParams[1] = plist[L"PS"];
			shaderParams[2] = plist[L"GS"];
		}

		for (int32 i = 0; i < countof(shaderParams); i++)
		{
			ConfigurationSection* sect = shaderParams[i];

			if (!sect)
				continue;

			for (ConfigurationSection* psect : sect->getSubSections())
			{
				EffectParameter ep(psect->getName());

				ParseEffectParameter(plistFileName, psect, ep);

				ep.ProgramType = shaderTypes[i];
				ep.SamplerState.Parse(psect);
				prof.Parameters.Add(ep);
			}
		}

	}

	bool ProcessIncludes(Configuration* plist, const String& baseDir)
	{
		Configuration* curPlist = plist;

		HashSet<String> openedIncludes;
		Queue<String> includeQueue;

		if (curPlist->get(L"Include"))
		{
			includeQueue.Enqueue(curPlist->get(L"Include")->getValue());
		}

		while (includeQueue.getCount() > 0)
		{
			String includeFile = includeQueue.Dequeue();

			if (!openedIncludes.Contains(includeFile))
			{
				openedIncludes.Add(includeFile);

				includeFile = PathUtils::Combine(baseDir, includeFile);
				if (!File::FileExists(includeFile))
				{
					BuildSystem::LogError(includeFile, L"Could not find param list include file.");
					return false;
				}

				Configuration includeSrc;
				XMLConfigurationFormat::Instance.Load(FileLocation(includeFile), &includeSrc);

				if (includeSrc[L"Include"])
				{
					includeQueue.Enqueue(includeSrc[L"Include"]->getValue());
				}

				curPlist->Merge(&includeSrc, true);
			}
		}
		return true;
	}


	void AFXBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		AFXBuildConfig config;
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
		if (config.GS.size() && !File::FileExists(config.GS))
		{
			BuildSystem::LogError(config.GS, L"Could not find source file.");
			return;
		}
		if (config.PListFile.size() && !File::FileExists(config.PListFile))
		{
			BuildSystem::LogError(config.PListFile, L"Could not find param list file.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		bool hasPlist = false;
		Configuration plist;
		if (config.PListFile.size())
		{
			hasPlist = true;
			XMLConfigurationFormat::Instance.Load(FileLocation(config.PListFile), &plist);

			if (!ProcessIncludes(&plist, PathUtils::GetDirectory(config.PListFile)))
			{
				return;
			}
		}

		EffectData data;
		data.Name = config.Name;
		data.Profiles.ReserveDiscard(config.Targets.getCount());
		
		for (int i = 0; i < config.Targets.getCount(); i++)
		{
			EffectProfileData& prof = data.Profiles[i];

			if (hasPlist)
			{
				ParseParametersFromPList(prof, plist, config.Targets[i], config.PListFile);
			}

			std::string impType;
			if (ParseShaderProfileString(config.Targets[i], impType, prof.MajorVer, prof.MinorVer))
			{
				prof.SetImplType(impType);

				if (!CompileShader(config.VS, config.EntryPointVS, prof, ShaderType::Vertex, config.IsDebug, config.NoOptimization, !hasPlist, &config.Defines))
					return;
				if (!CompileShader(config.PS, config.EntryPointPS, prof, ShaderType::Pixel, config.IsDebug, config.NoOptimization, !hasPlist, &config.Defines))
					return;

				if (config.GS.size())
				{
					if (!CompileShader(config.GS, config.EntryPointGS, prof, ShaderType::Geometry, config.IsDebug, config.NoOptimization, !hasPlist, &config.Defines))
						return;
				}
			}
			else
			{
				BuildSystem::LogError(L"Target profile is not supported " + config.Targets[i], config.Name);
				return;
			}
		}
		
		data.SortProfiles();
		data.Save(FileOutStream(config.DestFile));

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}


}