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

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			void D3D9RenderWindow::D3D9Game::Create()
			{				
				Game::Create();
				
				const RenderParameters& params = m_window->getRenderParams();

				//TODO: getGraphicsDeviceManager()->ChangeDevice()
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

				getGraphicsDeviceManager()->ChangeDevice(settings);

				D3D9RenderDevice* device = new D3D9RenderDevice(getGraphicsDeviceManager());
				m_window->setDevice(device);

				device->Initialize();
			}
			D3D9RenderWindow::D3D9RenderWindow(D3D9RenderDevice* device, const RenderParameters& pm)
				: RenderWindow(device, pm)
			{
				m_game = new D3D9Game(this);
			}

			void D3D9RenderWindow::Exit()
			{
				RenderWindow::Exit();
				m_game->Exit();
			}

			void D3D9RenderWindow::Run()
			{			
				m_game->Create();
				m_game->Run();
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

		}
	}
}