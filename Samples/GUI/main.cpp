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

#include "GUIDemo.h"

#include "DemoCommon.h"

#include <direct.h>
#include <Windows.h>

#include <SDKDDKVer.h>

using namespace Sample_GUI;

using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT cmdShow)
{
	wchar_t workingDir[260];
	DWORD len = GetCurrentDirectory(260, workingDir);


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
	escon.ModelAsync = false;
	escon.TextureAsync = false;
	

	wchar_t exePath[260];
	GetModuleFileName(0, exePath, 260);
	_chdir(StringUtils::toPlatformNarrowString(PathUtils::GetDirectory(exePath)).c_str());
	escon.WorkingDirectories.Add(PathUtils::GetDirectory(exePath));

	Engine::Initialize(&escon);

	PakArchiveFactory* pakSupport = new PakArchiveFactory();
	FileSystem::getSingleton().RegisterArchiveType(pakSupport);

	DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

	RenderParameters params;
	params.BackBufferHeight = 768;
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
	
	wnd->setEventHandler(new GUIDemo(wnd));

	if (wnd)
	{
		wnd->Run();
		delete wnd;
	}

	delete devContent;

	FileSystem::getSingleton().UnregisterArchiveType(pakSupport);
	delete pakSupport;

	Engine::Shutdown();

#ifndef APOC3D_DYNLIB
	delete input;
	delete d3d;
#endif

	return 0;
}
