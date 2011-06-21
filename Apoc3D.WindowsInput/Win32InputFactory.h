#ifndef WIN32INPUTFACTORY_H
#define WIN32INPUTFACTORY_H

#include "WinInputCommon.h"
#include "Input/InputAPI.h"

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class Win32InputFactory : public InputAPIFactory
			{
			public:
				virtual Mouse* CreateMouse();
				virtual Keyboard* CreateKeyboard();
			};
		}
	}
}

#endif