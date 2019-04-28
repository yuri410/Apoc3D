#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GRAPHICSDEVICEMANAGER_H
#define GRAPHICSDEVICEMANAGER_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GraphicsDeviceManager
			{
			public:
				const RenderParameters* getCurrentSetting() const { return m_currentSetting; }

				GraphicsDeviceManager(GL3RenderWindow* game);
				~GraphicsDeviceManager();


				/* Ensures that the device is properly initialized and ready to render.
				*/
				bool EnsureDevice() const
				{
					return m_deviceCreated;
				}

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

				/* Toggles between full screen and windowed mode.
				*/
				void ToggleFullScreen();
				void ReleaseDevice();

				void Present();
			private:
				RenderParameters* m_currentSetting = nullptr;

				HDC m_hDC;
				HGLRC m_hRC;
				bool m_deviceCreated = false;
				GL3RenderWindow* m_game;

				bool m_ignoreSizeChanges = false;

				int32 m_fullscreenWindowWidth;
				int32 m_fullscreenWindowHeight;
				int32 m_windowedWindowWidth;
				int32 m_windowedWindowHeight;
				WINDOWPLACEMENT m_windowedPlacement;
				int64 m_windowedStyle;
				
				void CreateDevice(const RenderParameters &settings, const DEVMODE* mode = nullptr);

				void Window_UserResized();

				void ChangeResolution(const DEVMODE* mode);
				void InitializeDevice(const RenderParameters &settings);
			

			};
		}
	}
};
#endif