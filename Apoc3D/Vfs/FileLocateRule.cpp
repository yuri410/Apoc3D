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