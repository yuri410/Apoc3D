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

#include "BuildEngine.h"

#include "apoc3d/Core/Logging.h"

#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/File.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#include "D3DHelper.h"
#include <GdiPlus.h>

using namespace APBuild;
using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;
using namespace Gdiplus;

static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR           gdiplusToken;

void NewLog(LogEntry e);

int Initialize()
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		wcout << L"DevIL version is different...!\n";
		return 1;
	}

	ilInit();
	iluInit();

	D3DHelper::Initalize();


	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	LogManager::Initialize();
	LogManager::getSingleton().eventNewLogWritten().bind(NewLog);

	return 0;
}
void Finalize()
{
	GdiplusShutdown(gdiplusToken);
	LogManager::Finalize();
	D3DHelper::Finalize();
}

void EnsureDirectory(const String& path)
{
	std::vector<String> paths = PathUtils::Split(path);
	String subPath;
	subPath.reserve(path.size());
	for (size_t i=0;i<paths.size();i++)
	{
		PathUtils::Append(subPath, paths[i]);

		if (!File::DirectoryExists(subPath))
		{
			CreateDirectory(subPath.c_str(), 0);
		}
	}

}

void NewLog(LogEntry e)
{
	wcout << e.ToString() << endl;
}