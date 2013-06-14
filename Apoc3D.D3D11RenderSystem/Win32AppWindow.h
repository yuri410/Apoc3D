#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"

namespace rex
{
	/* 
	 * Implements a specialized window for games and rendering.
	 */
	class Win32AppWindow
	{
	public:
		Win32AppWindow(const String& name, const String& wndTitle);
		~Win32AppWindow(void);

		HWND getHandle() const { return m_hWnd; }
		int32 getMouseWheel() const { return m_mouseWheel; }
		bool getIsMinimized() const { return m_minimized; }
		bool getIsMaximized() const { return m_maximized; }
		void Close();
		void MakeFixedSize(bool v);
		//void setTopMost(bool value)
		//{
		//	SetWindowPos(m_hWnd, value ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, 3);
		//}
		String getWindowTitle() const { return m_title; }
		void setWindowTitle(const String& txt);
		Size getCurrentSize() const;

		void Load(int32 width, int32 height, bool fixed);

		EventHandler eventResume;
		EventHandler eventUserResized;
		EventHandler eventSuspend;
		EventHandler eventApplicationActivated;
		EventHandler eventApplicationDeactivated;
		EventHandler eventSystemSuspend;
		EventHandler eventSystemResume;
		CancellableEventHandler eventScreensaver;
		EventHandler eventPaint;
		EventHandler eventMonitorChanged;

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

	private:
		bool m_minimized;
		bool m_maximized;
		bool m_inSizeMove;
		String m_title;
		String m_className;
		Size m_cachedSize;
		HINSTANCE m_hInst;

		HWND m_hWnd;
		HINSTANCE m_hInstance;

		HMONITOR m_currentMonitor;


		int32 m_mouseWheel;

		static Win32AppWindow* ms_Window;

		BOOL InitInstance(HINSTANCE hInstance, int32 width, int32 height, bool fixed, const TCHAR* const &wndClass, const TCHAR* const &wndTitle);
		ATOM MyRegisterClass(HINSTANCE hInstance, const TCHAR* const &wndClass);


		static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void UpdateMonitor();
	};
}
