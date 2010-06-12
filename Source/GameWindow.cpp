#include "GameWindow.h"

namespace Apoc3D
{
	GameWindow::GameWindow(void)
	{
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