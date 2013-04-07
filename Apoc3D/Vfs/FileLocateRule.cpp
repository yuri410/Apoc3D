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
#include "FileLocateRule.h"

#include "FileSystem.h"

namespace Apoc3D
{
	namespace VFS
	{
		FileLocateRule FileLocateRule::Textures;
		FileLocateRule FileLocateRule::Effects;
		FileLocateRule FileLocateRule::Default;
		FileLocateRule FileLocateRule::Materials;

		void FileLocateRule::Initialize()
		{
			{
				LocateCheckPoint pt;
				pt.AddPath(L"");

				List<LocateCheckPoint> pts;
				pts.Add(pt);

				Default = FileLocateRule(pts);

			}
			
			/********************************************************************************/
			{
				LocateCheckPoint pt;
				pt.AddPath(L"textures");
				pt.AddPath(L"textures.pak");

				List<LocateCheckPoint> pts;
				pts.Add(pt);

				Textures = FileLocateRule(pts);
			}
			/********************************************************************************/
			{
				LocateCheckPoint pt;
				pt.AddPath(L"effects");
				pt.AddPath(L"effects.pak");

				List<LocateCheckPoint> pts;
				pts.Add(pt);

				Effects = FileLocateRule(pts);
			}
			/********************************************************************************/
			{
				LocateCheckPoint pt;
				pt.AddPath(L"materials");
				pt.AddPath(L"materials.pak");

				List<LocateCheckPoint> pts;
				pts.Add(pt);

				Materials = FileLocateRule(pts);
			}
		}

		void LocateCheckPoint::AddPath(const String& path)
		{
			List<String> fullPath;
			List<String> arcPath;

			if (FileSystem::getSingleton().SplitExistingDirectories(path, fullPath, arcPath))
			{
				for (int32 i = 0; i < fullPath.getCount(); i++)
				{
					pathList.Add(Entry(fullPath[i], arcPath[i]));
				}
			}
		}
	}
}