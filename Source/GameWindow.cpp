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
#include "GameWindow.h"
#include "Resource.h"

namespace Apoc3D
{
	//
	//  函数: MyRegisterClass()
	//
	//  目的: 注册窗口类。
	//
	//  注释:
	//
	//    仅当希望
	//    此代码与添加到 Windows 95 中的“RegisterClassEx”
	//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
	//    这样应用程序就可以获得关联的
	//    “格式正确的”小图标。
	//
	ATOM GameWindow::MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (&GameWindow::WndProc);
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABTD));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= wndClass;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassEx(&wcex);
	}
	
	//
	//   函数: InitInstance(HINSTANCE, int)
	//
	//   目的: 保存实例句柄并创建主窗口
	//
	//   注释:
	//
	//        在此函数中，我们在全局变量中保存实例句柄并
	//        创建和显示主程序窗口。
	//
	BOOL GameWindow::InitInstance(HINSTANCE hInstance, int nCmdShow,
		const TCHAR* const &wndClass, const TCHAR* const &wndTitle)
	{
	   HWND hWnd;

	   hWnd = CreateWindow(wndClass, wndTitle, WS_OVERLAPPEDWINDOW,
		  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	   if (!hWnd)
	   {
		  return FALSE;
	   }
	   m_hWnd = hWnd;

	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);

	   return TRUE;
	}

	GameWindow::GameWindow(HINSTANCE hInstance, int nCmdShow, 
		const TCHAR* const &wndClass, const TCHAR* const &wndTitle)
	{
		MyRegisterClass(hInstance, wndClass);

		assert (!InitInstance (hInstance, nCmdShow, wndClass, wndTitle));							
	}

	GameWindow::~GameWindow(void)
	{
	}
	
	void GameWindow::UpdateMonitor()
	{
		HMONITOR windowMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
		if (!m_currentMonitor || windowMonitor!=m_currentMonitor)
		{
			m_currentMonitor=windowMonitor;
			if (m_currentMonitor)
				OnMonitorChanged();
		}
	}

	void GameWindow::OnUserResized()
	{
		INVOKE(m_eUserResized)();
	}
	void GameWindow::OnSuspend()
	{
		INVOKE(m_eSuspend)();
	}
	void GameWindow::OnApplicationActivated()
	{
		INVOKE(m_eApplicationActivated)();
	}
	void GameWindow::OnApplicationDeactivated()
	{
		INVOKE(m_eApplicationDeactivated)();
	}
	void GameWindow::OnSystemSuspend()
	{
		INVOKE(m_eSystemSuspend)();
	}
	void GameWindow::OnSystemResume()
	{
		INVOKE(m_eSystemResume)();
	}
	void GameWindow::OnScreensaver(bool * cancel)
	{
		INVOKE(m_eScreensaver)(cancel);
	}
	void GameWindow::OnPaint()
	{
		INVOKE(m_ePaint)();
	}
	void GameWindow::OnMonitorChanged()
	{
		INVOKE(m_eMonitorChanged)();
	}

	Size GameWindow::getCurrentSize()
	{
		RECT rect;
		if (!GetClientRect(m_hWnd, &rect))
		{
			rect.left = 0; rect.right = 0;
			rect.bottom = 0;
			rect.top = 0;
		}
		return Size(rect.right - rect.left, rect.bottom - rect.top);
	}

	LRESULT CALLBACK GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
            {
				m_minimized = true;
				m_maximized = false;

				OnSuspend();
			}
			else
			{
				RECT rect;
				if (!GetClientRect(hWnd, &rect))
				{
					rect.left = 0; rect.right = 0;
					rect.bottom = 0;
					rect.top = 0;
				}
				
				if (rect.bottom - rect.top == 0)
				{
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_minimized = false;
					m_maximized = true;

					OnUserResized();
					UpdateMonitor();
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (m_minimized)
						OnResume();

					m_minimized = false;
					m_maximized = false;

					Size newSize = getCurrentSize();
					if (!m_inSizeMove && m_cachedSize != newSize)
					{
						OnUserResized();

						UpdateMonitor();
						m_cachedSize = newSize;
					}
				}
			}
			break;
		case WM_ACTIVATEAPP:
			if (wParam)
			{
				OnApplicationActivated();
			}
			else
			{
				OnApplicationDeactivated();
			}
			break;
		case WM_POWERBROADCAST:
			if (wParam == PBT_APMQUERYSUSPEND)
			{
				OnSystemSuspend();	
				return 1;
			}
			else if (wParam == PBT_APMRESUMESUSPEND)
			{
				OnSystemResume();
				return 1;
			}
			break;
		case WM_SYSCOMMAND:
		{
			long wp = wParam & 0xFFF0;
			if (wp == SC_MONITORPOWER || wp == SC_SCREENSAVE)
			{
				bool cancel;
				OnScreensaver(&cancel);
				if (cancel)
				{
					return 0;
				}
			}
			break;
		}
		case WM_ENTERSIZEMOVE:
			m_inSizeMove = true;
            m_cachedSize = getCurrentSize();
            OnSuspend();
			break;
		case WM_EXITSIZEMOVE:
			 // check for screen and size changes
            OnUserResized();
			UpdateMonitor();
            m_inSizeMove = false;

            // resume application processing
            OnResume();
			break;
		case WM_PAINT:
			OnPaint();
			break;
		}
	
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}