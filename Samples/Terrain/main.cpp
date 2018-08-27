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

#include "TerrainCommon.h"

#include "TerrainDemo.h"

#include <direct.h>
#include <Windows.h>

#include <SDKDDKVer.h>

using namespace SampleTerrain;

using namespace std;

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT cmdShow)
{
	// the following code is the usual and typical way to initialize the engine for an render window

	wchar_t workingDir[260];
	GetCurrentDirectory(260, workingDir);


	// fill up the engine's initialization parameters
	ManualStartConfig escon;
	
#ifndef APOC3D_DYNLIB
	// manually add these plugins, since the library is built statically.
	Plugin* input = new Apoc3D::Input::Win32::WinInputPlugin();
	Plugin* d3d = new Apoc3D::Graphics::D3D9RenderSystem::D3D9RSPlugin();
	escon.PluginList.Add(input);
	escon.PluginList.Add(d3d);
#else
	escon.PluginDynLibList.Add(L"Apoc3D.D3D9RenderSystem.dll");
	escon.PluginDynLibList.Add(L"Apoc3D.WindowsInput.dll");
#endif
	
	escon.WorkingDirectories.Add(workingDir);
	escon.WorkingDirectories.Add(PathUtils::Combine(workingDir, L".."));

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
	_chdir(StringUtils::toPlatformNarrowString(PathUtils::GetDirectory(exePath)).c_str());
	escon.WorkingDirectories.Add(PathUtils::GetDirectory(exePath));

	/* Initialization */
	Engine::Initialize(&escon);

	// pak file support should be manually added to the file system. Apoc3DEx (Apoc3D.Essentials) will use it for the 'system.pak' file.
	PakArchiveFactory* pakSupport = new PakArchiveFactory();
	FileSystem::getSingleton().RegisterArchiveType(pakSupport);

	// now move on the creation of render window
	DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

	//List<RenderDisplayMode> modes = devContent->GetSupportedDisplayModes();

	// fill the params
	RenderParameters params;
	params.BackBufferHeight = 720;
	params.BackBufferWidth = 1280;
	params.ColorBufferFormat = FMT_X8R8G8B8;
	params.DepthBufferFormat = DEPFMT_Depth24Stencil8;
	params.EnableVSync = true;
	params.FSAASampleCount = 0;
	params.IsFullForm = true;
	params.IsWindowed = true;
	params.IgnoreMonitorChange = true;
	
	RenderView* view =  devContent->Create(params);

	RenderWindow* wnd = up_cast<RenderWindow*>(view);
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
	FileSystem::getSingleton().UnregisterArchiveType(pakSupport);
	delete pakSupport;

	Engine::Shutdown();

#ifndef APOC3D_DYNLIB
	delete input;
	delete d3d;
#endif

	return 0;
}
