#pragma once
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#ifndef APOC3D_WIN32_GAMEWINDOW_H
#define APOC3D_WIN32_GAMEWINDOW_H

#include "Win32Common.h"

#include "apoc3d/Math/Point.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Win32
	{
		/* Implements a specialized window for games and rendering. */
		class Win32Window
		{
		public:
			Win32Window(const String& name, const String& wndTitle);
			~Win32Window();

			/** Creates a win32 window using the given parameters. */
			void Load(int32 width, int32 height, bool fixed);

			void Close();

			/** Enable/disable the maximum button based on the given value */
			void MakeFixedSize(bool v);

			String getWindowTitle() const { return m_title; }
			void setWindowTitle(const String& txt);
			/** Gets the current client size */
			Size getCurrentSize() const;

			HWND getHandle() const { return m_hWnd; }

			/** Gets the mouse wheel amount from window message */
			int32 getMouseWheel() const { return m_mouseWheel; }
			bool getIsMinimized() const { return m_minimized; }
			bool getIsMaximized() const { return m_maximized; }

			EventHandler eventPaint;
			EventHandler eventResume;
			EventHandler eventUserResized;
			EventHandler eventSuspend;
			EventHandler eventApplicationActivated;
			EventHandler eventApplicationDeactivated;
			EventHandler eventSystemSuspend;
			EventHandler eventSystemResume;
			EventHandler eventMonitorChanged;
			CancellableEventHandler eventScreensaver;

		private:
			void OnUserResized();
			void OnSuspend();
			void OnApplicationActivated();
			void OnApplicationDeactivated();
			void OnSystemSuspend();
			void OnSystemResume();
			void OnScreensaver(bool * cancel);
			void OnResume();
			void OnPaint();

			void OnMonitorChanged();

			static Win32Window* ms_Window;

			BOOL InitInstance(HINSTANCE hInstance, int32 width, int32 height, bool fixed, const String& wndClass, const String& wndTitle);

			static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			void UpdateMonitor();

			bool m_minimized = false;
			bool m_maximized = false;
			bool m_inSizeMove = false;
			String m_title;
			String m_className;
			Size m_cachedSize;
			HINSTANCE m_hInst;

			HWND m_hWnd;
			HINSTANCE m_hInstance;

			HMONITOR m_currentMonitor;


			int32 m_mouseWheel = 0;

		};
	}

};
#endif