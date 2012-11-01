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

#include "D3D9RenderWindow.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"
#include "GameWindow.h"
#include "D3D9DeviceContent.h"
#include "Enumeration.h"
#include "D3D9RenderViewSet.h"
#include "DeviceSettings.h"
#include "Core/Logging.h"


namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RenderView::D3D9RenderView(D3D9RenderDevice* device, D3D9RenderViewSet* viewSet, IDirect3DSwapChain9* chain, const RenderParameters& pm)
				: RenderView(device, pm), m_viewSet(viewSet),  m_swapChain(chain)//, m_deviceLost(false)
			{
				m_controlHandle = reinterpret_cast<HANDLE>(pm.TargetHandle);
			}
			D3D9RenderView::~D3D9RenderView()
			{
				m_swapChain->Release();
			}

			void D3D9RenderView::ChangeRenderParameters(const RenderParameters& params)
			{
				DeviceSettings settings;
				settings.AdapterOrdinal = 0;
				settings.BackBufferCount = 1;
				settings.BackBufferHeight = params.BackBufferHeight;
				settings.BackBufferWidth = params.BackBufferWidth;
				settings.BackBufferFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				settings.DepthStencilFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				settings.DeviceType = D3DDEVTYPE_HAL;
				settings.EnableVSync = params.EnableVSync;
				settings.MultiSampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				settings.Multithreaded = true;
				settings.RefreshRate = 0;
				settings.Windowed = true;			

				m_viewSet->ChangeDevice(settings, 0);
			}
			void D3D9RenderView::Present(const GameTime* const time)
			{
				m_swapChain->Present(NULL,NULL,NULL,NULL, NULL);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			// The implementation of the D3D9Game class is just as the way labtd once uses it to initialize D3DDevice.
			// Despite some more features like the device enumeration were added to the GraphicsDeviceManager to 
			// increase reliability on latency hardwares, the way to use it almost remains unchanged.
			void D3D9RenderWindow::D3D9Game::Create()
			{
				// with this call, the RenderWindow and Game object are created.
				Game::Create();
				
				const RenderParameters& params = m_window->getRenderParams();

				DeviceSettings settings;
				settings.AdapterOrdinal = 0;
				settings.BackBufferCount = 1;
				settings.BackBufferHeight = params.BackBufferHeight;
				settings.BackBufferWidth = params.BackBufferWidth;
				settings.BackBufferFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				settings.DepthStencilFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				settings.DeviceType = D3DDEVTYPE_HAL;
				settings.EnableVSync = params.EnableVSync;
				settings.MultiSampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				settings.Multithreaded = true;
				settings.RefreshRate = 0;
				settings.Windowed = params.IsWindowd;				


				D3D9RenderDevice* device = new D3D9RenderDevice(getGraphicsDeviceManager());
				m_window->setDevice(device);
				m_window->m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[D3D9]Creating render window. ", 
					LOGLVL_Infomation);

				// Initialize() and Load() are called as the device is being created.
				getGraphicsDeviceManager()->ChangeDevice(settings);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[D3D9]Render window created. ", 
					LOGLVL_Infomation);

				//device->Initialize();
			}


			D3D9RenderWindow::D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContent* dc, const RenderParameters& pm)
				: RenderWindow(device, pm), m_dc(dc)
			{
				m_game = new D3D9Game(this, dc->getD3D());

				
			}
			D3D9RenderWindow::~D3D9RenderWindow()
			{
				// clean up traces.
				m_dc->NotifyWindowClosed(this);
				delete m_game;
			}

			void D3D9RenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);

				DeviceSettings settings;
				settings.AdapterOrdinal = 0;
				settings.BackBufferCount = 1;
				settings.BackBufferHeight = params.BackBufferHeight;
				settings.BackBufferWidth = params.BackBufferWidth;
				settings.BackBufferFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				settings.DepthStencilFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				settings.DeviceType = D3DDEVTYPE_HAL;
				settings.EnableVSync = params.EnableVSync;
				settings.MultiSampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				settings.Multithreaded = true;
				settings.RefreshRate = 0;
				settings.Windowed = params.IsWindowd;				

				m_game->getWindow()->MakeFixedSize(params.IsFixedWindow);
				
				m_game->getGraphicsDeviceManager()->ChangeDevice(settings);

			}


			void D3D9RenderWindow::Exit()
			{
				RenderWindow::Exit();
				m_game->Exit();
			}

			void D3D9RenderWindow::Run()
			{
				// This Run method is called from outside client code to create a render window
				// for the first time and right after when the constructor is called. No other methods are
				// needed. 
				
				// Creates almost every thing
				m_game->Create();
				
				m_game->Run();
				// Releases almost every thing
				m_game->Release();
			}

			String D3D9RenderWindow::getTitle()
			{
				return m_game->getWindow()->getWindowTitle();
			}
			void D3D9RenderWindow::setTitle(const String& name)
			{
				m_game->getWindow()->setWindowTitle(name);
			}

			Size D3D9RenderWindow::getClientSize()
			{
				return m_game->getWindow()->getCurrentSize();
			}
			bool D3D9RenderWindow::getIsActive() const
			{
				return m_game->getIsActive();
			}
		}
	}
}