#include "D3D11RenderWindow.h"
#include "Win32AppWindow.h"
#include "D3D11RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "D3D11Utils.h"

namespace rex
{
	void D3D11RenderWindow::D3D11Game::Create(const RenderParameters& params)
	{
		// with this call, the RenderWindow and the Game object are created.
		Win32AppHost::Create(params);			

		D3D11DeviceSettings settings;
		settings.AdapterOrdinal = 0;
		settings.sd.BufferCount = 1;
		settings.sd.BufferDesc.Height = params.BackBufferHeight;
		settings.sd.BufferDesc.Width = params.BackBufferWidth;
		settings.sd.BufferDesc.Format = dutConvertPixelFormat(params.ColorBufferFormat);
		settings.sd.SampleDesc.Count = params.FSAASampleCount;
		settings.sd.Windowed = params.IsWindowd;

		settings.AutoDepthStencilFormat = dutConvertDepthFormat(params.DepthBufferFormat);
		settings.DriverType = D3D_DRIVER_TYPE_HARDWARE;
		settings.SyncInterval = params.EnableVSync ? 1 : 0;

		D3D11RenderDevice* device = new D3D11RenderDevice(getGraphicsDeviceManager());
		m_window->setDevice(device);
		//m_window->m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);

		OutputDebugString(L"[D3D11]Creating render window. ");

		//getGraphicsDeviceManager()->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;

		// Initialize() and Load() are called as the device is being created.
		getGraphicsDeviceManager()->ChangeDevice(settings);

		OutputDebugString(L"[D3D11]Render window created. ");
	}
	void D3D11RenderWindow::D3D11Game::Initialize()
	{
		String adpDesc = getGraphicsDeviceManager()->GetCurrentAdapterDescription();
		if (adpDesc.size())
			m_window->m_hardwareName = adpDesc;

		m_window->getRenderDevice()->Initialize();
		m_window->OnInitialize(); // will make the event handler interface to process the event
	}


	D3D11RenderWindow::D3D11RenderWindow(D3D11RenderDevice* device, D3D11DeviceContext* dc, const RenderParameters& pm)
		: RenderWindow(dc, device, pm), m_dc(dc)
	{
		m_game = new D3D11Game(this);
	}
	D3D11RenderWindow::~D3D11RenderWindow()
	{
		// clean up traces.
		//m_dc->NotifyWindowClosed(this);
		delete m_game;
	}

	void D3D11RenderWindow::ChangeRenderParameters(const RenderParameters& params)
	{
		RenderWindow::ChangeRenderParameters(params);

		D3D11DeviceSettings settings;
		settings.AdapterOrdinal = 0;
		settings.sd.BufferCount = 1;
		settings.sd.BufferDesc.Height = params.BackBufferHeight;
		settings.sd.BufferDesc.Width = params.BackBufferWidth;
		settings.sd.BufferDesc.Format = dutConvertPixelFormat(params.ColorBufferFormat);
		settings.sd.SampleDesc.Count = params.FSAASampleCount;
		settings.sd.Windowed = params.IsWindowd;

		settings.AutoDepthStencilFormat = dutConvertDepthFormat(params.DepthBufferFormat);
		settings.DriverType = D3D_DRIVER_TYPE_HARDWARE;
		settings.SyncInterval = params.EnableVSync ? 1 : 0;
		

		m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);

		//m_game->getGraphicsDeviceManager()->UserIgnoreMonitorChanges() = params.IgnoreMonitorChange;
		m_game->getGraphicsDeviceManager()->ChangeDevice(settings);

	}


	void D3D11RenderWindow::Exit()
	{
		RenderWindow::Exit();
		m_game->Exit();
	}

	void D3D11RenderWindow::Run()
	{
		// This Run method is called from outside client code to create a render window
		// for the first time and right after when the constructor is called. No other methods are
		// needed. 

		// Creates almost every thing
		m_game->Create(getRenderParams());

		m_game->Run();
		// Releases almost every thing
		m_game->Release();
	}

	String D3D11RenderWindow::getTitle()
	{
		return m_game->getWindow()->getWindowTitle();
	}
	void D3D11RenderWindow::setTitle(const String& name)
	{
		m_game->getWindow()->setWindowTitle(name);
	}

	Size D3D11RenderWindow::getClientSize()
	{
		return m_game->getWindow()->getCurrentSize();
	}
	bool D3D11RenderWindow::getIsActive() const
	{
		return m_game->getIsActive();
	}

	void D3D11RenderWindow::setDevice(RenderDevice* device)
	{
		m_renderDevice = device;
	}

	void D3D11RenderWindow::SetVisible(bool v)
	{
		ShowWindow(m_game->getWindow()->getHandle(), v ? SW_NORMAL : SW_HIDE);

		m_visisble = v;
	}
}