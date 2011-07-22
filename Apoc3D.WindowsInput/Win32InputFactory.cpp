/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

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
			APIDescription Win32InputFactory::GetDescription()
			{
				PlatformAPISupport platform = { 100, L"WINDOWS" };

				APIDescription desc;
				desc.Name = L"Win32 Input";
				desc.SupportedPlatforms.push_back(platform);
				return desc;
			}
			void Win32InputFactory::Initialize(RenderWindow* window)
			{
				HWND hwnd = GetForegroundWindow();
				if (hwnd)
				{
					OIS::ParamList pl;


					std::ostringstream wnd;
					wnd << (size_t)hwnd;
					pl.insert(make_pair("WINDOW", wnd.str()));

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
				HWND hwnd = GetForegroundWindow();
				return new Win32Mouse(hwnd);
			}
			Keyboard* Win32InputFactory::CreateKeyboard()
			{
				return new Win32Keyboard(m_InputManager);
			}
		}
	}
}