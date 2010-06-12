#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#pragma once
#include "Common.h"

namespace Apoc3D
{
	class GameWindow
	{
	private:
		bool m_minimized;
		bool m_maximized;
		bool m_inSizeMove;
		HINSTANCE m_hInst;

		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow,
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		ATOM MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
	public:
		GameWindow(HINSTANCE hInstance, int nCmdShow, 
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		~GameWindow(void);
	};
}
#endif