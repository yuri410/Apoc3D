/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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

#include "Collections/CollectionsCommon.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Collections/FastList.h"
#include "Collections/ExistTable.h"
#include "Collections/FastMap.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryWriter.h"
#include "IOLib/EffectData.h"
#include "BuildConfig.h"
#include "BuildEngine.h"
#include "CompileLog.h"
#include "Utility/StringUtils.h"
#include "Vfs/File.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"

#include "HLSLCompileService.h"

#include <d3d9.h>
#include <d3dx9.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{
	void AFXBuild::Build(const ConfigurationSection* sect)
	{
		AFXBuildConfig config;
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
		if (!File::FileExists(config.PListFile))
		{
			CompileLog::WriteError(config.PListFile, L"Could not find param list file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));


		EffectData data;
		data.Name = config.Name;
		StringUtils::ToLowerCase(config.Profile);
		if (config.Profile == L"sm2.0")
		{
			data.MajorVer = 2;
			data.MinorVer = 0;
		}
		else if (config.Profile == L"sm3.0")
		{
			data.MajorVer = 3;
			data.MinorVer = 0;
		}
		else if (config.Profile == L"sm1.0")
		{
			data.MajorVer = 1;
			data.MinorVer = 1;
		}

		if (!CompileShader(config.SrcVSFile, config.EntryPointVS, config.Profile, data.VSCode, data.VSLength, true))
			return;
		if (!CompileShader(config.SrcPSFile, config.EntryPointPS, config.Profile, data.PSCode, data.PSLength, false))
			return;

		FileLocation* fl = new FileLocation(config.PListFile);
		XMLConfiguration* plist = new XMLConfiguration(fl);

		ConfigurationSection* s = plist->get(L"VS");

		for (ConfigurationSection::SubSectionEnumerator iter = s->GetSubSectionEnumrator(); iter.MoveNext();)
		{
			ConfigurationSection* ps = *iter.getCurrentValue();
			EffectParameter ep(ps->getName());
			
			//String usage = ps->getAttribute(L"Usage");
			//ep.TypicalUsage = EffectParameter::ParseParamUsage(usage);
			//if (ep.TypicalUsage == EPUSAGE_Unknown)
			//{
				//ep.IsCustomUsage = true;
				//ep.CustomUsage = usage;
			//}
			ep.CustomUsage = ps->getAttribute(L"Usage");
			ep.ProgramType = SHDT_Vertex;
			ep.SamplerState.Parse(ps);
			data.Parameters.Add(ep);
		}

		s = plist->get(L"PS");

		for (ConfigurationSection::SubSectionEnumerator iter = s->GetSubSectionEnumrator(); iter.MoveNext();)
		{
			ConfigurationSection* ps = *iter.getCurrentValue();
			EffectParameter ep(ps->getName());

			//String usage = ps->getAttribute(L"Usage");
			//ep.TypicalUsage = EffectParameter::ParseParamUsage(usage);
			//if (ep.TypicalUsage == EPUSAGE_Unknown)
			//{
			//	ep.IsCustomUsage = true;
			//	ep.CustomUsage = usage;
			//}
			ep.CustomUsage = ps->getAttribute(L"Usage");
			ep.ProgramType = SHDT_Pixel;
			ep.SamplerState.Parse(ps);
			data.Parameters.Add(ep);
		}

		delete fl;
		delete plist;

		FileOutStream* fos = new FileOutStream(config.DestFile);
		data.Save(fos);

	}
}