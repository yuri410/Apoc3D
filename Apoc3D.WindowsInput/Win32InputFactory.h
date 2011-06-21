#ifndef WIN32INPUTFACTORY_H
#define WIN32INPUTFACTORY_H

#include "WinInputCommon.h"
#include "Input/InputAPI.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class Win32InputFactory : public InputAPIFactory
			{
			private:
				OIS::InputManager* m_InputManager;

			public:
				virtual void Initialize(RenderWindow* window);
				virtual Mouse* CreateMouse();
				virtual Keyboard* CreateKeyboard();
			};
		}
	}
}

#endif