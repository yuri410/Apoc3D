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
#ifndef GL1COMMON_H
#define GL1COMMON_H

#include "apoc3d/Common.h"

#include <Windows.h>

#include "gl/glew.h"
#include "gl/wglew.h"

#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")

namespace Apoc3D
{
	typedef EventDelegate<bool*> CancellableEventHandler;
	typedef EventDelegate<> EventHandler;

	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class Game;
			class GraphicsDeviceManager;

			class GL1RenderView;
			class GL1RenderWindow;

			class GL1DeviceContext;
			
			class GL1ObjectFactory;
			class GL1RenderDevice;
			class GL1Capabilities;
			class GL1RenderStateManager;
			class GL1RenderTarget;

			class NativeGL1StateManager;

			class GL1Texture;
			class GL1Sprite;
			
		}
	}
}
#endif