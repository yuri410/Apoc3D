/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "Game.h"
#include "GameWindow.h"
#include "Enumeration.h"

#include "apoc3d/Core/Logging.h"

#include <strstream>
#include <sstream>

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			GraphicsDeviceManager::GraphicsDeviceManager(Game* game, IDirect3D9* d3d9)
				: m_currentSetting(0),m_device(0),
				m_deviceLost(false), m_ignoreSizeChanges(false), m_doNotStoreBufferSize(false), m_renderingOccluded(false),
				m_direct3D9(d3d9), m_userIgnoreMoniorChanges(false)
			{
				assert(game);

				m_game = game;

				m_game->eventFrameStart()->Bind(this, &GraphicsDeviceManager::game_FrameStart);
				m_game->eventFrameEnd()->Bind(this, &GraphicsDeviceManager::game_FrameEnd);
				m_game->getWindow()->eventUserResized()->Bind(this, &GraphicsDeviceManager::Window_UserResized);
				m_game->getWindow()->eventMonitorChanged()->Bind(this, &GraphicsDeviceManager::Window_MonitorChanged);
			}

			GraphicsDeviceManager::~GraphicsDeviceManager(void)
			{
				m_game->eventFrameStart()->Unbind(this, &GraphicsDeviceManager::game_FrameStart);
				m_game->eventFrameEnd()->Unbind(this, &GraphicsDeviceManager::game_FrameEnd);
				m_game->getWindow()->eventUserResized()->Unbind(this, &GraphicsDeviceManager::Window_UserResized);
				m_game->getWindow()->eventMonitorChanged()->Unbind(this, &GraphicsDeviceManager::Window_MonitorChanged);

				if (m_currentSetting)
					delete m_currentSetting;
			}

			void GraphicsDeviceManager::PropogateSettings()
			{
				m_currentSetting->BackBufferCount = m_currentSetting->D3D9.PresentParameters.BackBufferCount;
				m_currentSetting->BackBufferWidth = m_currentSetting->D3D9.PresentParameters.BackBufferWidth;
				m_currentSetting->BackBufferHeight = m_currentSetting->D3D9.PresentParameters.BackBufferHeight;
				m_currentSetting->BackBufferFormat = m_currentSetting->D3D9.PresentParameters.BackBufferFormat;
				m_currentSetting->DepthStencilFormat = m_currentSetting->D3D9.PresentParameters.AutoDepthStencilFormat;
				m_currentSetting->DeviceType = m_currentSetting->D3D9.DeviceType;
				m_currentSetting->MultiSampleQuality = m_currentSetting->D3D9.PresentParameters.MultiSampleQuality;
				m_currentSetting->MultiSampleType = m_currentSetting->D3D9.PresentParameters.MultiSampleType;
				m_currentSetting->RefreshRate = m_currentSetting->D3D9.PresentParameters.FullScreen_RefreshRateInHz;
				m_currentSetting->Windowed = m_currentSetting->D3D9.PresentParameters.Windowed ? true : false;

			}
			bool GraphicsDeviceManager::CanDeviceBeReset(const DeviceSettings* const oldset,
				const DeviceSettings* const newset) const
			{
				if (!oldset)
					return false;

				return m_device &&
					oldset->D3D9.AdapterOrdinal == newset->D3D9.AdapterOrdinal &&
					oldset->D3D9.DeviceType == newset->D3D9.DeviceType &&
					oldset->D3D9.CreationFlags == newset->D3D9.CreationFlags;
			}

			HRESULT GraphicsDeviceManager::ResetDevice()
			{
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[D3D9]Device lost. ", 
					LOGLVL_Default);

				m_game->OnDeviceLost();

				HRESULT hr = m_device->Reset(&m_currentSetting->D3D9.PresentParameters);
				
				PropogateSettings();

				m_game->OnDeviceReset();
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
				const AdapterInfo* adapter = 0;
				const FastList<AdapterInfo*> adInfo = Enumeration::getAdapters();
				for (int32 i=0;i<adInfo.getCount();i++)
				{
					if (m_direct3D9->GetAdapterMonitor(adInfo[i]->AdapterOrdinal) == mon)
					{
						adapter = adInfo[i];
						break;
					}
				}

				if (adapter)
				{
					return adapter->AdapterOrdinal;
				}

				return -1;
			}

			void GraphicsDeviceManager::game_FrameStart(bool* cancel)
			{
				if (!m_device)
					return;

//#ifdef _DEBUG
				//if (m_deviceLost)
					//Sleep(50);
//#else
				if (m_deviceLost || !m_game->getIsActive())
					Sleep(50);
//#endif

				if (m_deviceLost)
				{
					HRESULT hr = m_device->TestCooperativeLevel();
					if (hr == D3DERR_DEVICELOST)
					{
						*cancel = true;
						return;
					}

					//if (m_currentSetting->Windowed)
					//{
					//	D3DDISPLAYMODE mode;
					//	HRESULT hr = m_direct3D9->GetAdapterDisplayMode(m_currentSetting->D3D9.AdapterOrdinal, &mode);
					//	assert(SUCCEEDED(hr));

					//	if (m_currentSetting->D3D9.AdapterFormat != mode.Format)
					//	{
					//		DeviceSettings newSettings = *m_currentSetting;
					//		ChangeDevice(newSettings);
					//		*cancel = true;
					//		return;
					//	}
					//}

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
				if (hr == D3DERR_DEVICELOST)
				{
					m_deviceLost = true;
				}
			}
			void GraphicsDeviceManager::Window_UserResized()
			{
				// TBD
				if (m_ignoreSizeChanges || !EnsureDevice()) // || (m_currentSetting->Windowed)
					return;

				DeviceSettings newSettings = *m_currentSetting;

				RECT rect;
				GetClientRect(m_game->getWindow()->getHandle(), &rect);

				int32 width = rect.right - rect.left;
				int32 height = rect.bottom - rect.top;

				if (width != newSettings.BackBufferWidth || height != newSettings.BackBufferHeight)
				{
					newSettings.BackBufferWidth = 0;
					newSettings.BackBufferHeight = 0;
					newSettings.D3D9.PresentParameters.BackBufferWidth = 0;
					newSettings.D3D9.PresentParameters.BackBufferHeight = 0;
					CreateDevice(newSettings);
				}
			}
			void GraphicsDeviceManager::Window_MonitorChanged()
			{
				if (!EnsureDevice() || !m_currentSetting->Windowed || m_ignoreSizeChanges)
					return;

				if (m_userIgnoreMoniorChanges)
					return;

				HMONITOR windowMonitor = MonitorFromWindow(m_game->getWindow()->getHandle(), MONITOR_DEFAULTTOPRIMARY);

				DeviceSettings newSettings = *m_currentSetting;
				int adapterOrdinal = GetAdapterOrdinal(windowMonitor);

				if (adapterOrdinal == -1)
					return;

				newSettings.D3D9.AdapterOrdinal = adapterOrdinal;

				CreateDevice(newSettings);
			}

			void GraphicsDeviceManager::InitializeDevice()
			{
				HWND sss = m_game->getWindow()->getHandle();
				HRESULT result = m_direct3D9->CreateDevice(m_currentSetting->D3D9.AdapterOrdinal,
					m_currentSetting->D3D9.DeviceType, sss ,
					m_currentSetting->D3D9.CreationFlags, &m_currentSetting->D3D9.PresentParameters, &m_device);

				if (result == D3DERR_DEVICELOST)
				{
					m_deviceLost = true;
					return;
				}

				// UpdateDeviceStats();
				PropogateSettings();

				{
					std::wstringstream wss;

					if (m_currentSetting->D3D9.DeviceType == D3DDEVTYPE_HAL)
						wss << L"HAL";
					else if (m_currentSetting->D3D9.DeviceType == D3DDEVTYPE_REF)
						wss << L"REF";
					else if (m_currentSetting->D3D9.DeviceType == D3DDEVTYPE_SW)
						wss << L"SW";

					if (m_currentSetting->D3D9.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
					{
						if (m_currentSetting->D3D9.DeviceType == D3DDEVTYPE_HAL)
							wss << L" (hw vp)";
						else
							wss << L" (simulated hw vp)";
					}
					else if (m_currentSetting->D3D9.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
					{
						if (m_currentSetting->D3D9.DeviceType == D3DDEVTYPE_HAL)
							wss << L" (mixed vp)";
						else
							wss << L" (simulated mixed vp)";
					}
					else
					{
						wss << L" (sw vp)";
					}

					const FastList<AdapterInfo*> adapters = Enumeration::getAdapters();
					for (int32 i=0;i<adapters.getCount();i++)
					{
						if (adapters[i]->AdapterOrdinal
							== (int32)m_currentSetting->D3D9.AdapterOrdinal)
						{
							wss << L": ";
							wss << adapters[i]->Description;
							break;
						}
					}

					LogManager::getSingleton().Write(LOG_Graphics, 
						L"[D3D9]Created device: " + wss.str(), 
						LOGLVL_Infomation);
				}

 				m_game->Initialize();
				m_game->LoadContent();
			}
			void GraphicsDeviceManager::CreateDevice(const DeviceSettings& settings)
			{
				DeviceSettings* oldSettings = m_currentSetting;
				m_currentSetting = new DeviceSettings(settings);

				m_ignoreSizeChanges = true;

				bool keepCurrentWindowSize = false;
				if (settings.BackBufferWidth == 0 &&
					settings.BackBufferHeight == 0)
					keepCurrentWindowSize = true;

				GameWindow* wnd = m_game->getWindow();

				// handle the window state in Direct3D9 (it will be handled for us in DXGI)


				// check if we are going to windowed or fullscreen mode
				if (settings.Windowed)
				{
					if (oldSettings && !oldSettings->Windowed)
						SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)m_windowedStyle);
				}
				else
				{
					if (!oldSettings || oldSettings->Windowed)
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


				if (settings.Windowed)
				{
					if (oldSettings && !oldSettings->Windowed)
					{
						m_fullscreenWindowWidth = oldSettings->BackBufferWidth;
						m_fullscreenWindowHeight = oldSettings->BackBufferHeight;
					}
				}
				else
				{
					if (oldSettings && oldSettings->Windowed)
					{
						m_windowedWindowWidth = oldSettings->BackBufferWidth;
						m_windowedWindowHeight = oldSettings->BackBufferHeight;
					}
				}

				// check if the device can be reset, or if we need to completely recreate it

				int64 result = 0;
				bool canReset = CanDeviceBeReset(oldSettings, m_currentSetting);
				if (canReset)
					result = ResetDevice();

				if (result == D3DERR_DEVICELOST)
					m_deviceLost = true;
				else if (!canReset || result != D3D_OK)
				{
					if (oldSettings)
					{
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"[D3D9]Recreating Device. ", 
							LOGLVL_Default);
						ReleaseDevice();
					}
					InitializeDevice();
				}

				UpdateDeviceInformation();

				// check if we changed from fullscreen to windowed mode
				if (oldSettings && !oldSettings->Windowed && settings.Windowed)
				{
					SetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
					//wnd->setTopMost(m_savedTopmost);
				}

				// check if we need to resize
				if (settings.Windowed && !keepCurrentWindowSize)
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
							RECT frame;
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
					if (width != settings.BackBufferWidth ||
						height != settings.BackBufferHeight)
					{
						if (IsIconic(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);
						if (IsZoomed(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);

						RECT rect = RECT();
						rect.right = settings.BackBufferWidth;
						rect.bottom = settings.BackBufferHeight;
						AdjustWindowRect(& rect,
							GetWindowLong(wnd->getHandle(), GWL_STYLE), false);



						int clientWidth = rect.right - rect.left;
						int clientHeight = rect.bottom - rect.top;

						int scrnWidth = GetSystemMetrics(SM_CXFULLSCREEN);   
						int scrnHeight = GetSystemMetrics(SM_CYFULLSCREEN);



						SetWindowPos(wnd->getHandle(), 0, 
							(scrnWidth - clientWidth)>>1, (scrnHeight - clientHeight)>>1, 
							clientWidth, clientHeight, SWP_NOZORDER);

						RECT r;
						GetClientRect(wnd->getHandle(), &r);

						clientWidth = r.right - r.left;
						clientHeight = r.bottom - r.top;

						// check if the size was modified by Windows
						if (clientWidth != settings.BackBufferWidth ||
							clientHeight != settings.BackBufferHeight)
						{
							DeviceSettings newSettings = settings;
							newSettings.BackBufferWidth = 0;
							newSettings.BackBufferHeight = 0;

							CreateDevice(newSettings);
						}
					}
				}

				// if the window is still hidden, make sure it is shown
				if (!IsWindowVisible(wnd->getHandle()))
					ShowWindow(wnd->getHandle(), SW_SHOW);

				// set the execution state of the thread
				if (!m_currentSetting->Windowed)
					SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
				else
					SetThreadExecutionState(ES_CONTINUOUS);

				m_ignoreSizeChanges = false;

				if (oldSettings)
					delete oldSettings;
			}
			void GraphicsDeviceManager::ChangeDevice(const DeviceSettings& settings, const DeviceSettings* minimumSettings)
			{
				if (minimumSettings)
				{
					Enumeration::setMinimumSettings(*minimumSettings);
				}
				else
				{
					Enumeration::ClearMinimumSetting();
				}
				DeviceSettings validSettings;
				Enumeration::FindValidSettings(m_direct3D9, settings, validSettings);
				validSettings.D3D9.PresentParameters.hDeviceWindow = m_game->getWindow()->getHandle();
				CreateDevice(validSettings);

				
				
			}
			void GraphicsDeviceManager::ChangeDevice(bool windowed, int desiredWidth, int desiredHeight)
			{
				DeviceSettings desiredSettings = DeviceSettings();
				desiredSettings.Windowed = (windowed);
				desiredSettings.BackBufferWidth = (desiredWidth);
				desiredSettings.BackBufferHeight = (desiredHeight);

				ChangeDevice(desiredSettings, 0);
			}
			void GraphicsDeviceManager::ChangeDevice(const DeviceSettings &prefer)
			{
				ChangeDevice(prefer, 0);
			}
			void GraphicsDeviceManager::ToggleFullScreen()
			{
				assert(EnsureDevice());

				DeviceSettings newSettings = *m_currentSetting;
				newSettings.Windowed =  !newSettings.Windowed;

				int width = newSettings.Windowed ? m_windowedWindowWidth :  m_fullscreenWindowWidth;
				int height = newSettings.Windowed ?  m_windowedWindowHeight :  m_fullscreenWindowHeight;

				newSettings.BackBufferWidth =  width;
				newSettings.BackBufferHeight =  height;

				ChangeDevice(newSettings);
			}
		}
	}
}