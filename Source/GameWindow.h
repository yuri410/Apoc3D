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

	public:
		GameWindow(void);
		~GameWindow(void);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
	};
}
#endif