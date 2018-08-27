#pragma once
#ifndef APOC3D_FILE_H
#define APOC3D_FILE_H

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

#include "apoc3d/ApocCommon.h"

namespace Apoc3D
{
	namespace VFS
	{
		/**
		 *  Represent the properties of a file. Also provides some
		 *  cross-platform utility function on file system.
		 */
		class APAPI File
		{
		protected:
			bool m_isInArchive;
			int64 m_fileSize;
			String m_fileName;
			String m_filePath;

			File(const String& file, int64 size, bool isInPack);
			
		public:
			bool isInArchive() const { return m_isInArchive; }
			int64 getFileSize() const { return m_fileSize; }
			const String& getFileName() const { return m_fileName; }
			const String& getDirectory() const { return m_filePath; }

			static int64 GetFileSize(const std::string& path);
			static bool FileExists(const std::string& path);
			static time_t GetFileModifiyTime(const std::string& path);
			static bool DirectoryExists(const std::string& path);

			static int64 GetFileSize(const String& path);
			static bool FileExists(const String& path);
			static time_t GetFileModifiyTime(const String& path);
			static bool DirectoryExists(const String& path);

			static bool SetFileModifiyTime(const String& path, time_t mt);


			static bool ListDirectoryFiles(const String& path, Apoc3D::Collections::List<String>& items);
			static bool ListDirectoryFilesRecursive(const String& path, Apoc3D::Collections::List<String>& items, int32 maxDepth = -1);

		};
	}
}


#endif
