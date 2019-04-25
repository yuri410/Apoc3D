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

				GraphicsDeviceManager(Game* game);
				~GraphicsDeviceManager(void);


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

				/** The GraphicsDeviceManager needs to know some capabilities by
				*  doing some experiments. Call this prior to the creation of any
				*  window.
				*/
				void PreTest();
			private:
				RenderParameters* m_currentSetting;

				HDC m_hDC;
				HGLRC m_hRC;
				bool m_deviceCreated;
				Game* m_game;

				bool m_ignoreSizeChanges;

				bool m_doNotStoreBufferSize;
				bool m_renderingOccluded;

				int32 m_fullscreenWindowWidth;
				int32 m_fullscreenWindowHeight;
				int32 m_windowedWindowWidth;
				int32 m_windowedWindowHeight;
				WINDOWPLACEMENT m_windowedPlacement;
				int64 m_windowedStyle;

				int m_arbFSAAFormat;
				
				void CreateDevice(const RenderParameters &settings, const DEVMODE* mode = 0);
				void game_FrameStart(bool* cancel);
				void game_FrameEnd();
				void Window_UserResized();


				void ChangeResolution(const DEVMODE& mode);
				void InitializeDevice(const RenderParameters &settings);
			

			};
		}
	}
};
#endif