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
#include "Archive.h"

#include "ResourceLocation.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"

namespace Apoc3D
{
	namespace VFS
	{

		static const int PakFileID1 = ((byte)0 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');
		static const int PakFileID2 = ((byte)1 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');

		PakArchive::PakArchive(FileLocation* fl)
			: Archive(fl->getPath(), fl->getSize(), fl->isInArchive()), m_file(fl), m_compression(PCT_None)
		{
			Stream* stream = fl->GetReadStream();
			stream->setPosition(0);

			BinaryReader* br = new BinaryReader(stream);
			
			if (br->ReadInt32() == PakFileID1)
			{
				int fileCount = br->ReadInt32();

				for (int i = 0; i < fileCount; i++)
				{
					PakArchiveEntry ent;

					ent.Name = br->ReadString();
					ent.Offset = br->ReadUInt32();
					ent.Size = br->ReadUInt32();
					br->ReadUInt32();

					m_entries.Add(ent.Name, ent);
				}
			}
			else if (br->ReadInt32() == PakFileID2)
			{
				m_compression = static_cast<PakCompressionType>(br->ReadInt32());
				int fileCount = br->ReadInt32();

				for (int i = 0; i < fileCount; i++)
				{
					PakArchiveEntry ent;

					ent.Name = br->ReadString();
					ent.Offset = br->ReadUInt32();
					ent.Size = br->ReadUInt32();

					m_entries.Add(ent.Name, ent);
				}
			}
			else
			{
				LogManager::getSingleton().Write(LOG_System, L"Pak archive format is invalid " + fl->getPath(), LOGLVL_Warning);
			}
			br->Close();
			delete br;

			m_fileStream = m_file->GetReadStream();
		}
		PakArchive::~PakArchive()
		{
			delete m_fileStream;
			delete m_file;
		}

		void PakArchive::FillEntries(FastList<PakArchiveEntry>& entries)
		{
			for (HashMap<String, PakArchiveEntry>::Enumerator e = m_entries.GetEnumerator();
				e.MoveNext();)
			{
				entries.Add(*e.getCurrentValue());
			}
		}

		int PakArchive::getFileCount() const
		{
			return m_entries.getCount();
		}
		Stream* PakArchive::GetEntryStream(const String& file)
		{
			PakArchiveEntry lpkEnt;

			if (m_entries.TryGetValue(file, lpkEnt))
			{
				//int threadId = Thread.CurrentThread.ManagedThreadId;
				//Stream thStream;

				//if (!threadStreams.TryGetValue(threadId, out thStream))
				//{
				//	thStream = m_file->GetStream();
				//}
				VirtualStream* res = new VirtualStream(m_fileStream, lpkEnt.Offset, lpkEnt.Size);
				res->setPosition( 0 );
				return res;
			}
			return 0;
		}
		bool PakArchive::HasEntry(const String& file)
		{
			return m_entries.Contains(file);
		}
		int64 PakArchive::GetEntrySize(const String& file)
		{
			PakArchiveEntry lpkEnt;

			if (m_entries.TryGetValue(file, lpkEnt))
			{
				return lpkEnt.Size;
			}
			return 0;
		}
		String PakArchive::GetEntryName(int index)
		{
			int i=0;
			HashMap<String, PakArchiveEntry>::Enumerator e = m_entries.GetEnumerator();
			
			e.MoveNext();
			while (i<index)
			{
				e.MoveNext();
				i++;
			}
			assert(e.getCurrentKey());
			return *e.getCurrentKey();
		}

		Archive* PakArchiveFactory::CreateInstance(const String& file)
		{
			return CreateInstance(new FileLocation(file));
		}
	}
}