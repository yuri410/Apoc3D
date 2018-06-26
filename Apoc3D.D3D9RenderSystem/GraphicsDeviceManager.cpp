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
#include "D3D9RenderWindow.h"
#include "Enumeration.h"

#include "apoc3d.Win32/GameWindow.h"
#include "apoc3d/Core/Logging.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			GraphicsDeviceManager::GraphicsDeviceManager(D3D9RenderWindow* game, IDirect3D9* d3d9)
				: m_direct3D9(d3d9), m_game(game)
			{
				assert(game);

				m_game->eventFrameStart.Bind(this, &GraphicsDeviceManager::game_FrameStart);
				m_game->eventFrameEnd.Bind(this, &GraphicsDeviceManager::game_FrameEnd);
				m_game->getWindow()->eventUserResized.Bind(this, &GraphicsDeviceManager::Window_UserResized);
				m_game->getWindow()->eventMonitorChanged.Bind(this, &GraphicsDeviceManager::Window_MonitorChanged);

				m_windowedStyle = GetWindowLong(m_game->getWindow()->getHandle(), GWL_STYLE);
			}

			GraphicsDeviceManager::~GraphicsDeviceManager()
			{
				m_game->eventFrameStart.Unbind(this, &GraphicsDeviceManager::game_FrameStart);
				m_game->eventFrameEnd.Unbind(this, &GraphicsDeviceManager::game_FrameEnd);
				m_game->getWindow()->eventUserResized.Unbind(this, &GraphicsDeviceManager::Window_UserResized);
				m_game->getWindow()->eventMonitorChanged.Unbind(this, &GraphicsDeviceManager::Window_MonitorChanged);

				DELETE_AND_NULL(m_currentSetting);
			}

			void GraphicsDeviceManager::PropogateSettings()
			{
				/*m_currentSetting->BackBufferCount = m_currentSetting->D3D9.PresentParameters.BackBufferCount;
				m_currentSetting->BackBufferWidth = m_currentSetting->D3D9.PresentParameters.BackBufferWidth;
				m_currentSetting->BackBufferHeight = m_currentSetting->D3D9.PresentParameters.BackBufferHeight;
				m_currentSetting->BackBufferFormat = m_currentSetting->D3D9.PresentParameters.BackBufferFormat;
				m_currentSetting->DepthStencilFormat = m_currentSetting->D3D9.PresentParameters.AutoDepthStencilFormat;
				m_currentSetting->DeviceType = m_currentSetting->D3D9.DeviceType;
				m_currentSetting->MultiSampleQuality = m_currentSetting->D3D9.PresentParameters.MultiSampleQuality;
				m_currentSetting->MultiSampleType = m_currentSetting->D3D9.PresentParameters.MultiSampleType;
				m_currentSetting->RefreshRate = m_currentSetting->D3D9.PresentParameters.FullScreen_RefreshRateInHz;
				m_currentSetting->Windowed = m_currentSetting->D3D9.PresentParameters.Windowed ? true : false;
				*/
			}
			bool GraphicsDeviceManager::CanDeviceBeReset(const RawSettings* oldset, const RawSettings* newset) const
			{
				if (!oldset)
					return false;

				return m_device &&
					oldset->AdapterOrdinal == newset->AdapterOrdinal &&
					oldset->DeviceType == newset->DeviceType &&
					oldset->CreationFlags == newset->CreationFlags;
			}

			HRESULT GraphicsDeviceManager::ResetDevice()
			{
				if (!m_handingDeviceReset)
				{
					LogManager::getSingleton().Write(LOG_Graphics, L"[D3D9]Device lost. ", LOGLVL_Default);

					m_game->D3D9_OnDeviceLost();
					m_handingDeviceReset = true;
				}

				HRESULT hr = m_device->Reset(&m_currentSetting->PresentParameters);
				
				if (hr == D3DERR_DEVICELOST)
				{
					return hr;
				}

				LogManager::getSingleton().Write(LOG_Graphics, L"[D3D9]Device reset. ", LOGLVL_Default);

				PropogateSettings();

				m_game->D3D9_OnDeviceReset();
				m_handingDeviceReset = false;

				return hr;
			}

			void GraphicsDeviceManager::UpdateDeviceInformation()
			{

			}

			int32 GraphicsDeviceManager::GetAdapterOrdinal(HMONITOR mon)
			{
				const AdapterInfo* adapter = 0;
				const List<AdapterInfo*> adInfo = Enumeration::getAdapters();
				for (int32 i = 0; i < adInfo.getCount(); i++)
				{
					if (m_direct3D9->GetAdapterMonitor(adInfo[i]->AdapterIndex) == mon)
					{
						adapter = adInfo[i];
						break;
					}
				}

				if (adapter)
				{
					return adapter->AdapterIndex;
				}

				return -1;
			}

			void GraphicsDeviceManager::game_FrameStart(bool* cancel)
			{
				if (!m_device)
					return;

				if (m_deviceLost)
					Sleep(50);

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
				if (m_ignoreSizeChanges || !IsDeviceReady()) // || (m_currentSetting->Windowed)
					return;

				RawSettings newSettings = *m_currentSetting;

				RECT rect;
				GetClientRect(m_game->getWindow()->getHandle(), &rect);

				int32 width = rect.right - rect.left;
				int32 height = rect.bottom - rect.top;

				if (width != newSettings.PresentParameters.BackBufferWidth || height != newSettings.PresentParameters.BackBufferHeight)
				{
					//newSettings.BackBufferWidth = 0;
					//newSettings.BackBufferHeight = 0;
					newSettings.PresentParameters.BackBufferWidth = 0;
					newSettings.PresentParameters.BackBufferHeight = 0;
					CreateDevice(newSettings);
				}
			}
			void GraphicsDeviceManager::Window_MonitorChanged()
			{
				if (!IsDeviceReady() || !m_currentSetting->PresentParameters.Windowed || m_ignoreSizeChanges)
					return;

				if (m_userIgnoreMoniorChanges)
					return;

				HMONITOR windowMonitor = MonitorFromWindow(m_game->getWindow()->getHandle(), MONITOR_DEFAULTTOPRIMARY);

				RawSettings newSettings = *m_currentSetting;
				int adapterOrdinal = GetAdapterOrdinal(windowMonitor);

				if (adapterOrdinal == -1)
					return;

				newSettings.AdapterOrdinal = adapterOrdinal;

				CreateDevice(newSettings);
			}

			void GraphicsDeviceManager::InitializeDevice(bool isDeviceReset)
			{
				HWND sss = m_game->getWindow()->getHandle();
				HRESULT result = m_direct3D9->CreateDevice(m_currentSetting->AdapterOrdinal,
					m_currentSetting->DeviceType, sss ,
					m_currentSetting->CreationFlags, &m_currentSetting->PresentParameters, &m_device);

				if (result == D3DERR_DEVICELOST)
				{
					m_deviceLost = true;
					return;
				}

				// UpdateDeviceStats();
				PropogateSettings();

				{
					String descTxt;

					if (m_currentSetting->DeviceType == D3DDEVTYPE_HAL)
						descTxt.append( L"HAL");
					else if (m_currentSetting->DeviceType == D3DDEVTYPE_REF)
						descTxt.append( L"REF");
					else if (m_currentSetting->DeviceType == D3DDEVTYPE_SW)
						descTxt.append( L"SW");

					if (m_currentSetting->CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
					{
						if (m_currentSetting->DeviceType == D3DDEVTYPE_HAL)
							descTxt.append( L" (hw vp)");
						else
							descTxt.append( L" (simulated hw vp)");
					}
					else if (m_currentSetting->CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
					{
						if (m_currentSetting->DeviceType == D3DDEVTYPE_HAL)
							descTxt.append(L" (mixed vp)");
						else
							descTxt.append(L" (simulated mixed vp)");
					}
					else
					{
						descTxt.append( L" (sw vp)");
					}

					const List<AdapterInfo*> adapters = Enumeration::getAdapters();
					for (const AdapterInfo* adp : adapters)
					{
						if (adp->AdapterIndex == (int32)m_currentSetting->AdapterOrdinal)
						{
							descTxt.append(L": ");
							descTxt += adp->GraphicsCardName;
							break;
						}
					}

					LogManager::getSingleton().Write(LOG_Graphics, 
						L"[D3D9]Created device: " + descTxt, 
						LOGLVL_Infomation);
				}

				m_game->D3D9_Initialize(isDeviceReset);
				m_game->D3D9_LoadContent();
			}

			void GraphicsDeviceManager::ReleaseDevice(bool isDeviceReset)
			{
				if (m_device == nullptr)
					return;

				if (m_game)
				{
					m_game->D3D9_UnloadContent();
					m_game->D3D9_Finalize(isDeviceReset);
				}

				m_device->Release();
				m_device = 0;
			}

			void GraphicsDeviceManager::CreateDevice(const RawSettings& settings)
			{
				RawSettings* oldSettings = m_currentSetting;
				m_currentSetting = new RawSettings(settings);

				m_ignoreSizeChanges = true;

				bool keepCurrentWindowSize = false;
				if (settings.PresentParameters.BackBufferWidth == 0 &&
					settings.PresentParameters.BackBufferHeight == 0)
					keepCurrentWindowSize = true;

				GameWindow* wnd = m_game->getWindow();

				// handle the window state in Direct3D9 (it will be handled for us in DXGI)


				// check if we are going to windowed or fullscreen mode
				if (settings.PresentParameters.Windowed)
				{
					if (oldSettings && !oldSettings->PresentParameters.Windowed)
						SetWindowLong(wnd->getHandle(), GWL_STYLE, (uint32)m_windowedStyle);
				}
				else
				{
					if (!oldSettings || oldSettings->PresentParameters.Windowed)
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


				if (settings.PresentParameters.Windowed)
				{
					if (oldSettings && !oldSettings->PresentParameters.Windowed)
					{
						m_fullscreenWindowWidth = oldSettings->PresentParameters.BackBufferWidth;
						m_fullscreenWindowHeight = oldSettings->PresentParameters.BackBufferHeight;
					}
				}
				else
				{
					if (oldSettings && oldSettings->PresentParameters.Windowed)
					{
						m_windowedWindowWidth = oldSettings->PresentParameters.BackBufferWidth;
						m_windowedWindowHeight = oldSettings->PresentParameters.BackBufferHeight;
					}
				}

				// check if the device can be reset, or if we need to completely recreate it

				HRESULT result = 0;
				bool canReset = CanDeviceBeReset(oldSettings, m_currentSetting);
				if (canReset)
					result = ResetDevice();

				if (result == D3DERR_DEVICELOST)
					m_deviceLost = true;
				else if (!canReset || result != D3D_OK)
				{
					if (oldSettings)
					{
						ApocLog(LOG_Graphics, L"[D3D9]Recreating Device. ", LOGLVL_Default);

						ReleaseDevice(true);
						InitializeDevice(true);
					}
					else
						InitializeDevice(false);
				}

				UpdateDeviceInformation();

				// check if we changed from fullscreen to windowed mode
				if (oldSettings && !oldSettings->PresentParameters.Windowed && settings.PresentParameters.Windowed)
				{
					SetWindowPlacement(wnd->getHandle(), &m_windowedPlacement);
					//wnd->setTopMost(m_savedTopmost);
				}

				// check if we need to resize
				if (settings.PresentParameters.Windowed && !keepCurrentWindowSize)
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
					if (width != settings.PresentParameters.BackBufferWidth ||
						height != settings.PresentParameters.BackBufferHeight)
					{
						if (IsIconic(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);
						if (IsZoomed(wnd->getHandle()))
							ShowWindow(wnd->getHandle(), SW_RESTORE);

						RECT rect = RECT();
						rect.right = settings.PresentParameters.BackBufferWidth;
						rect.bottom = settings.PresentParameters.BackBufferHeight;
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
						if (clientWidth != settings.PresentParameters.BackBufferWidth ||
							clientHeight != settings.PresentParameters.BackBufferHeight)
						{
							RawSettings newSettings = settings;
							newSettings.PresentParameters.BackBufferWidth = 0;
							newSettings.PresentParameters.BackBufferHeight = 0;

							CreateDevice(newSettings);
						}
					}
				}

				// if the window is still hidden, make sure it is shown
				if (!IsWindowVisible(wnd->getHandle()))
					ShowWindow(wnd->getHandle(), SW_SHOW);

				// set the execution state of the thread
				if (!m_currentSetting->PresentParameters.Windowed)
					SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
				else
					SetThreadExecutionState(ES_CONTINUOUS);

				m_ignoreSizeChanges = false;

				if (oldSettings)
					delete oldSettings;
			}
			void GraphicsDeviceManager::ChangeDevice(const RenderParameters& settings, const RenderParameters* minimumSettings)
			{
				if (minimumSettings)
				{
					Enumeration::MinimumSettings.Set(*minimumSettings);
				}
				else
				{
					Enumeration::MinimumSettings.IsSet = false;
				}
				RawSettings validSettings;
				Enumeration::FindValidSettings(m_direct3D9, settings, validSettings);
				validSettings.PresentParameters.hDeviceWindow = m_game->getWindow()->getHandle();
				CreateDevice(validSettings);
			}
			void GraphicsDeviceManager::ChangeDevice(bool windowed, int desiredWidth, int desiredHeight)
			{
				RenderParameters desiredSettings;
				desiredSettings.IsWindowed = windowed;
				desiredSettings.BackBufferWidth = desiredWidth;
				desiredSettings.BackBufferHeight = desiredHeight;

				ChangeDevice(desiredSettings, 0);
			}
			void GraphicsDeviceManager::ChangeDevice(const RenderParameters &prefer)
			{
				ChangeDevice(prefer, 0);
			}
			void GraphicsDeviceManager::ChangeDevice(const RawSettings& settings)
			{
				RawSettings validSettings;
				Enumeration::FindValidSettings(m_direct3D9, settings, validSettings);
				validSettings.PresentParameters.hDeviceWindow = m_game->getWindow()->getHandle();
				CreateDevice(validSettings);
			}

			void GraphicsDeviceManager::ToggleFullScreen()
			{
				assert(IsDeviceReady());

				RawSettings newSettings = *m_currentSetting;
				newSettings.PresentParameters.Windowed = !newSettings.PresentParameters.Windowed;

				int width = newSettings.PresentParameters.Windowed ? m_windowedWindowWidth : m_fullscreenWindowWidth;
				int height = newSettings.PresentParameters.Windowed ? m_windowedWindowHeight : m_fullscreenWindowHeight;

				newSettings.PresentParameters.BackBufferWidth = width;
				newSettings.PresentParameters.BackBufferHeight = height;

				ChangeDevice(newSettings);
			}
		}
	}
}