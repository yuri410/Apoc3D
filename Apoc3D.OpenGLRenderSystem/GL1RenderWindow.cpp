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

#include "GL1RenderWindow.h"
#include "GL1RenderDevice.h"
#include "GL1Utils.h"
#include "GraphicsDeviceManager.h"

#include "GL1DeviceContent.h"
#include "GL1RenderViewSet.h"
#include "Core/Logging.h"

#include "Win32GameWindow.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
			
			void D3D9RenderWindow::GLGame::Create()
			{
				Game::Create();

				const RenderParameters& params = m_window->getRenderParams();

				//DeviceSettings settings;
				//settings.AdapterOrdinal = 0;
				//settings.BackBufferCount = 1;
				//settings.BackBufferHeight = params.BackBufferHeight;
				//settings.BackBufferWidth = params.BackBufferWidth;
				//settings.BackBufferFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				//settings.DepthStencilFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				//settings.DeviceType = D3DDEVTYPE_HAL;
				//settings.EnableVSync = params.EnableVSync;
				//settings.MultiSampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				//settings.Multithreaded = true;
				//settings.RefreshRate = 0;
				//settings.Windowed = params.IsWindowd;				

				GL1RenderDevice* device = new GL1RenderDevice();
				m_window->setDevice(device);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL1]Creating render window. ", 
					LOGLVL_Infomation);

				getGraphicsDeviceManager()->ChangeDevice(params);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL1]Render window created. ", 
					LOGLVL_Infomation);

				//device->Initialize();
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
			GL1RenderWindow::GL1RenderWindow(GL1RenderDevice* device, GL1DeviceContent* dc, const RenderParameters& pm)
				: RenderWindow(device, pm), m_dc(dc)
			{
				m_game = new Game(this);
			}
			GL1RenderWindow::~GL1RenderWindow()
			{
				m_dc->NotifyWindowClosed(this);
				delete m_game;
			}

			void GL1RenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);

				//DeviceSettings settings;
				//settings.AdapterOrdinal = 0;
				//settings.BackBufferCount = 1;
				//settings.BackBufferHeight = params.BackBufferHeight;
				//settings.BackBufferWidth = params.BackBufferWidth;
				//settings.BackBufferFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				//settings.DepthStencilFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				//settings.DeviceType = D3DDEVTYPE_HAL;
				//settings.EnableVSync = params.EnableVSync;
				//settings.MultiSampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				//settings.Multithreaded = true;
				//settings.RefreshRate = 0;
				//settings.Windowed = params.IsWindowd;				

				m_game->getGraphicsDeviceManager()->ChangeDevice(settings);

			}


			void GL1RenderWindow::Exit()
			{
				RenderWindow::Exit();
				m_game->Exit();
			}

			void GL1RenderWindow::Run()
			{
				
				m_game->Create();
				m_game->Run();
				m_game->Release();
			}

			String GL1RenderWindow::getTitle()
			{
				return m_game->getWindow()->getWindowTitle();
			}
			void GL1RenderWindow::setTitle(const String& name)
			{
				m_game->getWindow()->setWindowTitle(name);
			}

			Size GL1RenderWindow::getClientSize()
			{
				return m_game->getWindow()->getCurrentSize();
			}

		}
	}
}