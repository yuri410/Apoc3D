#ifndef WININPUTCOMMON_H
#define WININPUTCOMMON_H

#include "Common.h"

#define PLUGIN __declspec(dllexport)

#include <ois-v1-3/includes/OIS.h>


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