#pragma once
#ifndef APOC3D_ARCHIVE_H
#define APOC3D_ARCHIVE_H

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

#include "File.h"

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace VFS
	{
		/**
		 *  Abstract factory that create archive objects.
		 *	One archive object keeps the archive file open during its lifetime.
		 */
		class APAPI ArchiveFactory
		{
		public:
			virtual Archive* CreateInstance(const String& file) = 0;
			virtual Archive* CreateInstance(const FileLocation& fl) = 0;

			virtual String getExtension() const = 0;
		};

		/** Define interface for accessing pack files, like .zip or custom packs. */
		class APAPI Archive : public File
		{
		protected:
			Archive(const String& file, int64 size, bool isInArchive)
				: File(file, size, isInArchive)
			{

			}
		public:
			virtual int getFileCount() const = 0;
			virtual Stream* GetEntryStream(const String& file) = 0;
			virtual int64 GetEntrySize(const String& file) = 0;
			virtual bool HasEntry(const String& file) = 0;
			virtual String GetEntryName(int index) = 0;
		};

		struct PakArchiveEntry
		{
			String Name;
			int64 Offset;
			int64 Size;
		};

		// the engine has built in support for a kind of uncompressed pak file.
		class APAPI PakArchive : public Archive
		{
		public:
			PakArchive(const FileLocation& fl);
			~PakArchive();

			void FillEntries(List<PakArchiveEntry>& entries);

			virtual int getFileCount() const;
			virtual Stream* GetEntryStream(const String& file);
			virtual bool HasEntry(const String& file);
			virtual int64 GetEntrySize(const String& file);
			virtual String GetEntryName(int index);

			static void Pack(Stream& outStrm, const List<String>& sourceFiles);
			/*enum PakCompressionType
			{
				PCT_None,
				PCT_RLEPerEntry
			};*/

		private:
			HashMap<String, PakArchiveEntry> m_entries;
			List<String> m_entryNames;

			Stream* m_fileStream;
			//PakCompressionType m_compression;
		};

		class APAPI PakArchiveFactory : public ArchiveFactory
		{
		public:
			Archive* CreateInstance(const String& file);
			Archive* CreateInstance(const FileLocation& fl) { return new PakArchive(fl); }

			String getExtension() const { return L"pak"; }
		};

	}
}

#endif