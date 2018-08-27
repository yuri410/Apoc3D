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