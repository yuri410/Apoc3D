#ifndef WININPUTCOMMON_H
#define WININPUTCOMMON_H

#include "Common.h"

#define PLUGIN __declspec(dllexport)

#include <ois-v1-3/includes/OIS.h>
#include <ois-v1-3/includes/OISInputManager.h>
#include <ois-v1-3/includes/OISException.h>
#include <ois-v1-3/includes/OISMouse.h>
#include <ois-v1-3/includes/OISKeyboard.h>
#include <ois-v1-3/includes/OISJoyStick.h>
#include <ois-v1-3/includes/OISEvents.h>

#pragma comment(lib, "Apoc3D.lib")

#ifdef _DEBUG
#pragma comment(lib, "OIS_static_d.lib")
#else
#pragma comment(lib, "OIS_static.lib")
#endif

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class WinInputPlugin;
			class Win32InputFactory;
			class Win32Mouse;
			class Win32Keyboard;
		}
	}
}


#endif