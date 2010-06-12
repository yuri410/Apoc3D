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
		wcex.lpfnWndProc	= GameWindow::WndProc;
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

	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);

	   return TRUE;
	}

	GameWindow::GameWindow(HINSTANCE hInstance, int nCmdShow, 
		const TCHAR* const &wndClass, const TCHAR* const &wndTitle)
	{
		MyRegisterClass(hInstance, wndClass);

		if (!InitInstance (hInstance, nCmdShow, wndClass, wndTitle))
		{
			
		}
	}


	GameWindow::~GameWindow(void)
	{
	}

	LRESULT CALLBACK GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_SIZE)
        {
			if (wParam == SIZE_MINIMIZED)
            {
				m_minimized = true;
				m_maximized = false;
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
				}
				else if (wParam == SIZE_RESTORED)
				{
					m_minimized = false;
					m_maximized = false;

					if (!m_inSizeMove)
					{

					}
				}
			}
		}
		else if (message == WM_ACTIVATEAPP)
		{
			if (wParam)
			{
			}
			else
			{}
		}
		else if (message == WM_POWERBROADCAST)
		{
			if (wParam == PBT_APMQUERYSUSPEND)
			{
				
			}
			else if (wParam == PBT_APMRESUMESUSPEND)
			{

			}
		}
		else if (message == WM_SYSCOMMAND)
		{
			long wp = wParam & 0xFFF0;
			if (wp == SC_MONITORPOWER || wp == SC_SCREENSAVE)
			{

			}
		}

	
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}