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
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	/* Implements a specialized window for games and rendering.
	*/
	class APOC3D_API GameWindow
	{
	private:
		bool m_minimized;
		bool m_maximized;
		bool m_inSizeMove;

		Size m_cachedSize;
		HINSTANCE m_hInst;

		HWND m_hWnd;
		HINSTANCE m_hInstance;
		int m_nCmdShow;
		HMONITOR m_currentMonitor;

		const wchar_t* m_className;
		const wchar_t* m_wndTitle;

		EventHandler m_ePaint;
		EventHandler m_eResume;
		EventHandler m_eUserResized;
		EventHandler m_eSuspend;
		EventHandler m_eApplicationActivated;
		EventHandler m_eApplicationDeactivated;
		EventHandler m_eSystemSuspend;
		EventHandler m_eSystemResume;
		EventHandler m_eMonitorChanged;
		CancellableEventHandler m_eScreensaver;

		int32 m_mouseWheel;

		static GameWindow* ms_Window;

		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow,
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		ATOM MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass);


		static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void UpdateMonitor();
	protected:
		virtual void OnUserResized();
		virtual void OnSuspend();
		virtual void OnApplicationActivated();
		virtual void OnApplicationDeactivated();
		virtual void OnSystemSuspend();
		virtual void OnSystemResume();
		virtual void OnScreensaver(bool * cancel);
		virtual void OnResume();
		virtual void OnPaint();

		virtual void OnMonitorChanged();

	public:
		HWND getHandle() const { return m_hWnd; }
		int32 getMouseWheel() const { return m_mouseWheel; }
		bool getIsMinimized() const { return m_minimized; }
		bool getIsMaximized() const { return m_maximized; }
		void Close();
		//void setTopMost(bool value)
		//{
		//	SetWindowPos(m_hWnd, value ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, 3);
		//}
		Size getCurrentSize();

		void Load();

		EventHandler* eventResume() { return &m_eResume; }
		EventHandler* eventUserResized() { return &m_eUserResized; }
		EventHandler* eventSuspend() { return &m_eSuspend; }
		EventHandler* eventApplicationActivated() { return &m_eApplicationActivated; }
		EventHandler* eventApplicationDeactivated() { return &m_eApplicationDeactivated; }
		EventHandler* eventSystemSuspend() { return &m_eSystemSuspend; }
		EventHandler* eventSystemResume() { return &m_eSystemResume; }
		CancellableEventHandler* eventScreensaver() { return &m_eScreensaver; }
		EventHandler* eventPaint() { return &m_ePaint; }
		EventHandler* eventMonitorChanged() { return &m_eMonitorChanged; }

		GameWindow(HINSTANCE hInstance, int nCmdShow,
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		~GameWindow(void);
	};
};
#endif