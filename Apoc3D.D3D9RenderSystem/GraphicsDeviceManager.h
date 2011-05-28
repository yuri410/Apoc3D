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
#ifndef GRAPHICSDEVICEMANAGER_H
#define GRAPHICSDEVICEMANAGER_H

#include "D3D9Common.h"
#include "DeviceSettings.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class GraphicsDeviceManager
			{
			private:
				DeviceSettings* m_currentSetting;
				IDirect3D9* m_direct3D9;
				D3DDevice* m_device;

				Game* m_game;

				bool m_ignoreSizeChanges;
				bool m_deviceLost;

				bool m_doNotStoreBufferSize;
				bool m_renderingOccluded;

				int32 m_fullscreenWindowWidth;
				int32 m_fullscreenWindowHeight;
				int32 m_windowedWindowWidth;
				int32 m_windowedWindowHeight;
				WINDOWPLACEMENT m_windowedPlacement;
				int64 m_windowedStyle;
				//bool m_savedTopmost;

				void PropogateSettings();
				bool CanDeviceBeReset(const DeviceSettings* const oldset, const DeviceSettings* const newset) const;
				void CreateDevice(const DeviceSettings &settings);
				void game_FrameStart(bool* cancel);
				void game_FrameEnd();
				void Window_UserResized();
				void Window_MonitorChanged();

				void InitializeDevice();
				HRESULT ResetDevice();

				int32 GetAdapterOrdinal(HMONITOR mon);
				void UpdateDeviceInformation();

			public:
				GraphicsDeviceManager(Game* game);
				~GraphicsDeviceManager(void);

				D3DDevice* getDevice() const { return m_device; }
				IDirect3D9* getDirect3D() const { return m_direct3D9; }


				/* Ensures that the device is properly initialized and ready to render.
				*/
				bool EnsureDevice() const
				{
					return m_device && !m_deviceLost;
				}

				/** Changes the device.
					param
					@settings The settings.
					@minimumSettings The minimum settings.
				*/
				void ChangeDevice(const DeviceSettings& settings, const DeviceSettings* minimumSettings);
				
				/** Changes the device.
					param
					@windowed if set to true, the application will run in windowed mode instead of full screen.
					@desiredWidth Desired width of the window.
					@desiredHeight Desired height of the window.
				*/
				void ChangeDevice(bool windowed, int desiredWidth, int desiredHeight);

				/** Changes the device.
					param
					@settings The settings.
				*/
				void ChangeDevice(const DeviceSettings& settings);

				/* Toggles between full screen and windowed mode.
				*/
				void ToggleFullScreen();
				void ReleaseDevice();

			};
		}
	}
};
#endif