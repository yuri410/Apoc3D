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
#include "MeshBuild.h"

#include "Config/ConfigurationSection.h"
#include "BuildConfig.h"
#include "AIImporter.h"
#include "XImporter.h"
#include "FbxImporter.h"
#include "IOLib/Streams.h"
#include "IOLib/ModelData.h"
#include "CompileLog.h"
#include "Vfs/File.h"
#include "Vfs/PathUtils.h"
#include "BuildEngine.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	void MeshBuild::BuildByFBX(const MeshBuildConfig& config)
	{
		FbxImporter::Import(config);

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
	void MeshBuild::BuildByASS(const MeshBuildConfig& config)
	{
		AIImporter importer;
		ModelData* data = importer.Import(config);

		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);


		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
	void MeshBuild::BuildByD3D(const MeshBuildConfig& config)
	{
		XImporter importer;
		ModelData* data = importer.Import(config);

		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);

		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}

	void MeshBuild::Build(const ConfigurationSection* sect)
	{
		MeshBuildConfig config;
		config.Parse(sect);
		
		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find source file.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DstFile));
		EnsureDirectory(PathUtils::GetDirectory(config.DstAnimationFile));

		switch (config.Method)
		{
		case MESHBUILD_ASS:
			BuildByASS(config);
			break;
		case MESHBUILD_FBX:
			BuildByFBX(config);
			break;
		case MESHBUILD_D3D:
			BuildByD3D(config);
			break;
		}
	}
}