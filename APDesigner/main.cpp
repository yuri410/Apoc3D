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

#include "APDCommon.h"

#ifndef APOC3D_DYNLIB
#include "Apoc3D.D3D9RenderSystem/Plugin.h"
#include "Apoc3D.WindowsInput/Plugin.h"
#endif

#include "MainWindow.h"
#include "ServWindow.h"

#include <direct.h>
#include <Windows.h>
#include <ShellAPI.h>

#include <SDKDDKVer.h>

using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;
using namespace Apoc3D::Math;
using namespace APDesigner;

struct StartupParameters
{
	bool ServiceMode;
	String ProjectFile;

	StartupParameters() : ServiceMode(false) { }

	void Parse(wchar_t** argv, int count);
};


INT WINAPI wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	DWORD len = GetCurrentDirectory(0, NULL);

	wchar_t* workingDir = new wchar_t[len+1];
	workingDir[len] = 0;
	GetCurrentDirectory(len, workingDir);


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
	escon.TextureCacheSize = 512 * 1048576;
	escon.ModelCacheSize = 192 * 1048576;
#if _DEBUG
	escon.WriteLogToStd = true;
#endif

	delete[] workingDir;

	wchar_t exePath[260];
	GetModuleFileName(0, exePath, 260);
	_chdir(StringUtils::toPlatformNarrowString(PathUtils::GetDirectory(exePath)).c_str());

	Engine::Initialize(&escon);

	PakArchiveFactory* pakSupport = new PakArchiveFactory();
	FileSystem::getSingleton().RegisterArchiveType(pakSupport);

	int numOfArgs;
	LPWSTR* argv = CommandLineToArgvW(lpCmdLine, &numOfArgs);
	
	StartupParameters startupParams;
	startupParams.Parse(argv, numOfArgs);
	LocalFree(argv);

	RenderParameters params;
	params.ColorBufferFormat = FMT_X8R8G8B8;
	params.DepthBufferFormat = DEPFMT_Depth24Stencil8;
	params.EnableVSync = true;
	params.FSAASampleCount = 4;
	params.IsFullForm = true;
	params.IsWindowed = true;
	params.IgnoreMonitorChange = true;

	if (!startupParams.ServiceMode)
	{
		DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

		params.BackBufferHeight = 720;
		params.BackBufferWidth = 1280;

		RenderView* view =  devContent->Create(params);

		RenderWindow* wnd = up_cast<RenderWindow*>(view);

		//MainWindow* mainWnd = new MainWindow(wnd);
		wnd->setEventHandler(new MainWindow(wnd));

		if (wnd)
		{
			wnd->Run();
			delete wnd;
		}

		delete devContent;
	}
	else
	{
		if (!File::FileExists(startupParams.ProjectFile))
		{
			MessageBox(0, (L"Can not find the project file:\n" + startupParams.ProjectFile).c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION);
			return 0;
		}

		DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

		params.BackBufferHeight = 300;
		params.BackBufferWidth = 220;
		params.IsFixedWindow = true;

		RenderView* view =  devContent->Create(params);

		RenderWindow* wnd = up_cast<RenderWindow*>(view);
		
		wnd->setEventHandler(new ServWindow(wnd, startupParams.ProjectFile));

		if (wnd)
		{
			wnd->Run();
			delete wnd;
		}

		delete devContent;
	}


	FileSystem::getSingleton().UnregisterArchiveType(pakSupport);
	delete pakSupport;

	Engine::Shutdown();

#ifndef APOC3D_DYNLIB
	delete input;
	delete d3d;
#endif

	return 0;
}


void StartupParameters::Parse(wchar_t** argv, int count)
{
	for (int i=0;i<=count;i++)
	{
		if (!argv[i])
			break;

		if (!_wcsicmp(argv[i], L"-serv"))
		{
			ProjectFile = argv[++i];
			ServiceMode = true;
		}
	}
}