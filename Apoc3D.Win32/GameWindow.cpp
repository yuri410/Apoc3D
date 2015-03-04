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
#include "GameWindow.h"

namespace Apoc3D
{
	namespace Win32
	{
		GameWindow* GameWindow::ms_Window = nullptr;

		GameWindow::GameWindow(const String& wndClass, const String& title)
			: m_className(wndClass), m_title(title)
		{
			m_hInstance = GetModuleHandle(0);
			ms_Window = this;
		}

		GameWindow::~GameWindow()
		{
			if (ms_Window == this)
				ms_Window = NULL;
		}

		void GameWindow::Load(int32 width, int32 height, bool fixed)
		{
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = (GameWindow::WndProcStatic);
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = m_hInstance;
			wcex.hIcon = LoadIcon(m_hInstance, L"AppIcon");
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = m_className.c_str();
			wcex.hIconSm = LoadIcon(m_hInstance, L"AppSmallIcon");
			RegisterClassEx(&wcex);

			if (!InitInstance(m_hInstance, width, height, fixed, m_className, m_title))
			{
				assert(FALSE);
			}
		}

		// creates a window with the given dimension and other options
		BOOL GameWindow::InitInstance(HINSTANCE hInstance, int32 width, int32 height, bool fixed, const String& wndClass, const String& wndTitle)
		{
			m_title = wndTitle;

			RECT dstRect;

			int scrnWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			int scrnHeight = GetSystemMetrics(SM_CYFULLSCREEN);

			dstRect.left = (scrnWidth - width) / 2;
			dstRect.top = (scrnHeight - height) / 2;
			dstRect.right = dstRect.left + width;
			dstRect.bottom = dstRect.top + height;

			DWORD style = WS_OVERLAPPEDWINDOW;
			if (fixed)
			{
				style &= (~WS_MAXIMIZEBOX);
				style &= (~WS_SIZEBOX);
			}

			AdjustWindowRect(&dstRect, style, FALSE);

			HWND hWnd = CreateWindow(wndClass.c_str(), wndTitle.c_str(), style,
				dstRect.left, dstRect.top, dstRect.right - dstRect.left, dstRect.bottom - dstRect.top,
				NULL, NULL, hInstance, NULL);

			if (!hWnd)
			{
				return FALSE;
			}
			m_hWnd = hWnd;

			m_currentMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);


			//RECT rect;
			//GetClientRect(hWnd, &rect);
			//int clientWidth = rect.right - rect.left;
			//int clientHeight = rect.bottom - rect.top;




			ShowWindow(hWnd, SW_NORMAL);
			//UpdateWindow(hWnd);


			return TRUE;
		}

		void GameWindow::UpdateMonitor()
		{
			HMONITOR windowMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
			if (!m_currentMonitor || windowMonitor != m_currentMonitor)
			{
				m_currentMonitor = windowMonitor;
				if (m_currentMonitor)
					OnMonitorChanged();
			}
		}

		void GameWindow::OnUserResized() { eventUserResized.Invoke(); }
		void GameWindow::OnSuspend() { eventSuspend.Invoke(); }
		void GameWindow::OnResume() { eventResume.Invoke(); }
		void GameWindow::OnApplicationActivated() { eventApplicationActivated.Invoke(); }
		void GameWindow::OnApplicationDeactivated() { eventApplicationDeactivated.Invoke(); }
		void GameWindow::OnSystemSuspend() { eventSystemSuspend.Invoke(); }
		void GameWindow::OnSystemResume() { eventSystemResume.Invoke(); }
		void GameWindow::OnScreensaver(bool* cancel) { eventScreensaver.Invoke(cancel); }

		void GameWindow::OnPaint() { eventPaint.Invoke(); }
		void GameWindow::OnMonitorChanged() { eventMonitorChanged.Invoke(); }

		Size GameWindow::getCurrentSize() const
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

		void GameWindow::setWindowTitle(const String& txt)
		{
			m_title = txt;
			SetWindowText(m_hWnd, txt.c_str());
		}

		void GameWindow::Close()
		{

			DestroyWindow(m_hWnd);
		}
		void GameWindow::MakeFixedSize(bool v)
		{
			RECT oldRect;
			GetClientRect(m_hWnd, &oldRect);

			LONG style = GetWindowLong(m_hWnd, GWL_STYLE);

			style &= ~WS_CLIPSIBLINGS;

			if (!v)
			{
				style |= WS_MAXIMIZEBOX;
				style |= WS_SIZEBOX;
			}
			else
			{
				style &= (~WS_MAXIMIZEBOX);
				style &= (~WS_SIZEBOX);
			}

			SetWindowLong(m_hWnd, GWL_STYLE, style);

			SetWindowPos(m_hWnd, 0, oldRect.left, oldRect.top, oldRect.right - oldRect.left, oldRect.bottom - oldRect.top, SWP_NOZORDER);
		}

		LRESULT CALLBACK GameWindow::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if (ms_Window)
				return ms_Window->WndProc(hWnd, message, wParam, lParam);
			return 0;
		}

		LRESULT GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			//return DefWindowProc(hWnd, message, wParam, lParam);
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

							m_cachedSize = getCurrentSize();
							//RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
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

								//RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
							}
						}
					}
					break;
				case WM_MOVE:

					//RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
					break;
				case WM_PAINT:
					OnPaint();
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
				case WM_DESTROY:
					UnregisterClass(m_className.c_str(), m_hInstance);

					PostQuitMessage(0);
					break;
					//case WM_MOUSELEAVE:
					//	ShowCursor(TRUE);
					//	break;
				case WM_MOUSEWHEEL:
					m_mouseWheel += (int16)(wParam >> 0x10);
					break;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}