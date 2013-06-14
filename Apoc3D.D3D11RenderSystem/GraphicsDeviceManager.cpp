#include "GraphicsDeviceManager.h"
#include "Win32AppWindow.h"
#include "Win32AppHost.h"
#include "D3D11Enumeration.h"

using namespace rex;

GraphicsDeviceManager::GraphicsDeviceManager(Win32AppHost* game)
	: m_currentSetting(NULL), 
	m_device(NULL), m_deviceContext(NULL),
	m_swapChain(NULL), m_depthStencil(NULL), m_depthStencilView(NULL), m_renderTarget(NULL), m_rasterizerState(NULL),
	m_ignoreSizeChanges(false), m_doNotStoreBufferSize(false), m_renderingOccluded(false)
{
	assert(game);

	m_game = game;
	
	CreateDXGIFactory1(__uuidof( IDXGIFactory1 ), (void**)&m_dxgi);

	m_game->eventFrameStart.Bind(this, &GraphicsDeviceManager::game_FrameStart);
	m_game->eventFrameEnd.Bind(this, &GraphicsDeviceManager::game_FrameEnd);
	m_game->getWindow()->eventUserResized.Bind(this, &GraphicsDeviceManager::Window_UserResized);
}

GraphicsDeviceManager::~GraphicsDeviceManager(void)
{
	m_game->eventFrameStart.Unbind(this, &GraphicsDeviceManager::game_FrameStart);
	m_game->eventFrameEnd.Unbind(this, &GraphicsDeviceManager::game_FrameEnd);
	m_game->getWindow()->eventUserResized.Unbind(this, &GraphicsDeviceManager::Window_UserResized);

	if (m_currentSetting)
		delete m_currentSetting;
}


void GraphicsDeviceManager::ReleaseDevice()
{
	if (!m_device)
		return;

	if (m_game)
	{
		m_game->UnloadContent();
	}

	SAFE_RELEASE(m_depthStencil);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_renderTarget);
	SAFE_RELEASE(m_rasterizerState);
	SAFE_RELEASE(m_swapChain);
	
	m_deviceContext->ClearState();

	m_device->Release();
	m_device = NULL;

	m_dxgi->Release();
	m_dxgi = NULL;
}

void GraphicsDeviceManager::game_FrameStart(bool* cancel)
{
	if (!m_device)
		return;

	if (!m_game->getIsActive())
		Sleep(50);
}
void GraphicsDeviceManager::game_FrameEnd()
{
	UINT presentFlags = 0;
	if (m_renderingOccluded)
		presentFlags |= DXGI_PRESENT_TEST;
	else
		presentFlags |= m_currentSetting->PresentFlags;

	HRESULT hr = m_swapChain->Present(m_currentSetting->SyncInterval, presentFlags);

	if (hr == DXGI_STATUS_OCCLUDED)
	{
		m_renderingOccluded = true;
	}
	else if (hr == DXGI_ERROR_DEVICE_RESET)
	{
		ResetDevice();
	}
	else
	{
		m_renderingOccluded = false;
	}
}
bool GraphicsDeviceManager::CanDeviceBeReset(const D3D11DeviceSettings* oldSettings, const D3D11DeviceSettings* newSettings)
{
	if (oldSettings == nullptr)
		return false;

	return m_device && oldSettings->AdapterOrdinal == newSettings->AdapterOrdinal &&
		oldSettings->DriverType == newSettings->DriverType &&
		oldSettings->CreateFlags == newSettings->CreateFlags &&
		oldSettings->sd.SampleDesc.Count == newSettings->sd.SampleDesc.Count &&
		oldSettings->sd.SampleDesc.Quality == newSettings->sd.SampleDesc.Quality;
}
void GraphicsDeviceManager::ResizeDXGIBuffers(int width, int height, bool fullscreen)
{
	m_currentSetting->sd.Windowed = !fullscreen;

	if (m_depthStencil)
	{
		m_depthStencil->Release();
		m_depthStencil = NULL;

		m_depthStencilView->Release();
		m_depthStencilView = NULL;

		m_rasterizerState->Release();
		m_rasterizerState = NULL;
	}

	m_renderTarget->Release();
	m_renderTarget = NULL;

	UINT swapChainFlag = 0;
	if (fullscreen)
		swapChainFlag |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	m_swapChain->ResizeBuffers(m_currentSetting->sd.BufferCount, width, height, m_currentSetting->sd.BufferDesc.Format, swapChainFlag);
	
	if (!m_doNotStoreBufferSize)
	{
		RECT clRect;
		GetClientRect(m_game->getWindow()->getHandle(), &clRect);
		m_currentSetting->sd.BufferDesc.Width = clRect.right - clRect.left;
		m_currentSetting->sd.BufferDesc.Height = clRect.bottom - clRect.top;
	}

	SetupD3DViews();
}
void GraphicsDeviceManager::SetupD3DViews()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* pBackBuffer;
	m_swapChain->GetBuffer( 0, __uuidof( *pBackBuffer ), ( LPVOID* )&pBackBuffer );
	D3D11_TEXTURE2D_DESC backBufferSurfaceDesc;
	pBackBuffer->GetDesc( &backBufferSurfaceDesc );

	// Setup the viewport to match the backbuffer
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)backBufferSurfaceDesc.Width;
	vp.Height = (FLOAT)backBufferSurfaceDesc.Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports( 1, &vp );

	ID3D11DepthStencilView* pDSV = NULL;
	ID3D11RenderTargetView* pRTV = NULL;

	// Create the render target view
	hr = m_device->CreateRenderTargetView( pBackBuffer, NULL, &pRTV );
	SAFE_RELEASE( pBackBuffer );
	
	if (m_currentSetting->AutoCreateDepthStencil)
	{
		// Create depth stencil texture
		ID3D11Texture2D* pDepthStencil = NULL;
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = backBufferSurfaceDesc.Width;
		descDepth.Height = backBufferSurfaceDesc.Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = m_currentSetting->AutoDepthStencilFormat;
		descDepth.SampleDesc.Count = m_currentSetting->sd.SampleDesc.Count;
		descDepth.SampleDesc.Quality = m_currentSetting->sd.SampleDesc.Quality;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = m_device->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
		
		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.Flags = 0;
		if( descDepth.SampleDesc.Count > 1 )
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = m_device->CreateDepthStencilView( pDepthStencil, &descDSV, &pDSV );

		D3D11_RASTERIZER_DESC rsDesc;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.MultisampleEnable = descDepth.SampleDesc.Count>1 ? TRUE : FALSE;
		m_device->CreateRasterizerState(&rsDesc, &m_rasterizerState);
		m_deviceContext->RSSetState(m_rasterizerState);

		m_depthStencil = pDepthStencil;
	}

	// Set the render targets
	m_deviceContext->OMSetRenderTargets( 1, &pRTV, pDSV );

	m_depthStencilView = pDSV;
	m_renderTarget = pRTV;

}
void GraphicsDeviceManager::Window_UserResized()
{
	if (m_ignoreSizeChanges || !EnsureDevice()) // || (m_currentSetting->Windowed)
		return;

	D3D11DeviceSettings newSettings = *m_currentSetting;

	BOOL fullscreen;

	IDXGIOutput* output;
	HRESULT hr = m_swapChain->GetFullscreenState(&fullscreen, &output);
	if (SUCCEEDED(hr))
	{
		ResizeDXGIBuffers(0,0,!!fullscreen);
		ShowWindow(m_game->getWindow()->getHandle(), SW_SHOW);
	}
}
void GraphicsDeviceManager::InitializeDevice()
{
	m_dxgi->MakeWindowAssociation(m_game->getWindow()->getHandle(), 0);

	IDXGIAdapter1* adapter = NULL;
	
	D3D_DRIVER_TYPE ddt = m_currentSetting->DriverType;
	if( m_currentSetting->DriverType == D3D_DRIVER_TYPE_HARDWARE ) 
	{
		m_dxgi->EnumAdapters1( m_currentSetting->AdapterOrdinal, &adapter );
		ddt = D3D_DRIVER_TYPE_UNKNOWN;    
	}
	else if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_WARP) 
	{
		ddt = D3D_DRIVER_TYPE_WARP;  
		adapter = NULL;
	}
	else if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_REFERENCE) 
	{
		ddt = D3D_DRIVER_TYPE_REFERENCE;
		adapter = NULL;
	}
	
	ID3D11DeviceContext* pd3dImmediateContext = NULL;
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT result = D3D11CreateDevice(adapter, ddt, (HMODULE)0, 
		m_currentSetting->CreateFlags,
		&m_currentSetting->DeviceFeatureLevel, 1,
		D3D11_SDK_VERSION,
		&m_device,
		&FeatureLevel, &pd3dImmediateContext);
	assert(SUCCEEDED(result));

	result = m_dxgi->CreateSwapChain(m_device, &m_currentSetting->sd, &m_swapChain);
	assert(SUCCEEDED(result));

	SetupD3DViews();

	UpdateDeviceStats();
	
	m_game->Initialize();
	m_game->LoadContent();
}
void GraphicsDeviceManager::ResetDevice()
{
	DXGI_SWAP_CHAIN_DESC desc;
	m_swapChain->GetDesc(&desc);

	D3D11DeviceSettings newSettings = *m_currentSetting;
	if (newSettings.sd.Windowed && desc.Windowed)
	{
		RECT clRect;
		GetClientRect(m_game->getWindow()->getHandle(), &clRect);
		newSettings.sd.BufferDesc.Width = clRect.right-clRect.left;
		newSettings.sd.BufferDesc.Height = clRect.bottom-clRect.top;
	}

	if (newSettings.sd.Windowed != desc.Windowed)
	{
		if (newSettings.sd.Windowed)
		{
			m_swapChain->SetFullscreenState(FALSE, nullptr);
		}
		else
		{
			m_doNotStoreBufferSize = true;
			m_swapChain->SetFullscreenState(TRUE, nullptr);
			m_doNotStoreBufferSize = false;
			m_swapChain->ResizeTarget(&newSettings.sd.BufferDesc);
		}
	}
	else
	{
		if (newSettings.sd.BufferDesc.Width == desc.BufferDesc.Width &&
			newSettings.sd.BufferDesc.Height == desc.BufferDesc.Height &&
			newSettings.sd.BufferDesc.Format != desc.BufferDesc.Format)
			ResizeDXGIBuffers(0,0, !newSettings.sd.Windowed);
		else if (newSettings.sd.BufferDesc.Width != desc.BufferDesc.Width ||
			newSettings.sd.BufferDesc.Height != desc.BufferDesc.Height)
			m_swapChain->ResizeTarget(&newSettings.sd.BufferDesc);
	}

	UpdateDeviceStats();
}
void GraphicsDeviceManager::UpdateDeviceStats()
{
	std::wstringstream wss;

	if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_HARDWARE)
		wss << L"HAL";
	else if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_REFERENCE)
		wss << L"REF";
	else if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_SOFTWARE)
		wss << L"SW";
	else if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_WARP)
		wss << L"WRP";

	if (m_currentSetting->DriverType == D3D_DRIVER_TYPE_HARDWARE)
	{
		wss << L": ";

		String desc = GetCurrentAdapterDescription();
		if (desc.size())
		{
			wss << desc.c_str();
		}
		else
		{
			wss << L"Unknown";
		}
	}

	String msg = String(L"[D3D11]Created device: ") + wss.str();
	OutputDebugString(msg.c_str());
}
void GraphicsDeviceManager::CreateDevice(const D3D11DeviceSettings& settings)
{
	D3D11DeviceSettings* oldSettings = m_currentSetting;
	m_currentSetting = new D3D11DeviceSettings(settings);

	m_ignoreSizeChanges = true;

	bool keepCurrentWindowSize = false;
	if (settings.sd.BufferDesc.Width == 0 &&
		settings.sd.BufferDesc.Height == 0)
		keepCurrentWindowSize = true;

	Win32AppWindow* wnd = m_game->getWindow();


	if (settings.sd.Windowed)
	{
		if (oldSettings && !oldSettings->sd.Windowed)
		{
			m_fullscreenWindowWidth = oldSettings->sd.BufferDesc.Width;
			m_fullscreenWindowHeight = oldSettings->sd.BufferDesc.Height;
		}
	}
	else
	{
		if (oldSettings && oldSettings->sd.Windowed)
		{
			m_windowedWindowWidth = oldSettings->sd.BufferDesc.Width;
			m_windowedWindowHeight = oldSettings->sd.BufferDesc.Height;
		}
	}

	// check if the device can be reset, or if we need to completely recreate it

	bool canReset = CanDeviceBeReset(oldSettings, m_currentSetting);
	if (canReset)
		ResetDevice();
	else
	{
		if (oldSettings)
		{
			OutputDebugString(L"[D3D11]Recreating Device. ");
			ReleaseDevice();
		}
		InitializeDevice();
	}


	// check if we need to resize
	if (settings.sd.Windowed && !keepCurrentWindowSize)
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
				AdjustWindowRect(&frame, GetWindowLong(wnd->getHandle(), GWL_STYLE), false);
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
		if (width != settings.sd.BufferDesc.Width ||
			height != settings.sd.BufferDesc.Height)
		{
			if (IsIconic(wnd->getHandle()))
				ShowWindow(wnd->getHandle(), SW_RESTORE);
			if (IsZoomed(wnd->getHandle()))
				ShowWindow(wnd->getHandle(), SW_RESTORE);

			RECT rect = RECT();
			rect.right = settings.sd.BufferDesc.Width;
			rect.bottom = settings.sd.BufferDesc.Height;
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
			if (clientWidth != settings.sd.BufferDesc.Width ||
				clientHeight != settings.sd.BufferDesc.Height)
			{
				D3D11DeviceSettings newSettings = settings;
				newSettings.sd.BufferDesc.Width = 0;
				newSettings.sd.BufferDesc.Height = 0;

				CreateDevice(newSettings);
			}
		}
	}

	// if the window is still hidden, make sure it is shown
	if (!IsWindowVisible(wnd->getHandle()))
		ShowWindow(wnd->getHandle(), SW_SHOW);

	// set the execution state of the thread
	if (!m_currentSetting->sd.Windowed)
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
	else
		SetThreadExecutionState(ES_CONTINUOUS);

	m_ignoreSizeChanges = false;

	if (oldSettings)
		delete oldSettings;
}
void GraphicsDeviceManager::ChangeDevice(const D3D11DeviceSettings& settings, const D3D11DeviceSettings* minimumSettings)
{
	if (minimumSettings)
	{
		D3D11Enumeration::GetInstance()->SetMinimumSettings(*minimumSettings);
	}
	else
	{
		D3D11Enumeration::GetInstance()->ClearMinimumSettings();
	}
	D3D11DeviceSettings validSettings;
	D3D11Enumeration::GetInstance()->FindValidSettings(m_dxgi, settings, validSettings);
	validSettings.sd.OutputWindow = m_game->getWindow()->getHandle();

	CreateDevice(validSettings);
}
void GraphicsDeviceManager::ChangeDevice(bool windowed, int desiredWidth, int desiredHeight)
{
	D3D11DeviceSettings desiredSettings = D3D11DeviceSettings();
	desiredSettings.sd.Windowed = (windowed);
	desiredSettings.sd.BufferDesc.Width = (desiredWidth);
	desiredSettings.sd.BufferDesc.Height = (desiredHeight);

	ChangeDevice(desiredSettings, 0);
}
void GraphicsDeviceManager::ChangeDevice(const D3D11DeviceSettings& prefer)
{
	ChangeDevice(prefer, nullptr);
}
void GraphicsDeviceManager::ToggleFullScreen()
{
	assert(EnsureDevice());

	D3D11DeviceSettings newSettings = *m_currentSetting;
	newSettings.sd.Windowed =  !newSettings.sd.Windowed;

	int width = newSettings.sd.Windowed ? m_windowedWindowWidth :  m_fullscreenWindowWidth;
	int height = newSettings.sd.Windowed ?  m_windowedWindowHeight :  m_fullscreenWindowHeight;

	newSettings.sd.BufferDesc.Width =  width;
	newSettings.sd.BufferDesc.Height = height;

	ChangeDevice(newSettings);
}

String GraphicsDeviceManager::GetCurrentAdapterDescription()
{
	D3D11EnumDeviceSettingsCombo* pDeviceSettingsCombo = D3D11Enumeration::GetInstance()->GetDeviceSettingsCombo(
		m_currentSetting->AdapterOrdinal, m_currentSetting->DriverType, m_currentSetting->Output,
		m_currentSetting->sd.BufferDesc.Format, m_currentSetting->sd.Windowed );

	if( pDeviceSettingsCombo )
	{
		return pDeviceSettingsCombo->pAdapterInfo->UniqueDescription;
	}
	return L"";
}