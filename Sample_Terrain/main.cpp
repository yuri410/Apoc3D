/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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

#include "TerrainCommon.h"

#include "TerrainDemo.h"

#include "Math/RandomUtils.h"

#include "Graphics/RenderSystem/GraphicsAPI.h"
#include "Graphics/RenderSystem/DeviceContext.h"
#include "Graphics/RenderSystem/RenderWindow.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/PathUtils.h"
#include "Utility/StringUtils.h"

#include "Apoc3D.D3D9RenderSystem/Plugin.h"
#include "Apoc3D.WindowsInput/Plugin.h"

#include "Math/Matrix.h"

#include <direct.h>
#include <Windows.h>

#include <SDKDDKVer.h>

using namespace SampleTerrain;

using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT cmdShow)
{
	// the following code is the usual and typical way to initialize the engine for an render window

	wchar_t workingDir[260];
	DWORD len = GetCurrentDirectory(260, workingDir);

	// manually add these plugins, since the library is built statically.
	// dynamic lib loading is not practical
	Plugin* input = new Apoc3D::Input::Win32::WinInputPlugin();
	Plugin* d3d = new Apoc3D::Graphics::D3D9RenderSystem::D3D9RSPlugin();

	// fill up the engine's initialization parameters
	ManualStartConfig escon;
	//escon.PluginDyList.Add(L"Apoc3D.D3D9RenderSystem");
	escon.PluginList.Add(input);
	escon.PluginList.Add(d3d);
	escon.WorkingDirectories.Add(workingDir);
	escon.TextureCacheSize = 512 * 1048576;
	escon.ModelCacheSize = 192 * 1048576;
#if _DEBUG
	escon.WriteLogToStd = true;
#endif
	escon.ModelAsync = true;
	escon.TextureAsync = true;

	// get the exe's path, add this path to the working directories
	wchar_t exePath[260];
	GetModuleFileName(0, exePath, 260);
	_chdir(StringUtils::toString(PathUtils::GetDirectory(exePath)).c_str());
	escon.WorkingDirectories.Add(PathUtils::GetDirectory(exePath));

	/* Initialization */
	Engine::Initialize(&escon);

	// pak file support should be manually added to the file system. Apoc3DEx (Apoc3D.Essentials) will use it for the 'system.pak' file.
	PakArchiveFactory* pakSupport = new PakArchiveFactory();
	FileSystem::getSingletonPtr()->RegisterArchiveType(pakSupport);

	// now move on the creation of render window
	DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

	// fill the params
	RenderParameters params;
	params.BackBufferHeight = 720;
	params.BackBufferWidth = 1280;
	params.ColorBufferFormat = FMT_X8R8G8B8;
	params.DepthBufferFormat = DEPFMT_Depth24Stencil8;
	params.EnableVSync = true;
	params.FSAASampleCount = 0;
	params.IsFullForm = true;
	params.IsWindowd = true;


	RenderView* view =  devContent->Create(params);

	RenderWindow* wnd = dynamic_cast<RenderWindow*>(view);
	// once created, set the application's main class as the event handler, 
	// so that the window's events will be passes to demo code
	wnd->setEventHandler(new TerrainDemo(wnd));

	if (wnd)
	{
		// this will enter the main loop. the execution will not leave the Run method until
		// the renderWindow closes.
		wnd->Run();

		// clean up then the window closed
		delete wnd;
	}

	delete devContent;

	// unregister pakSupport before deleting it.
	FileSystem::getSingletonPtr()->UnregisterArchiveType(pakSupport);
	delete pakSupport;

	Engine::Shutdown();

	delete input;
	delete d3d;

	return 0;
}
