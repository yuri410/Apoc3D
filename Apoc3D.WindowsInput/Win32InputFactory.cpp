#include "Win32InputFactory.h"
#include "Apoc3DException.h"
#include "Win32Mouse.h"
#include "Win32Keyboard.h"
#include <Windows.h>

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{

			void Win32InputFactory::Initialize(RenderWindow* window)
			{
				HWND hwnd = GetForegroundWindow();
				if (hwnd)
				{
					OIS::ParamList pl;
					pl.insert(make_pair("WINDOW", hwnd));

					m_InputManager = OIS::InputManager::createInputSystem(pl);
					m_InputManager->enableAddOnFactory(OIS::InputManager::AddOn_All);
				}
				else
				{
					throw Apoc3DException::createException(EX_InvalidData, L"The Input System requires an active window to be initialized.");
				}
			}
			Mouse* Win32InputFactory::CreateMouse()
			{
				
			}
			Keyboard* Win32InputFactory::CreateKeyboard()
			{

			}
		}
	}
}