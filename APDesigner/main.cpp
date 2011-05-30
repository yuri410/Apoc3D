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
#include "APDCommon.h"

#include "Graphics/RenderSystem/GraphicsAPI.h"
#include "Graphics/RenderSystem/DeviceContent.h"
#include "Graphics/RenderSystem/RenderWindow.h"

#include <Windows.h>

#include <SDKDDKVer.h>

using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;


INT WINAPI wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	ManualStartConfig escon;
	escon.PluginList.push_back(L"Apoc3D.D3D9RenderSystem");

	Engine::Initialize(&escon);

	DeviceContent* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContent();

	RenderParameters params;
	params.BackBufferHeight = 720;
	params.BackBufferWidth = 1280;
	params.ColorBufferFormat = FMT_X8R8G8B8;
	params.DepthBufferFormat = DEPFMT_Depth24Stencil8;
	params.EnableVSync = true;
	params.FSAASampleCount = 4;
	params.IsFullForm = true;
	params.IsWindowd = true;


	RenderView* view =  devContent->Create(params);

	RenderWindow* wnd = dynamic_cast<RenderWindow*>(view);

	if (wnd)
	{
		wnd->Run();
		delete wnd;
	}

	delete devContent;

	Engine::Shutdown();

	return 0;
}


