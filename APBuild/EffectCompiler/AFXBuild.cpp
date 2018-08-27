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
		ProjectResEffect config(nullptr, nullptr);
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
			EffectProfileData& prof = data.Profiles[i];

			if (hasPlist)
			{
				ParseParametersFromPList(prof, plist, config.Targets[i], config.PListFile);
			}

			std::string impType;
			if (ParseShaderProfileString(config.Targets[i], impType, prof.MajorVer, prof.MinorVer))
			{
				prof.SetImplType(impType);

				if (!CompileShader(config.VS, config.EntryPointVS, prof, ShaderType::Vertex, config.IsDebug, config.NoOptimization, !hasPlist, &defines))
					return;
				if (!CompileShader(config.PS, config.EntryPointPS, prof, ShaderType::Pixel, config.IsDebug, config.NoOptimization, !hasPlist, &defines))
					return;

				if (config.GS.size())
				{
					if (!CompileShader(config.GS, config.EntryPointGS, prof, ShaderType::Geometry, config.IsDebug, config.NoOptimization, !hasPlist, &defines))
						return;
				}
			}
			else
			{
				BuildSystem::LogError(L"Target profile is not supported " + config.Targets[i], data.Name);
				return;
			}
		}
		
		data.SortProfiles();
		data.Save(FileOutStream(config.DestFile));

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}


}