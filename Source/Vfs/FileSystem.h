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
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Common.h"
#include "Core/Singleton.h"

using namespace Apoc3D::Core;
using namespace std;

namespace Apoc3D
{
	namespace VFS
	{
		//template class APAPI vector<String>;
		//template class APAPI unordered_map<String, Archive*>;
		//template class APAPI unordered_map<String, ArchiveFactory*>;

		class APAPI FileSystem : public Singleton<FileSystem>
		{
		private:
			typedef unordered_map<String, Archive*> PackTable;
			typedef unordered_map<String, ArchiveFactory*> PackFactoryTable;

			PackTable m_openedPack;
			PackFactoryTable m_factories;
			vector<String> m_workingDirs;

			
			ArchiveFactory* FindArchiveFactory(const String& ext)
			{
				if (m_factories.empty())
					return 0;

				PackFactoryTable::const_iterator iter = m_factories.find(ext);

				if (iter != m_factories.end())
				{
					return iter->second;
				}
				return 0;
			}

			bool IsOpened(String filePath, Archive** entry) const
			{
				PackTable::const_iterator iter = m_openedPack.find(filePath);
				
				if (iter != m_openedPack.end())
				{
					(*entry) = iter->second;
					return true;
				}
				return false;
			}
			Archive* CreateArchive(FileLocation* fl);
			Archive* CreateArchive(const String& file);

		public:
			FileSystem(void) { }
			~FileSystem(void);

			//static void Initialize();

			void AddWrokingDirectory(const String& path);
			const String& getWorkingDirectory(int i) const { return m_workingDirs[i]; }
			int getNumWorkingDirectories() const { return (int)m_workingDirs.size(); }

			void RegisterArchiveType(ArchiveFactory* factory);			
			bool UnregisterArchiveType(ArchiveFactory* factory);


			bool DirectoryExists(const String& path) const;
			bool DirectoryExists(const String& path, String& result) const;
			bool SplitExistingDirectories(const String& path, vector<String>& result, vector<String>& archivePath) const;

			vector<String> SearchFile(const String& path);
			
			Archive* LocateArchive(const String& filePath, const FileLocateRule& rule);
			FileLocation* Locate(const String& filePath, const FileLocateRule& rule);
			FileLocation* TryLocate(const String& filePath, const FileLocateRule& rule);

			SINGLETON_DECL_HEARDER(FileSystem);
		};
	}
}
#endif