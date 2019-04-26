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

#include "GL3RenderWindow.h"
#include "GL3RenderDevice.h"
#include "GL3Utils.h"

#include "GL3DeviceContext.h"
#include "apoc3d/Core/Logging.h"

#include "Win32GameWindow.h"
#include "GraphicsDeviceManager.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			void GL3RenderWindow::GLGame::Create()
			{
				// with this call, the RenderWindow and Game object are created.
				Game::Create();

				const RenderParameters& params = m_window->getRenderParams();

				GL3RenderDevice* device = new GL3RenderDevice(getGraphicsDeviceManager());
				m_window->setDevice(device);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL3]Creating render window. ", 
					LOGLVL_Infomation);

				// Initialize() and Load() are called as the device is being created.
				// The GraphicsDeviceManager here accepts RenderParameters directly.
				getGraphicsDeviceManager()->ChangeDevice(params);

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"[GL3]Render window created. ", 
					LOGLVL_Infomation);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			GL3RenderWindow::GL3RenderWindow(GL3RenderDevice* device, GL3DeviceContext* dc, const RenderParameters& pm)
				: RenderWindow(device, pm), m_dc(dc)
			{
				m_game = new GLGame(this, dc);
			}
			GL3RenderWindow::~GL3RenderWindow()
			{
				m_dc->NotifyWindowClosed(this);
				delete m_game;
			}

			void GL3RenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);


				m_game->getGraphicsDeviceManager()->ChangeDevice(params);

			}


			void GL3RenderWindow::Exit()
			{
				RenderWindow::Exit();
				m_game->Exit();
			}

			void GL3RenderWindow::Run()
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

			String GL3RenderWindow::getTitle()
			{
				return m_game->getWindow()->getWindowTitle();
			}
			void GL3RenderWindow::setTitle(const String& name)
			{
				m_game->getWindow()->setWindowTitle(name);
			}

			Size GL3RenderWindow::getClientSize()
			{
				return m_game->getWindow()->getCurrentSize();
			}

		}
	}
}