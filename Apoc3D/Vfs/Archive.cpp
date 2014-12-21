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
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/Vfs/PathUtils.h"

namespace Apoc3D
{
	namespace VFS
	{
		const int PakFileID1 = ((byte)0 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');
		//const int PakFileID2 = ((byte)1 << 24) | ((byte)'P' << 16) | ((byte)'A' << 8) | ((byte)'K');
		const int PakFileID2 = 'PAK2';

		enum Pak3Flags
		{
			Pak2_64 = 1 << 0
		};

		PakArchive::PakArchive(const FileLocation& fl)
			: Archive(fl.getPath(), fl.getSize(), fl.isInArchive())
		{
			BinaryReader br(fl);
			int32 fid = br.ReadInt32();

			if (fid == PakFileID1)
			{
				int fileCount = br.ReadInt32();

				for (int i = 0; i < fileCount; i++)
				{
					PakArchiveEntry ent;

					ent.Name = br.ReadString();
					ent.Offset = br.ReadUInt32();
					ent.Size = br.ReadUInt32();
					br.ReadUInt32();

					m_entries.Add(ent.Name, ent);
					m_entryNames.Add(ent.Name);
				}
			}
			else if (fid == PakFileID2)
			{
				int flags = br.ReadInt32();
				br.ReadInt32();
				br.ReadInt32();

				int fileCount = br.ReadInt32();

				if (flags & Pak2_64)
				{
					for (int i = 0; i < fileCount; i++)
					{
						PakArchiveEntry ent;

						ent.Name = br.ReadString();
						ent.Offset = br.ReadInt64();
						ent.Size = br.ReadInt64();

						m_entries.Add(ent.Name, ent);
						m_entryNames.Add(ent.Name);
					}
				}
				else
				{
					for (int i = 0; i < fileCount; i++)
					{
						PakArchiveEntry ent;

						ent.Name = br.ReadString();
						ent.Offset = br.ReadUInt32();
						ent.Size = br.ReadUInt32();

						m_entries.Add(ent.Name, ent);
						m_entryNames.Add(ent.Name);
					}
				}
			}
			else
			{
				LogManager::getSingleton().Write(LOG_System, L"Pak archive format is invalid " + fl.getPath(), LOGLVL_Warning);
			}
			
			m_fileStream = fl.GetReadStream();
		}
		PakArchive::~PakArchive()
		{
			delete m_fileStream;
		}

		void PakArchive::FillEntries(List<PakArchiveEntry>& entries)
		{
			m_entries.FillValues(entries);
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
				VirtualStream* res = new VirtualStream(m_fileStream, lpkEnt.Offset, lpkEnt.Size);
				res->setPosition( 0 );
				return res;
			}
			return nullptr;
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

		String PakArchive::GetEntryName(int index) { return m_entryNames[index]; }

		void PakArchive::Pack(Stream& outStrm, const List<String>& sourceFiles)
		{
			const int32 count = sourceFiles.getCount();
			PakArchiveEntry* entries = new PakArchiveEntry[count];
			int64 totalSize = 0;
			int64 maxItemSize = 0;
			for (int i = 0; i < count; i++)
			{
				const String& fn = sourceFiles[i];
				entries[i].Name = PathUtils::GetFileName(sourceFiles[i]);

				int64 fs = File::GetFileSize(fn);
				if (fs > maxItemSize)
					maxItemSize = fs;

				totalSize += fs;
			}

			bool use64Bit = (totalSize > MaxUInt32 || maxItemSize > MaxUInt32);

			int32 flags = 0;
			if (use64Bit)
				flags |= Pak2_64;

			BinaryWriter bw(&outStrm, false);

			bw.WriteInt32(PakFileID2);
			bw.WriteInt32(flags);
			bw.WriteInt32(0);
			bw.WriteInt32(0);

			bw.WriteInt32(count);

			int64 oldPos = outStrm.getPosition();

			if(use64Bit)
			{
				for (int i = 0; i < count; i++)
				{
					bw.WriteString(entries[i].Name);
					bw.WriteInt64(entries[i].Offset);
					bw.WriteInt64(entries[i].Size);
				}
			}
			else
			{
				for (int i = 0; i < count; i++)
				{
					bw.WriteString(entries[i].Name);
					bw.WriteUInt32((uint32)0);
					bw.WriteUInt32((uint32)0);
				}
			}

			const int bufferSize = 4096;
			char* buffer = new char[bufferSize];

			for (int i = 0; i < count; i++)
			{
				FileStream fs2(sourceFiles[i]);
				
				entries[i].Offset = outStrm.getPosition();
				entries[i].Size = fs2.getLength();

				for (;;)
				{
					int64 count = fs2.Read(buffer, bufferSize);

					if (count > 0)
					{
						bw.Write(buffer, count);
					}
					else break;
				}
			}
			delete[] buffer;

			outStrm.Seek(oldPos, SeekMode::Begin);

			if (use64Bit)
			{
				for (int i = 0; i < count; i++)
				{
					bw.WriteString(entries[i].Name);
					bw.WriteInt64(entries[i].Offset);
					bw.WriteInt64(entries[i].Size);
				}
			}
			else
			{
				for (int i = 0; i < count; i++)
				{
					bw.WriteString(entries[i].Name);
					bw.WriteUInt32((uint32)(entries[i].Offset & 0xffffffffU));
					bw.WriteUInt32((uint32)(entries[i].Size & 0xffffffffU));
				}
			}
			
			delete[] entries;
		}


		Archive* PakArchiveFactory::CreateInstance(const String& file)
		{
			return CreateInstance(FileLocation(file));
		}
	}
}