#pragma once
#ifndef APOC3D_FILESYSTEM_H
#define APOC3D_FILESYSTEM_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace VFS
	{
		/**
		 *  A services class providing file locating, 
		 *  multiple working directories and archive file features.
		 * 
		 *  Ordinarily FileSystem::Locate or FileSystem::TryLocate 
		 *  is used to locate a file. A file name or relative directory
		 *  as well as a FileLocateRule object is needed. When not locating 
		 *  files inside archive, relative directory with
		 *  file names can be directly used with a default FileLocateRule.
		 * 
		 *  On the other hand, when locating files inside archives, 
		 *  FileLocateRule is required or the locating will fail.
		 *  FileLocateRule is a series of check points telling 
		 *  the FileSystem where to find the file.
		 *
		 *  These check points can point to archives, even to archives 
		 *  in archives, while normal directory is supported at the same
		 *  time. It is encouraged to use FileLocateRules, by preparing 
		 *  them at the beginning of your app's Initialization
		 *  (after engine's init).
		 */
		class APAPI FileSystem
		{
			SINGLETON_DECL(FileSystem);
		public:
			FileSystem(void);
			~FileSystem(void);

			/**
			 *  Adds an absolute path as a new working directory. 
			 *
			 *  @remark
			 *   Duplicated dir is not checked.
			 */
			void AddWrokingDirectory(const String& path);
			void PushWrokingDirectory(const String& path);
			void PopWrokingDirectory();


			const String& getWorkingDirectory(int i) const { return m_workingDirs[i]; }
			int getNumWorkingDirectories() const { return m_workingDirs.getCount(); }

			void RegisterArchiveType(ArchiveFactory* factory);			
			bool UnregisterArchiveType(ArchiveFactory* factory);


			bool DirectoryExists(const String& path) const;
			bool DirectoryExists(const String& path, String& result) const;
			bool FindAllExistingDirectoriesSplited(const String& path, List<String>& result, List<String>& archivePath) const;

			void ListDirectoryFiles(const String& path, List<String>& subItems, String* dirPath = nullptr);
			
			Archive* LocateArchive(const String& filePath, const FileLocateRule& rule);
			FileLocation Locate(const String& filePath, const FileLocateRule& rule);
			bool TryLocate(const String& filePath, const FileLocateRule& rule, FileLocation& result);
			bool TryLocate(const String& filePath, const FileLocateRule& rule);


		private:
			struct ArchiveKey
			{
				std::thread::id threadID;
				String filePath;
			};

			struct ArchiveKeyEqualityComparer
			{
				static bool Equals(const ArchiveKey& x, const ArchiveKey& y);
				static int64 GetHashCode(const ArchiveKey& obj);

				static const std::hash<std::thread::id> m_threadIDHasher;
			};

			typedef HashMap<ArchiveKey, Archive*, ArchiveKeyEqualityComparer> PackTable;
			typedef HashMap<String, ArchiveFactory*> PackFactoryTable;

			ArchiveFactory* FindArchiveFactory(const String& ext)
			{
				if (m_factories.getCount() == 0)
					return nullptr;

				ArchiveFactory* fac;
				if (m_factories.TryGetValue(ext, fac))
					return fac;

				return nullptr;
			}

			bool ObtainOpenedArchive(const String& filePath, Archive*& entry);
			void StoreNewArchive(const String& filePath, Archive* arc);

			Archive* CreateArchive(const FileLocation& fl);
			Archive* CreateArchive(const String& file);

			std::mutex m_openedPackMutex;
			PackTable m_openedPack;

			PackFactoryTable m_factories;
			List<String> m_workingDirs;


		};
	}
}
#endif