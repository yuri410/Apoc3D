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

#include "Win32InputFactory.h"
#include "Win32Mouse.h"
#include "Win32Keyboard.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d.Win32/Win32Common.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Core;

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

				m_hwnd = Apoc3D::Win32::MainWindowHandle;

				if (m_hwnd == NULL)
				{
					EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProcStatic, 0);
				}
				else
				{
					ApocLog(LOG_System, L"Direct Input Window found.", LOGLVL_Infomation);
				}

				if (m_hwnd == NULL)
				{
					// find again with relaxed rule
					m_relaxedWindowFinding = true;
					m_largestWindowSize = 0;
					EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProcStatic, 0);
				}

				if (m_hwnd == NULL)
				{
					m_hwnd = GetConsoleWindow();
				}

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

				int32 wndWidth = info.rcClient.right - info.rcClient.left;
				int32 wndHeight = info.rcClient.bottom - info.rcClient.top;

				if (m_relaxedWindowFinding)
				{
					int32 area = wndWidth*wndHeight;
					if (area > m_largestWindowSize)
					{
						m_largestWindowSize = area;
						m_hwnd = hwnd;
					}

					return TRUE;
				}

				if (m_tempClientSizeParam.Width == wndWidth &&
					m_tempClientSizeParam.Height == wndHeight)
				{
					int len = GetWindowTextLength(hwnd);
					len += 8;
					wchar_t* title = new wchar_t[len];
					GetWindowText(hwnd, title, len);

					String strTitle = title;
					delete[] title;

					if (strTitle == m_tempTitleParam)
					{
						wchar_t buffer[40];
						GetClassName(hwnd, buffer, 40);
						if (StringUtils::StartsWith(String(buffer), Apoc3D::Win32::WindowClass))
						{
							m_hwnd = hwnd;

							return FALSE;
						}
						return TRUE;
					}
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