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
#include "PakBuild.h"

#include "BuildSystem.h"
#include "BuildConfig.h"

#include <dirent.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{

	void PakBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		PakBuildConfig config;
		config.Parse(sect);

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));
		for (const String& fn : config.Files)
		{
			if (!File::FileExists(fn))
			{
				BuildSystem::LogError(L"Source file does not exist: " + fn, config.DestFile);
				return;
			}
		}

		List<String> sourceFiles;
		sourceFiles.AddList(config.Files);

		for (const PakBuildConfig::PakDirEntryConfig& dc : config.Dirs)
		{
			if (dc.Flatten)
			{
				List<String> entries;
				File::ListDirectoryFiles(dc.Path, entries);

				for (const String& fn : entries)
				{
					sourceFiles.Add(PathUtils::Combine(dc.Path, fn));
				}

				if (entries.getCount() > 0)
				{
					BuildSystem::LogInformation(L"Adding " + StringUtils::IntToString(entries.getCount())
						+ L" files from flatten dir: " + dc.Path, config.DestFile);
				}
			}
			else
			{
				BuildSystem::LogError(L"Recursive dir currently not supported: " + dc.Path, config.DestFile);
			}
		}

		PakArchive::Pack(FileOutStream(config.DestFile), sourceFiles);

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}
}