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
#include "Win32InputFactory.h"
#include "Win32Mouse.h"
#include "Win32Keyboard.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			Win32InputFactory* Win32InputFactory::m_instance = 0;

			APIDescription Win32InputFactory::GetDescription()
			{
				PlatformAPISupport platform = { 100, L"WINDOWS" };

				APIDescription desc;
				desc.Name = L"Win32 Input";
				desc.SupportedPlatforms.Add(platform);
				return desc;
			}
			void Win32InputFactory::Initialize(RenderWindow* window)
			{
				m_tempClientSizeParam = window->getClientSize();
				m_tempTitleParam = window->getTitle();

				EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProcStatic, 0);
				if (m_hwnd)
				{
					OIS::ParamList pl;

					pl.insert({ "WINDOW", StringUtils::UIntToNarrowString(reinterpret_cast<uintptr>(m_hwnd)) });

					m_InputManager = OIS::InputManager::createInputSystem(pl);
					m_InputManager->enableAddOnFactory(OIS::InputManager::AddOn_All);
				}
			}

			BOOL Win32InputFactory::EnumWindowsProc(
				_In_  HWND hwnd,
				_In_  LPARAM lParam)
			{
				WINDOWINFO info;
				info.cbSize = sizeof(WINDOWINFO);
				GetWindowInfo(hwnd, &info);

				if (m_tempClientSizeParam.Width == info.rcClient.right - info.rcClient.left &&
					m_tempClientSizeParam.Height == info.rcClient.bottom - info.rcClient.top)
				{
					int len = GetWindowTextLength(hwnd);
					len += 8;
					wchar_t* title = new wchar_t[len];
					GetWindowText(hwnd, title, len);

					if (String(title)==m_tempTitleParam)
					{
						delete[] title;
						wchar_t buffer[40];
						GetClassName(hwnd, buffer, 40);
						if (StringUtils::StartsWith(String(buffer), L"d5325676b0844be1a06964bc3f6603ec"))
						{
							m_hwnd = hwnd;

							return FALSE;
						}
						return TRUE;
					}

					delete[] title;
				}
				return TRUE;
			}

			Mouse* Win32InputFactory::CreateMouse()
			{
				return new OldSchoolMouse(m_hwnd);
			}
			Keyboard* Win32InputFactory::CreateKeyboard()
			{
				return new Win32Keyboard(m_InputManager);
			}
		}
	}
}