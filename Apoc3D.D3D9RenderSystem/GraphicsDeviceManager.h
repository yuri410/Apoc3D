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

#ifndef APOC3D_D3D9_GRAPHICSDEVICEMANAGER_H
#define APOC3D_D3D9_GRAPHICSDEVICEMANAGER_H

#include "D3D9Common.h"
#include "RawSettings.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/**
			 *  A graphics device manager manages the device. It not just takes care of creation 
			 *  with setting enumeration for a more reliable starting up,
			 *  but also handle the error and changes happened like DeviceLost.
			 */
			class GraphicsDeviceManager
			{
			public:
				const RawSettings* getCurrentSetting() const { return m_currentSetting; }

				GraphicsDeviceManager(D3D9RenderWindow* game, IDirect3D9* d3d9);
				~GraphicsDeviceManager();

				bool& UserIgnoreMonitorChanges() { return m_userIgnoreMoniorChanges; }

				/* Check if the device is properly initialized and ready to render. */
				bool IsDeviceReady() const { return m_device && !m_deviceLost; }

				/** Changes the device.
					param
					@settings The settings.
					@minimumSettings The minimum settings.
				*/
				void ChangeDevice(const RenderParameters& settings, const RenderParameters* minimumSettings);
				
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
				void ChangeDevice(const RenderParameters& settings);

				/* Toggles between full screen and windowed mode. */
				void ToggleFullScreen();
				void ReleaseDevice(bool isDeviceReset);

				bool FrameStart();
				void FrameEnd();

				D3DDevice* getDevice() const { return m_device; }
				IDirect3D9* getDirect3D() const { return m_direct3D9; }

			private:
				void PropogateSettings();
				bool CanDeviceBeReset(const RawSettings* oldset, const RawSettings* newset) const;
				void CreateDevice(const RawSettings &settings);
				void ChangeDevice(const RawSettings& settings);

				void Window_UserResized();
				void Window_MonitorChanged();

				void InitializeDevice(bool isDeviceReset);
				HRESULT ResetDevice();

				int32 GetAdapterOrdinal(HMONITOR mon);
				void UpdateDeviceInformation();
								
				RawSettings* m_currentSetting = nullptr;

				IDirect3D9* m_direct3D9;
				D3DDevice* m_device = nullptr;

				D3D9RenderWindow* m_game;

				bool m_userIgnoreMoniorChanges = false;

				bool m_ignoreSizeChanges = false;
				bool m_deviceLost = false;
				bool m_handingDeviceReset = false;

				int32 m_fullscreenWindowWidth;
				int32 m_fullscreenWindowHeight;
				int32 m_windowedWindowWidth;
				int32 m_windowedWindowHeight;
				WINDOWPLACEMENT m_windowedPlacement;
				int64 m_windowedStyle;
				//bool m_savedTopmost
				

			};
		}
	}
};
#endif