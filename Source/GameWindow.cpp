#include "GameWindow.h"
#include "Resource.h"

namespace Apoc3D
{
	//
	//  ����: MyRegisterClass()
	//
	//  Ŀ��: ע�ᴰ���ࡣ
	//
	//  ע��:
	//
	//    ����ϣ��
	//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
	//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
	//    ����Ӧ�ó���Ϳ��Ի�ù�����
	//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
	//   ����: InitInstance(HINSTANCE, int)
	//
	//   Ŀ��: ����ʵ�����������������
	//
	//   ע��:
	//
	//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
	//        ��������ʾ�����򴰿ڡ�
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