#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of labtd

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

#ifndef GAMECOMMON_H
#define GAMECOMMON_H

#include "apoc3d/Engine.h"

#pragma comment(lib, "Apoc3D.lib")
#pragma comment(lib, "Apoc3D.D3D9RenderSystem.lib")
#pragma comment(lib, "Apoc3D.WindowsInput.lib")
#pragma comment(lib, "Apoc3D.Essentials.lib")

// Forward Declarations
namespace SampleTerrain
{
	typedef fastdelegate::FastDelegate0<void> EventHandler;
	typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;

	class TerrainDemo;
	class GameCamera;
	class Terrain;
	class TerrainMesh;
	class TerrainMeshManager;
	class SharedIndexData;


};

using namespace Apoc3D;

#endif
