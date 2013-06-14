#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"
#include "D3D11DeviceSettings.h"

namespace rex
{
	class GraphicsDeviceManager
	{
	public:
		const D3D11DeviceSettings* getCurrentSetting() const { return m_currentSetting; }

		GraphicsDeviceManager(Win32AppHost* game);
		~GraphicsDeviceManager(void);

		/* Ensures that the device is properly initialized and ready to render.
		*/
		bool EnsureDevice() const { return !!m_device; }

		/** Changes the device.
			param
			@settings The settings.
			@minimumSettings The minimum settings.
		*/
		void ChangeDevice(const D3D11DeviceSettings& settings, const D3D11DeviceSettings* minimumSettings);
				
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
		void ChangeDevice(const D3D11DeviceSettings& settings);

		/* Toggles between full screen and windowed mode.
		*/
		void ToggleFullScreen();
		void ReleaseDevice();

		String GetCurrentAdapterDescription();

		ID3D11Device* getD3D11Device() const { return m_device; }
		ID3D11DeviceContext* getD3D11Context() const { return m_deviceContext; }
		ID3D11RenderTargetView* getDefaultRenderTargetView() const { return m_renderTarget; }
		ID3D11DepthStencilView* getDefaultDepthStencilView() const { return m_depthStencilView; }
	private:
		void CreateDevice(const D3D11DeviceSettings& settings);
		void game_FrameStart(bool* cancel);
		void game_FrameEnd();
		void Window_UserResized();

		void InitializeDevice();
		void ResetDevice();

		bool CanDeviceBeReset(const D3D11DeviceSettings* oldSettings, const D3D11DeviceSettings* newSettings);
		void ResizeDXGIBuffers(int width, int height, bool fullscreen);
		void SetupD3DViews();
		void UpdateDeviceStats();
		
		D3D11DeviceSettings* m_currentSetting;
		IDXGIFactory1* m_dxgi;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		IDXGISwapChain* m_swapChain;
		ID3D11Texture2D* m_depthStencil;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RenderTargetView* m_renderTarget;
		ID3D11RasterizerState* m_rasterizerState;

		Win32AppHost* m_game;

		
		bool m_ignoreSizeChanges;

		bool m_doNotStoreBufferSize;
		bool m_renderingOccluded;

		int32 m_fullscreenWindowWidth;
		int32 m_fullscreenWindowHeight;
		int32 m_windowedWindowWidth;
		int32 m_windowedWindowHeight;
		WINDOWPLACEMENT m_windowedPlacement;
		int64 m_windowedStyle;
		//bool m_savedTopmost
	};
}