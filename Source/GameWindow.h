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
	
	class _Export GameWindow
	{
	private:
		bool m_minimized;
		bool m_maximized;
		bool m_inSizeMove;
		
		Size m_cachedSize;
		HINSTANCE m_hInst;

		HWND m_hWnd;
		
		HMONITOR m_currentMonitor;

		DelegateEvent<EventHandler> m_ePaint;
		DelegateEvent<EventHandler> m_eResume;
		DelegateEvent<EventHandler> m_eUserResized;
		DelegateEvent<EventHandler> m_eSuspend;
		DelegateEvent<EventHandler> m_eApplicationActivated;
		DelegateEvent<EventHandler> m_eApplicationDeactivated;
		DelegateEvent<EventHandler> m_eSystemSuspend;
		DelegateEvent<EventHandler> m_eSystemResume;
		DelegateEvent<EventHandler> m_eMonitorChanged;
		DelegateEvent<CancellableEventHandler> m_eScreensaver;


		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow,
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		ATOM MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass);


		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
		bool getIsMinimized() const { return m_minimized; }
		bool getIsMaximized() const { return m_maximized; }
		//void setTopMost(bool value)
		//{
		//	SetWindowPos(m_hWnd, value ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, 3);
		//}
		Size getCurrentSize();

		const DelegateEvent<EventHandler> &eventResume() const { return m_eResume; }
		const DelegateEvent<EventHandler> &eventUserResized() const { return m_eUserResized; }
		const DelegateEvent<EventHandler> &eventSuspend() const { return m_eSuspend; }
		const DelegateEvent<EventHandler> &eventApplicationActivated() const { return m_eApplicationActivated; }
		const DelegateEvent<EventHandler> &eventApplicationDeactivated() const { return m_eApplicationDeactivated; }
		const DelegateEvent<EventHandler> &eventSystemSuspend() const { return m_eSystemSuspend; }
		const DelegateEvent<EventHandler> &eventSystemResume() const { return m_eSystemResume; }
		const DelegateEvent<CancellableEventHandler> &eventScreensaver() const { return m_eScreensaver; }
		const DelegateEvent<EventHandler> &eventPaint() const { return m_ePaint; }
		const DelegateEvent<EventHandler> &eventMonitorChanged() const { return m_eMonitorChanged; }

		GameWindow(HINSTANCE hInstance, int nCmdShow, 
			const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		~GameWindow(void);
	};
};
#endif