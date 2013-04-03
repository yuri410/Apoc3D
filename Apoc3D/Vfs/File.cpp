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

#include "File.h"

#include "apoc3d/Vfs/PathUtils.h"
#include <sys/types.h> 
#include <sys/stat.h> 
#include <io.h>

using namespace Apoc3D::VFS;

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
			struct stat status;

			char buffer[256];
			size_t res;
			wcstombs_s(&res, buffer, path.c_str(), 256);

			if (_access(buffer, 0) == 0)
			{
				stat(buffer, &status);
				
				if (status.st_mode & S_IFDIR)
				{
					return true;
				}
			}			
			return false;
		}
		time_t File::GetFileModifiyTime(const String& path)
		{
			struct stat status;

			char buffer[256];
			size_t res;
			wcstombs_s(&res, buffer, path.c_str(), 256);

			if (_access(buffer, 0) == 0)
			{
				stat(buffer, &status);

				return status.st_mtime;
			}			
			return 0;
		}
		bool File::FileExists(const String& path)
		{
			struct stat status;

			char buffer[256];
			size_t res;
			wcstombs_s(&res, buffer, path.c_str(), 256);

			if (_access(buffer, 0) == 0)
			{
				stat(buffer, &status);
				
				if (status.st_mode & S_IFDIR)
				{
					return false;
				}
				return true;
			}			
			return false;
		}
		int64 File::GetFileSize(const String& path)
		{
			struct stat status;

			char buffer[256];
			size_t res;
			wcstombs_s(&res, buffer, path.c_str(), 256);

			if (_access(buffer, 0) == 0)
			{
				stat(buffer, &status);
				
				if (status.st_mode & S_IFDIR)
				{
					return -1;
				}
				return status.st_size;
			}			
			return -1;
			//ifstream fs(path, ios::binary | ios::in);

			//if (!fs)
			//{
			//	return -1;
			//}
			//
			//fs.seekg(0, ios::end);

			//streampos pos = fs.tellg();

			//fs.close();
			//return pos;
		}

	}
}