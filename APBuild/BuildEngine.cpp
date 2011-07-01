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

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#include "D3DHelper.h"
#include <GdiPlus.h>

using namespace APBuild;
using namespace Gdiplus;

static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR           gdiplusToken;

int Initialize()
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		printf("DevIL version is different...exiting!\n");
		return 1;
	}

	ilInit();
	iluInit();

	D3DHelper::Initalize();


	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	return 0;
}
void Finalize()
{
	GdiplusShutdown(gdiplusToken);
	D3DHelper::Finalize();
}