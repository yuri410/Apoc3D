#pragma once

#ifndef GAMECOMMON_H
#define GAMECOMMON_H

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

#include "apoc3d/Engine.h"

#pragma comment(lib, "Apoc3D.lib")
#pragma comment(lib, "Apoc3D.D3D9RenderSystem.lib")
#pragma comment(lib, "Apoc3D.WindowsInput.lib")
#pragma comment(lib, "Apoc3D.Essentials.lib")

// Forward Declarations
namespace SampleTerrain
{
	class SampleFont;
	class GameCamera;
	class Terrain;
	class TerrainMesh;
	class TerrainMeshManager;
	class SharedIndexData;


};

using namespace Apoc3D;

#endif
