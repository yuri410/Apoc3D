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
#ifndef FILELOCATERULE_H
#define FILELOCATERULE_H

#include "Common.h"

using namespace std;

namespace Apoc3D
{	
	namespace VFS
	{
		/** Contains several paths to look when searching file
		*/
		class APAPI LocateCheckPoint
		{
		private:
			struct Entry
			{
				String Path;
				String ArchivePath;

				Entry(const String& path, const String& ap)
				{
					Path = path;
					ArchivePath = ap;
				}
			};

			vector<Entry> pathList;
			
		public:	
			/** [Obsolete] Indicates whether the file system should search for file in standard archive set when using this rule
			*/
			bool SearchesCurrectArchiveSet;

			LocateCheckPoint() : SearchesCurrectArchiveSet(false) { }

			/** Add a check point path
			*/ 
			void AddPath(const String& path);

			void Clear()
			{
				pathList.clear();
			}


			/** Check if check point at index is pointed to a archvie file
			*/
			bool hasArchivePath(int index) const
			{
				return !!pathList[index].ArchivePath.length();
			}

			/** Get the path of the check point at index
			*/
			const String& GetPath(int index) const
			{
				return pathList[index].Path;
			}

=			/** Get the archive path of the check point at index
			*/
			const String& GetArchivePath(int index) const
			{
				return pathList[index].ArchivePath;
			}

			/** Get the number of check point in this rule.
			*/
			int getCount() const
			{
				return pathList.size();
			}

		};

		/** Defines the rule used for locate files in FileSystem.
		* The rule is a sequence of directory/archive to look for the file.
		*/
		class APAPI FileLocateRule
		{
		public:
			static void Initialize();

			static FileLocateRule Textures;
			static FileLocateRule Effects;
			static FileLocateRule Default;


			vector<LocateCheckPoint> pathChkPt;

			FileLocateRule()
			{
			}

			FileLocateRule(vector<LocateCheckPoint> checkPoints)
				: pathChkPt(checkPoints)
			{
			}

			void AddCheckPoint(const LocateCheckPoint& coll)
			{
				pathChkPt.push_back(coll);
			}

			int getCount() const
			{
				return pathChkPt.size();
			}
			LocateCheckPoint getCheckPoint(int index) const
			{
				return pathChkPt[index];
			}
		};
	}
}
#endif