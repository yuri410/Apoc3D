#pragma once
#ifndef APOC3D_WIN32_COMMON_H
#define APOC3D_WIN32_COMMON_H

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Core/Plugin.h"

#include <Windows.h>

#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")

namespace Apoc3D
{
	typedef EventDelegate<bool*> CancellableEventHandler;
	typedef EventDelegate<> EventHandler;


	namespace Win32
	{
		PLUGINAPI extern const wchar_t WindowClass[];
		PLUGINAPI extern HWND MainWindowHandle;
		PLUGINAPI extern int32 MainWindowMouseWheel;

		class Win32Clock;
		class Win32Window;

	}
}

#endif