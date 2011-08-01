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
#include "PakBuild.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace APBuild
{

	static const int PakFileID = ((byte)0 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');

	struct PakEntry
	{
		String SourceFile;
		String DestPath;
	};

	void Build(const std::vector<PakEntry>& entries, Stream* dest)
	{
		
	}

	void PakBuild::Build(const ConfigurationSection* sect)
	{

		PakBuildConfig config;
		config.Parse(sect);

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->Write(PakFileID);

		
		int count = config.Files.getCount();
		for (int i=0;i<config.Dirs.getCount();i++)
		{
			if (config.Dirs[i].Flatten)
			{
				
			}
		}

		int64 oldPos = bw->getBaseStream()->getPosition();

		

		bw->Close();
		delete bw;
	}
}