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
#include "GraphicsDeviceManager.h"
#include "..\Game.h"
#include "..\GameWindow.h"

namespace Apoc3D
{
	namespace Graphics
	{
		GraphicsDeviceManager::GraphicsDeviceManager(Game* game)
		{
			assert(!game);

			m_game = game;

			
			m_game->eventFrameStart()->bind(this, &GraphicsDeviceManager::game_FrameStart);
			m_game->eventFrameEnd()->bind(this, &GraphicsDeviceManager::game_FrameEnd);
			m_game->getWindow()->eventUserResized()->bind(this, &GraphicsDeviceManager::Window_UserResized);
			m_game->getWindow()->eventMonitorChanged()->bind(this, &GraphicsDeviceManager::Window_MonitorChanged);
		}


		GraphicsDeviceManager::~GraphicsDeviceManager(void)
		{
		}
		bool GraphicsDeviceManager::CanResetDevice(const DeviceSettings* const oldset, 
			const DeviceSettings* const newset) const
		{
			if (!oldset)
				return false;

			return !m_device &&
                oldset->AdapterOrdinal == newset->AdapterOrdinal &&
                oldset->DeviceType == newset->DeviceType &&
                oldset->CreationFlags == newset->CreationFlags;
		}
			
		HRESULT GraphicsDeviceManager::ResetDevice()
		{
			m_game->UnloadContent();

			HRESULT hr = m_device->Reset(&m_currentSetting->PresentParams);

			if (hr == D3DERR_DEVICELOST)
				return hr;

			m_game->LoadContent();
			return hr;
		}
		void GraphicsDeviceManager::ReleaseDevice()
		{
			if (!m_device)
				return;

			if (m_game)
			{
				m_game->UnloadContent();
			}

			m_device->Release();
			m_device = 0;
		}

		void GraphicsDeviceManager::UpdateDeviceInformation()
		{

		}
		
		int32 GraphicsDeviceManager::GetAdapterOrdinal(HMONITOR mon)
		{
			UINT count = m_direct3D9->GetAdapterCount();
			for (UINT i = 0;i<count;i++)
			{
				if (mon == m_direct3D9->GetAdapterMonitor(i))
				{
					return static_cast<int32>(i);
				}
			}
			return -1;
		}

		void GraphicsDeviceManager::game_FrameStart(bool* cancel)
		{
			if (!m_game->getIsActive() || m_deviceLost)
                Sleep(50);

			if (m_deviceLost)
			{
				HRESULT hr = m_device->TestCooperativeLevel();
				if (hr = D3DERR_DEVICELOST)
				{
					*cancel = true;
					return;
				}


				if (m_currentSetting && m_currentSetting->getWindowed())
				{
					D3DDISPLAYMODE mode;
					m_direct3D9->GetAdapterDisplayMode(m_currentSetting->AdapterOrdinal, &mode);
					if (m_currentSetting->getBackBufferFormat() != mode.Format)
					{
						DeviceSettings newSettings = *m_currentSetting;
						ChangeDevice(newSettings);
                        *cancel = true;
                        return;
					}
				}

				hr = ResetDevice();
				if (hr != D3D_OK)
				{
					*cancel = true;
					return;
				}
				m_deviceLost = false;
			}
		}
		void GraphicsDeviceManager::game_FrameEnd()
		{
			HRESULT hr = m_device->Present(NULL, NULL, NULL, NULL);
			if (hr = D3DERR_DEVICELOST)
			{
				m_deviceLost = true;
			}
		}
		void GraphicsDeviceManager::Window_UserResized()
		{
			if (m_ignoreSizeChanges || !EnsureDevice() ||
               (m_currentSetting->getWindowed()))
                return;

			DeviceSettings newSettings = *m_currentSetting;

			RECT rect;
			GetClientRect(m_game->getWindow()->getHandle(), &rect);

			int32 width = rect.right - rect.left;
			int32 height = rect.bottom - rect.top;

			if (width != newSettings.getBackBufferWidth() || height != newSettings.getBackBufferHeight())
            {
                newSettings.setBackBufferWidth(0);
                newSettings.setBackBufferHeight(0);
                CreateDevice(newSettings);
            }
		}
		void GraphicsDeviceManager::Window_MonitorChanged()
		{
			if (!EnsureDevice() || !m_currentSetting->getWindowed() || m_ignoreSizeChanges)
                return;

            HMONITOR windowMonitor = MonitorFromWindow(m_game->getWindow()->getHandle(), MONITOR_DEFAULTTOPRIMARY);

            DeviceSettings newSettings = *m_currentSetting;
            int adapterOrdinal = GetAdapterOrdinal(windowMonitor);

			if (adapterOrdinal == -1)
                return;

			newSettings.AdapterOrdinal = adapterOrdinal;
			
			CreateDevice(newSettings);
		}

		void GraphicsDeviceManager::InitializeDevice()
		{
			if (!m_direct3D9)
			{
				m_direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
			}

			HRESULT result = m_direct3D9->CreateDevice(m_currentSetting->AdapterOrdinal,
				m_currentSetting->DeviceType, m_game->getWindow()->getHandle(),
				m_currentSetting->CreationFlags, &m_currentSetting->PresentParams, &m_device);

			if (result == D3DERR_DEVICELOST)
			{
				m_deviceLost = true;
			}
			// UpdateDeviceStats();

            m_game->Initialize();
            m_game->LoadContent();
		}
		void GraphicsDeviceManager::CreateDevice(const DeviceSettings const &settings)
		{
			DeviceSettings* oldSettings = m_currentSetting;
			m_currentSetting = new DeviceSettings(settings);

			m_ignoreSizeChanges = true;

			bool keepCurrentWindowSize = false;
            if (settings.getBackBufferWidth() == 0 && 
				settings.getBackBufferHeight() == 0)
                keepCurrentWindowSize = true;

			GameWindow* wnd = m_game->getWindow();

            // handle the window state in Direct3D9 (it will be handled for us in DXGI)
            

            // check if we are going to windowed or fullscreen mode
            if (settings.getWindowed())
            {
                if (oldSettings && !oldSettings->getWindowed())
                    SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)m_windowedStyle);
            }
            else
            {
                if (!oldSettings || oldSettings->getWindowed())
                {
                    //m_savedTopmost = wnd->getTopMost();
                    long style = GetWindowLong(wnd->getHandle(), GWL_STYLE);
                    style &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
                    m_windowedStyle = style;

                    m_windowedPlacement = WINDOWPLACEMENT();
                    m_windowedPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
                }

                // hide the window until we are done messing with it
                ShowWindow(wnd->getHandle(), SW_HIDE);

                SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)(WS_POPUP | WS_SYSMENU));

                WINDOWPLACEMENT placement = WINDOWPLACEMENT();
                placement.length = sizeof(WINDOWPLACEMENT);
                GetWindowPlacement(wnd->getHandle(), &placement);

                // check if we are in the middle of a restore
                if ((placement.flags & WPF_RESTORETOMAXIMIZED) != 0)
                {
                    // update the flags to avoid sizing issues
                    placement.flags &= ~WPF_RESTORETOMAXIMIZED;
                    placement.showCmd = SW_RESTORE;
                    SetWindowPlacement(wnd->getHandle(), &placement);
                }
            }
            

            if (settings.getWindowed())
            {
                if (oldSettings && !oldSettings->getWindowed())
                {
                    m_fullscreenWindowWidth = oldSettings->getBackBufferWidth();
                    m_fullscreenWindowHeight = oldSettings->getBackBufferHeight();
                }
            }
            else
            {
                if (oldSettings && oldSettings->getWindowed());
                {
                    m_windowedWindowWidth = oldSettings->getBackBufferWidth();
                    m_windowedWindowHeight = oldSettings->getBackBufferHeight();
                }
            }

            // check if the device can be reset, or if we need to completely recreate it
            
			int64 result = 0;
            bool canReset = CanResetDevice(oldSettings, m_currentSetting);
            if (canReset)
                result = ResetDevice();

            if (result == D3DERR_DEVICELOST)
                m_deviceLost = true;
            else if (!canReset || result != D3D_OK)
            {
                if (oldSettings)
                    ReleaseDevice();

                InitializeDevice();
            }

            UpdateDeviceInformation();

            // check if we changed from fullscreen to windowed mode
            if (oldSettings && !oldSettings->getWindowed() && settings.getWindowed())
            {
                SetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
                //wnd->setTopMost(m_savedTopmost);
            }

            // check if we need to resize
            if (settings.getWindowed() && !keepCurrentWindowSize)
            {
                int width;
                int height;
                if (IsIconic(wnd->getHandle()))
                {
                    WINDOWPLACEMENT placement = WINDOWPLACEMENT();
                    placement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(wnd->getHandle(), &placement);

                    // check if we are being restored
                    if ((placement.flags & WPF_RESTORETOMAXIMIZED) != 0 && placement.showCmd == SW_SHOWMINIMIZED)
                    {
                        ShowWindow(wnd->getHandle(), SW_RESTORE);

                        RECT rect;
						GetClientRect(wnd->getHandle(), &rect);

						width = rect.right - rect.left;
						height = rect.bottom - rect.top;
                        ShowWindow(wnd->getHandle(), SW_MINIMIZE);
                    }
                    else
                    {
                        RECT frame = RECT();
                        AdjustWindowRect(&frame, (uint32)m_windowedStyle, false);
                        int frameWidth = frame.right - frame.left;
                        int frameHeight = frame.bottom - frame.top;

                        width = placement.rcNormalPosition.right - placement.rcNormalPosition.left - frameWidth;
                        height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top - frameHeight;
                    }
                }
                else
                {
					RECT rect;
					GetClientRect(wnd->getHandle(), &rect);

					width = rect.right - rect.left;
					height = rect.bottom - rect.top;
                }

                // check if we have a different desired size
                if (width != settings.getBackBufferWidth() ||
                    height != settings.getBackBufferHeight())
                {
                    if (IsIconic(wnd->getHandle()))
                        ShowWindow(wnd->getHandle(), SW_RESTORE);
                    if (IsZoomed(wnd->getHandle()))
                        ShowWindow(wnd->getHandle(), SW_RESTORE);

                    RECT rect = RECT();
                    rect.right = settings.getBackBufferWidth();
                    rect.bottom = settings.getBackBufferHeight();
                    AdjustWindowRect(& rect,
                        GetWindowLong(wnd->getHandle(), GWL_STYLE), false);

                    SetWindowPos(wnd->getHandle(), 0, 0, 0, rect.right - rect.left,
                        rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);

                    RECT r ;
					GetClientRect(wnd->getHandle(), &rect);

					int clientWidth = r.right - r.left;
					int clientHeight = r.bottom - r.top;

                    // check if the size was modified by Windows
                    if (clientWidth != settings.getBackBufferWidth() ||
                        clientHeight != settings.getBackBufferHeight())
                    {
                        DeviceSettings newSettings = settings;
                        newSettings.setBackBufferWidth(0);
                        newSettings.setBackBufferHeight(0);
                        
                        CreateDevice(newSettings);
                    }
                }
            }

            // if the window is still hidden, make sure it is shown
			if (!IsWindowVisible(wnd->getHandle()))
                ShowWindow(wnd->getHandle(), SW_SHOW);

            // set the execution state of the thread
			if (!m_currentSetting->getWindowed())
                SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
            else
                SetThreadExecutionState(ES_CONTINUOUS);

            m_ignoreSizeChanges = false;

			if (oldSettings)
				delete oldSettings;
		}
		void GraphicsDeviceManager::ChangeDevice(bool windowed, int desiredWidth, int desiredHeight)
		{
			DeviceSettings desiredSettings = DeviceSettings();
            desiredSettings.setWindowed(windowed);
            desiredSettings.setBackBufferWidth(desiredWidth);
            desiredSettings.setBackBufferHeight(desiredHeight);

            ChangeDevice(desiredSettings);
		}
		void GraphicsDeviceManager::ChangeDevice(const DeviceSettings const &prefer)
		{
			CreateDevice(prefer);
		}
		void GraphicsDeviceManager::ToggleFullScreen()
		{
			assert(!EnsureDevice());

			DeviceSettings newSettings = *m_currentSetting;
			newSettings.setWindowed( !newSettings.getWindowed() );

            int width = newSettings.getWindowed() ? m_windowedWindowWidth :  m_fullscreenWindowWidth;
            int height = newSettings.getWindowed() ?  m_windowedWindowHeight :  m_fullscreenWindowHeight;

            newSettings.setBackBufferWidth( width );
            newSettings.setBackBufferHeight( height );

            ChangeDevice(newSettings);
		}
	}
}