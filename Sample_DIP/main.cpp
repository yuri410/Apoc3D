/*
-----------------------------------------------------------------------------
This source file is part of labtd

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
#include "DIP1.h"

#include "DIPCommon.h"

#include "apoc3d/Math/RandomUtils.h"

#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"

#include "Apoc3D.D3D9RenderSystem/Plugin.h"
#include "Apoc3D.WindowsInput/Plugin.h"

#include "apoc3d/Math/Matrix.h"

#include <direct.h>
#include <Windows.h>

#include <SDKDDKVer.h>

using namespace dip;

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

	Plugin* input = new Apoc3D::Input::Win32::WinInputPlugin();
	Plugin* d3d = new Apoc3D::Graphics::D3D9RenderSystem::D3D9RSPlugin();

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
	escon.ModelAsync = false;
	escon.TextureAsync = false;
	

	wchar_t exePath[260];
	GetModuleFileName(0, exePath, 260);
	_chdir(StringUtils::toPlatformNarrowString(PathUtils::GetDirectory(exePath)).c_str());
	escon.WorkingDirectories.Add(PathUtils::GetDirectory(exePath));

	Engine::Initialize(&escon);

	PakArchiveFactory* pakSupport = new PakArchiveFactory();
	FileSystem::getSingletonPtr()->RegisterArchiveType(pakSupport);

	DeviceContext* devContent =  GraphicsAPIManager::getSingleton().CreateDeviceContext();

	RenderParameters params;
	params.BackBufferHeight = 768;
	params.BackBufferWidth = 1280;
	params.ColorBufferFormat = FMT_X8R8G8B8;
	params.DepthBufferFormat = DEPFMT_Depth24Stencil8;
	params.EnableVSync = true;
	params.FSAASampleCount = 0;
	params.IsFullForm = true;
	params.IsWindowd = true;


	RenderView* view =  devContent->Create(params);

	RenderWindow* wnd = dynamic_cast<RenderWindow*>(view);
	
	wnd->setEventHandler(new DIP1(wnd));

	if (wnd)
	{
		wnd->Run();
		delete wnd;
	}

	delete devContent;

	FileSystem::getSingletonPtr()->UnregisterArchiveType(pakSupport);
	delete pakSupport;

	Engine::Shutdown();

	delete input;
	delete d3d;

	return 0;
}