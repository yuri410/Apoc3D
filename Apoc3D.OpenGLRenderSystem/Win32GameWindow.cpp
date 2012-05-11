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
#include "Win32GameWindow.h"

#include "Resource.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			Win32GameWindow* Win32GameWindow::ms_Window = 0;

			ATOM Win32GameWindow::MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass)
			{
				WNDCLASSEX wcex;

				wcex.cbSize = sizeof(WNDCLASSEX);

				wcex.style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (Win32GameWindow::WndProcStatic);
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
			//   ����: InitInstance(HINSTANCE, int)
			//
			//   Ŀ��: ����ʵ�����������������
			//
			//   ע��:
			//
			//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
			//        ��������ʾ�����򴰿ڡ�
			//
			BOOL Win32GameWindow::InitInstance(HINSTANCE hInstance, 
				const TCHAR* const &wndClass, const TCHAR* const &wndTitle)
			{
				HWND hWnd;
				
				m_title = wndTitle;

				hWnd = CreateWindow(wndClass, wndTitle, WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, 0, 1280, 720, NULL, NULL, hInstance, NULL);

				if (!hWnd)
				{
					return FALSE;
				}
				m_hWnd = hWnd;

				m_currentMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);

				ShowWindow(hWnd, SW_NORMAL);
				UpdateWindow(hWnd);

				RECT rect;
				GetClientRect(hWnd, &rect);
				int clientWidth = rect.right - rect.left;
				int clientHeight = rect.bottom - rect.top;

				int scrnWidth = GetSystemMetrics(SM_CXFULLSCREEN);   
				int scrnHeight = GetSystemMetrics(SM_CYFULLSCREEN);

				SetWindowPos(hWnd, 0, 
					(scrnWidth - clientWidth)>>1, (scrnHeight - clientHeight)>>1, 
					clientWidth, clientHeight, SWP_NOZORDER);



				return TRUE;
			}

			Win32GameWindow::Win32GameWindow(const String& wndClass, const String& wndTitle)
				: m_mouseWheel(0)
			{
				m_hInstance= GetModuleHandle (0);
				ms_Window = this;

				m_className = wndClass;
				m_title = wndTitle;
			}

			Win32GameWindow::~Win32GameWindow(void)
			{
				if (ms_Window == this)
					ms_Window = NULL;
			}

			void Win32GameWindow::Load()
			{
				MyRegisterClass(m_hInstance, m_className.c_str());

				if (!InitInstance (m_hInstance, m_className.c_str(), m_title.c_str()))
				{
					assert(FALSE);
				}
			}

			void Win32GameWindow::UpdateMonitor()
			{
				HMONITOR windowMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
				if (!m_currentMonitor || windowMonitor!=m_currentMonitor)
				{
					m_currentMonitor=windowMonitor;
					if (m_currentMonitor)
						OnMonitorChanged();
				}
			}

			void Win32GameWindow::OnUserResized()
			{
				if (!m_eUserResized.empty())
					m_eUserResized();
			}
			void Win32GameWindow::OnSuspend()
			{
				if (!m_eSuspend.empty())
					m_eSuspend();
			}
			void Win32GameWindow::OnResume()
			{
				if (!m_eResume.empty())
					m_eResume();
			}
			void Win32GameWindow::OnApplicationActivated()
			{
				if (!m_eApplicationActivated.empty())
					m_eApplicationActivated();
			}
			void Win32GameWindow::OnApplicationDeactivated()
			{
				if (!m_eApplicationDeactivated.empty())
					m_eApplicationDeactivated();
			}
			void Win32GameWindow::OnSystemSuspend()
			{
				if (!m_eSystemSuspend.empty())
					m_eSystemSuspend();
			}
			void Win32GameWindow::OnSystemResume()
			{
				if (!m_eSystemResume.empty())
					m_eSystemResume();
			}
			void Win32GameWindow::OnScreensaver(bool * cancel)
			{
				if (!m_eScreensaver.empty())
					m_eScreensaver(cancel);
			}

			void Win32GameWindow::OnPaint()
			{
				if (!m_ePaint.empty())
					m_ePaint();
			}
			void Win32GameWindow::OnMonitorChanged()
			{
				if (!m_eMonitorChanged.empty())
					m_eMonitorChanged();
			}

			Size Win32GameWindow::getCurrentSize() const
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

			void Win32GameWindow::setWindowTitle(const String& txt)
			{
				m_title = txt;
				SetWindowText(m_hWnd, txt.c_str());
			}

			void Win32GameWindow::Close()
			{

				DestroyWindow(m_hWnd);
			}

			LRESULT CALLBACK Win32GameWindow::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				if (ms_Window)
				{
					return ms_Window->WndProc(hWnd, message, wParam, lParam);
				}
				return 0;
			}

			LRESULT Win32GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
					m_mouseWheel += (int16) (wParam >> 0x10);
					break;
				}

				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
}