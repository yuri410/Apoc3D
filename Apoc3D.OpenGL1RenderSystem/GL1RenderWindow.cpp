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

#include "GL1RenderWindow.h"
#include "GL1RenderDevice.h"
#include "GLUtils.h"

#include "GL1DeviceContent.h"
#include "apoc3d/Core/Logging.h"

#include "Win32GameWindow.h"
#include "GraphicsDeviceManager.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			// The implementation of the GLGame class is similar to D3D9RenderWindow::D3D9Game.
			// Please refer to that class.
			

			void GL1RenderWindow::GLGame::Create()
			{
				// with this call, the RenderWindow and Game object are created.
				Game::Create();

				const RenderParameters& params = m_window->getRenderParams();

				GL1RenderDevice* device = new GL1RenderDevice(getGraphicsDeviceManager());
				m_window->setDevice(device);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL1]Creating render window. ", 
					LOGLVL_Infomation);

				// Initialize() and Load() are called as the device is being created.
				// The GraphicsDeviceManager here accepts RenderParameters directly.
				getGraphicsDeviceManager()->ChangeDevice(params);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL1]Render window created. ", 
					LOGLVL_Infomation);

			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
			GL1RenderWindow::GL1RenderWindow(GL1RenderDevice* device, GL1DeviceContent* dc, const RenderParameters& pm)
				: RenderWindow(device, pm), m_dc(dc)
			{
				m_game = new GLGame(this, dc);
			}
			GL1RenderWindow::~GL1RenderWindow()
			{
				m_dc->NotifyWindowClosed(this);
				delete m_game;
			}

			void GL1RenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);


				m_game->getGraphicsDeviceManager()->ChangeDevice(params);

			}


			void GL1RenderWindow::Exit()
			{
				RenderWindow::Exit();
				m_game->Exit();
			}

			void GL1RenderWindow::Run()
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