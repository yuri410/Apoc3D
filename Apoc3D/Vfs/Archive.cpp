/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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
		const uint32 PakFileID1 = FourCC("PAK");
		const uint32 PakFileID2 = FourCC("PAK2");

		enum Pak3Flags
		{
			Pak2_64 = 1 << 0
		};

		PakArchive::PakArchive(const FileLocation& fl)
			: Archive(fl.getPath(), fl.getSize(), fl.isInArchive())
		{
			BinaryReader br(fl);
			uint32 fid = br.ReadUInt32();

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

			bw.WriteUInt32(PakFileID2);
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
						bw.WriteBytes(buffer, count);
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