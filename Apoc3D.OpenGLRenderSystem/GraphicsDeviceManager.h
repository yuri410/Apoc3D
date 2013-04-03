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
#ifndef GRAPHICSDEVICEMANAGER_H
#define GRAPHICSDEVICEMANAGER_H

#include "GL1Common.h"
#include "Graphics/RenderSystem/DeviceContent.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
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