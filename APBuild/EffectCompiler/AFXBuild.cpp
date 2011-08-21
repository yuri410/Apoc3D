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
		
		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find source file.");
			return;
		}
		if (!File::FileExists(config.PListFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find param list file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		ID3DXBuffer* error;
		ID3DXBuffer* shader;
		ID3DXConstantTable* constants;
		
		HRESULT hr = D3DXCompileShaderFromFile(config.SrcFile.c_str(), 0, 0, 
			StringUtils::toString(config.EntryPoint.c_str()).c_str(), StringUtils::toString(config.Profile.c_str()).c_str(), 
			D3DXSHADER_PACKMATRIX_ROWMAJOR, &shader, &error, &constants);

		if (hr != S_OK)
		{
			String errmsg = String(reinterpret_cast<const wchar_t*>(error->GetBufferPointer()), error->GetBufferSize());

			std::vector<String> errs = StringUtils::Split(errmsg, L"\n\r");

			for (size_t i=0;i<errs.size();i++)
			{
				CompileLog::WriteError(errs[i], config.SrcFile);
			}
			error->Release();
			return;
		}
		constants->Release();

		EffectData data;
		data.ShaderCodeLength = static_cast<int>( shader->GetBufferSize());
		data.ShaderCode = new char[data.ShaderCodeLength];
	
		memcpy(data.ShaderCode, shader->GetBufferPointer(), data.ShaderCodeLength);
		
		FileLocation* fl = new FileLocation(config.PListFile);
		XMLConfiguration* plist = new XMLConfiguration(fl);

		for (Configuration::Iterator iter = plist->begin(); iter != plist->end();iter++)
		{
			ConfigurationSection* s = iter->second;
			EffectParameter ep(s->getAttribute(L"Name"));
			
			String usage = s->getAttribute(L"Usage");
			ep.TypicalUsage = EffectParameter::ParseParamUsage(usage);
			if (ep.TypicalUsage == EPUSAGE_Unknown)
			{
				ep.IsCustomUsage = true;
				ep.CustomUsage = usage;
			}

			data.Parameters.Add(ep);
		}

		delete fl;


		FileOutStream* fos = new FileOutStream(config.DestFile);
		data.Save(fos);

	}
}