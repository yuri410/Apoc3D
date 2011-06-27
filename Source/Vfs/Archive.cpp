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
#include "Archive.h"

#include "ResourceLocation.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryReader.h"

namespace Apoc3D
{
	namespace VFS
	{

		static const int PakFileID = ((byte)0 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');

		PakArchive::PakArchive(FileLocation* fl)
			: Archive(fl->getPath(), fl->getSize(), fl->isInArchive()), m_file(fl)
		{
			Stream* stream = fl->GetReadStream();
			stream->setPosition(0);

			BinaryReader* br = new BinaryReader(stream);
			
			br->Close();
			delete br;
		}
		PakArchive::~PakArchive()
		{

		}

		void PakArchive::FillEntries(FastList<PakArchiveEntry>& entries)
		{
			
		}

		int PakArchive::getFileCount() const
		{

		}
		Stream* PakArchive::GetEntryStream(const String& file)
		{

		}

		Archive* PakArchiveFactory::CreateInstance(const String& file)
		{
			return CreateInstance(new FileLocation(file));
		}
	}
}