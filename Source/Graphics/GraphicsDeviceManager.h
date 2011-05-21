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

#pragma once

#include "Common.h"
#include "DeviceSettings.h"

namespace Apoc3D
{
	namespace Graphics
	{
		class APOC3D_API GraphicsDeviceManager
		{
		private:
			DeviceSettings* m_currentSetting;
			IDirect3D9* m_direct3D9;
			Device* m_device;

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


			bool CanResetDevice(const DeviceSettings* const oldset, const DeviceSettings* const newset) const;
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

			Device* getDevice() const { return m_device; }
			IDirect3D9* getDirect3D() const { return m_direct3D9; }


			/* Ensures that the device is properly initialized and ready to render.
			*/
			bool EnsureDevice() const
			{
				return m_device && !m_deviceLost;
			}

			void ChangeDevice(bool windowed, int desiredWidth, int desiredHeight);

			/* Changes the device.
			*/
			void ChangeDevice(const DeviceSettings &prefer);
			/* Toggles between full screen and windowed mode.
			*/
			void ToggleFullScreen();
			void ReleaseDevice();

		};
	}
};
#endif