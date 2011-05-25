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
#include "FileLocateRule.h"

#include "FileSystem.h"

namespace Apoc3D
{
	namespace VFS
	{
		FileLocateRule FileLocateRule::Textures;
		FileLocateRule FileLocateRule::Effects;
		FileLocateRule FileLocateRule::Default;

		void FileLocateRule::Initialize()
		{
			{
				LocateCheckPoint pt;
				pt.AddPath(L"");

				vector<LocateCheckPoint> pts;
				pts.push_back(pt);

				Default = FileLocateRule(pts);

			}
			
			/********************************************************************************/
			{
				LocateCheckPoint pt;
				pt.AddPath(L"textures");

				vector<LocateCheckPoint> pts;
				pts.push_back(pt);

				Textures = FileLocateRule(pts);
			}
			/********************************************************************************/
			{
				LocateCheckPoint pt;
				pt.AddPath(L"effects");

				vector<LocateCheckPoint> pts;
				pts.push_back(pt);

				Effects = FileLocateRule(pts);
			}
		}

		void LocateCheckPoint::AddPath(const String& path)
		{
			vector<String> fullPath;
			vector<String> arcPath;

			if (FileSystem::getSingleton().SplitExistingDirectories(path, fullPath, arcPath))
			{
				for (size_t i = 0; i < fullPath.size(); i++)
				{
					pathList.push_back(Entry(fullPath[i], arcPath[i]));
				}
			}
		}
	}
}