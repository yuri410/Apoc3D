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
#include "DIP1.h"

#include "DIPCommon.h"

#include <direct.h>
#include <Windows.h>

#include <SDKDDKVer.h>

using namespace dip;

using namespace std;

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


	RenderView* view =  devContent->Create(params);

	RenderWindow* wnd = up_cast<RenderWindow*>(view);
	
	wnd->setEventHandler(new DIP1(wnd));

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
