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

#include "File.h"

#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/utime.h>
#include <io.h>
#include <dirent.h>

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS

#else

#endif

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;
using namespace std;

namespace Apoc3D
{
	namespace VFS
	{
		File::File(const String& file, int64 size, bool isInPack)
			: m_fileSize(size), m_isInArchive(isInPack)
		{
			PathUtils::SplitFilePath(file, m_fileName, m_filePath);
		}


		bool File::DirectoryExists(const String& path)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return DirectoryExists(spath);
		}
		time_t File::GetFileModifiyTime(const String& path)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return GetFileModifiyTime(spath);
		}
		bool File::FileExists(const String& path)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return FileExists(spath);
		}
		int64 File::GetFileSize(const String& path)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return GetFileSize(spath);
		}


		int64 File::GetFileSize(const std::string& path)
		{
			struct stat status;
			if (_access(path.c_str(), 0) == 0)
			{
				stat(path.c_str(), &status);

				if (status.st_mode & S_IFDIR)
				{
					return 0;
				}
				return status.st_size;
			}
			return 0;
		}
		bool File::FileExists(const std::string& path)
		{
			struct stat status;
			if (_access(path.c_str(), 0) == 0)
			{
				stat(path.c_str(), &status);

				if (status.st_mode & S_IFDIR)
				{
					return false;
				}
				return true;
			}
			return false;
		}
		time_t File::GetFileModifiyTime(const std::string& path)
		{
			struct stat status;
			if (_access(path.c_str(), 0) == 0)
			{
				stat(path.c_str(), &status);

				return status.st_mtime;
			}
			return 0;
		}
		bool File::DirectoryExists(const std::string& path)
		{
			struct stat status;
			if (_access(path.c_str(), 0) == 0)
			{
				stat(path.c_str(), &status);

				if (status.st_mode & S_IFDIR)
				{
					return true;
				}
			}
			return false;
		}

		bool File::SetFileModifiyTime(const String& path, time_t mt)
		{
			std::string spath = Utility::StringUtils::toPlatformNarrowString(path);

			struct stat path_stat;

			if (stat(spath.c_str(), &path_stat))
				return false;

			utimbuf buf;
			buf.actime = path_stat.st_atime;
			buf.modtime = mt;
			if (utime(spath.c_str(), &buf))
				return false;
			return true;
		}


		bool ListDirectoryFilesRecursiveGeneric(const char* path, const String& basePath, List<String>& items, int32 depth, int32 maxDepth)
		{
			DIR *dir;
			struct dirent *ent;

			if ((dir = opendir(path)) != NULL)
			{
				while ((ent = readdir(dir)) != NULL)
				{
					if (ent->d_type & S_IFREG)
					{
						if (basePath.size())
							items.Add(PathUtils::Combine(basePath, StringUtils::toPlatformWideString(ent->d_name)));
						else
							items.Add(StringUtils::toPlatformWideString(ent->d_name));
					}
					else if (ent->d_type & S_IFDIR)
					{
						if ((depth + 1 < maxDepth || maxDepth < 0) && 
							strcmp(".", ent->d_name) != 0 && 
							strcmp("..", ent->d_name) != 0)
						{
							String subPath = PathUtils::Combine(basePath, StringUtils::toPlatformWideString(ent->d_name));
							std::string spath = PathUtils::Combine(path, ent->d_name);

							ListDirectoryFilesRecursiveGeneric(spath.c_str(), subPath, items, depth + 1, maxDepth);
						}
					}
				}
				closedir(dir);
				return true;
			}
			return false;
		}

		bool ListDirectoryFilesRecursiveGeneric(const char* path, const std::string& basePath, List<std::string>& items, int32 depth, int32 maxDepth)
		{
			DIR* dir;
			struct dirent* ent;

			if ((dir = opendir(path)) != NULL)
			{
				while ((ent = readdir(dir)) != NULL)
				{
					if (ent->d_type & S_IFREG)
					{
						if (basePath.size())
							items.Add(PathUtils::Combine(basePath, ent->d_name));
						else
							items.Add(ent->d_name);
					}
					else if (ent->d_type & S_IFDIR)
					{
						if ((depth + 1 < maxDepth || maxDepth < 0) &&
							strcmp(".", ent->d_name) != 0 &&
							strcmp("..", ent->d_name) != 0)
						{
							std::string subPath = PathUtils::Combine(basePath, ent->d_name);
							std::string spath = PathUtils::Combine(path, ent->d_name);

							ListDirectoryFilesRecursiveGeneric(spath.c_str(), subPath, items, depth + 1, maxDepth);
						}
					}
				}
				closedir(dir);
				return true;
			}
			return false;
		}

		bool File::ListDirectoryFiles(const String& path, List<String>& items)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return ListDirectoryFilesRecursiveGeneric(spath.c_str(), L"", items, 0, 0);
		}
		bool File::ListDirectoryFilesRecursive(const String& path, Apoc3D::Collections::List<String>& items, int32 maxDepth)
		{
			std::string spath = StringUtils::toPlatformNarrowString(path);

			return ListDirectoryFilesRecursiveGeneric(spath.c_str(), L"", items, 0, maxDepth);
		}

		bool File::ListDirectoryFiles(const std::string& path, List<std::string>& items)
		{
			return ListDirectoryFilesRecursiveGeneric(path.c_str(), "", items, 0, 0);
		}
		bool File::ListDirectoryFilesRecursive(const std::string& path, Apoc3D::Collections::List<std::string>& items, int32 maxDepth)
		{
			return ListDirectoryFilesRecursiveGeneric(path.c_str(), "", items, 0, maxDepth);
		}
	}
}