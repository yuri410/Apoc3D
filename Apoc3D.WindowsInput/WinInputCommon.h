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

#ifndef WININPUTCOMMON_H
#define WININPUTCOMMON_H

#include "apoc3d/ApocCommon.h"

#pragma warning ( push )
#pragma warning ( disable:4512 )

#include <ois-v1-3/includes/OIS.h>
#include <ois-v1-3/includes/OISInputManager.h>
#include <ois-v1-3/includes/OISException.h>
#include <ois-v1-3/includes/OISMouse.h>
#include <ois-v1-3/includes/OISKeyboard.h>
#include <ois-v1-3/includes/OISJoyStick.h>
#include <ois-v1-3/includes/OISEvents.h>

#pragma warning (pop)


#include <Windows.h>

#pragma comment(lib, "Apoc3D.lib")
#pragma comment(lib, "Apoc3D.Win32.lib")

#ifdef _DEBUG
#	ifdef APOC3D_MT
#		pragma comment(lib, "OIS_static_d_mt.lib")
#	else
#		pragma comment(lib, "OIS_static_d.lib")
#	endif
#else
#	ifdef APOC3D_MT
#		pragma comment(lib, "OIS_static_mt.lib")
#	else
#		pragma comment(lib, "OIS_static.lib")
#	endif
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