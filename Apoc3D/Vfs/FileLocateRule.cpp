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
			Default = { { L"" } };
			Textures = { { L"textures", L"textures.pak" } };
			Effects = { { L"effects", L"effects.pak" } };
			Materials = { { L"materials", L"materials.pak" } };
		}

		LocateCheckPoint::LocateCheckPoint(std::initializer_list<String> list)
		{
			for (const String& e : list)
			{
				AddPath(e);
			}
		}

		void LocateCheckPoint::AddPath(const String& path)
		{
			List<String> fullPath;
			List<String> arcPath;

			if (FileSystem::getSingleton().FindAllExistingDirectoriesSplited(path, fullPath, arcPath))
			{
				for (int32 i = 0; i < fullPath.getCount(); i++)
				{
					m_pathList.Add(Entry(fullPath[i], arcPath[i]));
				}
			}
		}
	}
}