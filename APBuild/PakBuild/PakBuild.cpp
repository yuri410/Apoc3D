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

#include "../CompileLog.h"
#include "../BuildEngine.h"
#include "../BuildConfig.h"

#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/Archive.h"

#include <dirent.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{

	const int PakFileID = ((byte)0 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');

	//struct PakEntry
	//{
	//	String SourceFile;
	//	String DestFile;
	//};

	//struct PakNode
	//{
	//	List<PakEntry> SourceFiles;
	//	List<PakNode> SubNodes;
	//	String Directory;
	//};
	//PakNode* GetNodes(const PakBuildConfig& config)
	//{
	//	PakNode* result = new PakNode();
	//	
	//	for (int i=0;i<config.Files.getCount();i++)
	//	{
	//		
	//	}

	//	return result;
	//}
	//void Build(const PakNode& node, Stream* dest)
	//{


	//}
	//
	//
	void PakBuild::Build(const ConfigurationSection* sect)
	{

		PakBuildConfig config;
		config.Parse(sect);

		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));
		for (int i=0;i<config.Files.getCount();i++)
		{
			if (!File::FileExists(config.Files[i]))
			{
				CompileLog::WriteError(L"Source file does not exist: " + config.Files[i], config.DestFile);
				return;
			}
		}

		//CompileLog::WriteInformation(L"Pak builder currently only supports directory flatten build.", L"");

		//PakNode* root = GetNodes(config);
		CompileLog::WriteInformation(config.DestFile, L">");

		List<String> sourceFiles;

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->WriteInt32(PakFileID);

		
		sourceFiles.ResizeDiscard(config.Files.getCount());
		for (int i=0;i<config.Files.getCount();i++)
		{
			sourceFiles.Add(config.Files[i]);
		}

		for (int i=0;i<config.Dirs.getCount();i++)
		{
			if (config.Dirs[i].Flatten)
			{
				DIR* dir;
				struct dirent* ent;

				dir = opendir(StringUtils::toString(config.Dirs[i].Path).c_str());
				if (dir)
				{
					int counter = 0;
					while ((ent = readdir(dir)))
					{
						switch (ent->d_type)
						{
						case DT_REG:
							String file = StringUtils::toWString(ent->d_name);
							file = PathUtils::Combine(config.Dirs[i].Path,file);
							sourceFiles.Add(file);
							counter++;
							break;
						}
					}

					closedir(dir);
					CompileLog::WriteInformation(L"Adding " + StringUtils::ToString(counter)
						+ L" files from flatten dir: " + config.Dirs[i].Path, config.DestFile);
				}
				else
				{
					CompileLog::WriteError(L"Cannot read directory: "+config.Dirs[i].Path, config.DestFile);
				}
			}
			else
			{
				CompileLog::WriteError(L"Recursive dir currently not supported: " + config.Dirs[i].Path, config.DestFile);
			}
		}
		int count = sourceFiles.getCount();
		bw->WriteInt32(count);

		int64 oldPos = bw->getBaseStream()->getPosition();

		PakArchiveEntry* entries = new PakArchiveEntry[count];
		for (int i=0;i<count;i++)
		{
			entries[i].Name = PathUtils::GetFileName(sourceFiles[i]);
			bw->WriteString(entries[i].Name);
			bw->WriteUInt32(entries[i].Offset);
			bw->WriteUInt32(entries[i].Size);
			bw->WriteInt32((int32)0);
		}

		for (int i=0;i<count;i++)
		{
			FileStream* fs2 = new FileStream(sourceFiles[i]);
			BinaryReader* br = new BinaryReader(fs2);

			entries[i].Offset = (uint)fs->getPosition();
			entries[i].Size = (uint)fs2->getLength();
			//entries[i].Flag = 0;

			char* buffer = new char[entries[i].Size];
			br->ReadBytes(buffer, entries[i].Size);

			bw->Write(buffer, entries[i].Size);

			delete[] buffer;

			br->Close();
			delete br;
		}

		fs->Seek(oldPos, SEEK_Begin);
		for (int i=0;i<count;i++)
		{
			bw->WriteString(entries[i].Name);
			bw->WriteUInt32(entries[i].Offset);
			bw->WriteUInt32(entries[i].Size);
			bw->WriteInt32((int32)0);
		}
		

		bw->Close();
		delete bw;
	}
}