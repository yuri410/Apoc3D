#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GL3COMMON_H
#define GL3COMMON_H

#include "apoc3d/ApocCommon.h"

#include <Windows.h>

#include "GL/gl3w.h"

#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")

namespace Apoc3D
{
	typedef EventDelegate<bool*> CancellableEventHandler;
	typedef EventDelegate<> EventHandler;

	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class Game;
			class GraphicsDeviceManager;

			class GL3RenderView;
			class GL3RenderWindow;

			class GL3DeviceContext;
			
			class GL3ObjectFactory;
			class GL3RenderDevice;
			class GL3Capabilities;
			class GL3RenderStateManager;
			class GL3RenderTarget;

			class NativeGL1StateManager;

			class GL3Texture;
			class GL3Sprite;
			
		}
	}
}
#endif