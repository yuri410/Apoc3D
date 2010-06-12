
#ifndef GRAPHICSDEVICEMANAGER_H
#define GRAPHICSDEVICEMANAGER_H

#include "..\Common.h"
#include "DeviceSettings.h"

namespace Apoc3D
{
	namespace Graphics
	{
		class _Export GraphicsDeviceManager
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

			int m_fullscreenWindowWidth;
			int m_fullscreenWindowHeight;
			int m_windowedWindowWidth;
			int m_windowedWindowHeight;
			WINDOWPLACEMENT m_windowedPlacement;
			long m_windowedStyle;
			bool m_savedTopmost;


			bool CanResetDevice(const DeviceSettings const *oldset, const DeviceSettings const *newset) const;
			void CreateDevice(const DeviceSettings const &settings);
			void game_FrameStart(bool* cancel);
			void game_FrameEnd();
			void Window_UserResized();
			void Window_ScreenChanged();

			void InitializeDevice();
			int64 ResetDevice();

			void ReleaseDevice();

			void UpdateDeviceInformation();

		public:
			GraphicsDeviceManager(Game* game);
			~GraphicsDeviceManager(void);

			/* Ensures that the device is properly initialized and ready to render.
			*/
			bool EnsureDevice() const
			{
				return m_device && !m_deviceLost;
			}

			void ChangeDevice(bool windowed, int desiredWidth, int desiredHeight);
        
			/* Changes the device.
			*/
			void ChangeDevice(const DeviceSettings const &prefer);
			/* Toggles between full screen and windowed mode.
			*/
			void ToggleFullScreen();
			

			
		};
	}
};
#endif