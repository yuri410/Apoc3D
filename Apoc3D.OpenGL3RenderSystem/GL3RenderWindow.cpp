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

#include "GraphicsDeviceManager.h"

#include "apoc3d.Win32/Win32Window.h"
#include "apoc3d.Win32/Win32Clock.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			GL3RenderWindow::GL3RenderWindow(GL3RenderDevice* device, GL3DeviceContext* dc, const RenderParameters& pm)
				: Win32RenderWindow(dc, device, pm), m_dc(dc)
			{
				m_graphicsDeviceManager = new GraphicsDeviceManager(this);
			}
			GL3RenderWindow::~GL3RenderWindow()
			{
				// clean up traces.
				m_dc->NotifyWindowClosed(this);

				delete m_graphicsDeviceManager;
			}

			void GL3RenderWindow::Present()
			{
				m_graphicsDeviceManager->Present();
			}

			void GL3RenderWindow::Run()
			{
				// This Run method is called from outside client code to create a render window
				// for the first time and right after when the constructor is called. No other methods are
				// needed. 

				// Creates almost every thing
				GL_Create(getRenderParams());

				MainLoop();
				// Releases almost every thing
				GL_Release();
			}


			//////////////////////////////////////////////////////////////////////////

			void GL3RenderWindow::GL_Create(const RenderParameters& params)
			{
				m_gameWindow->Load(params.BackBufferWidth, params.BackBufferHeight, params.IsFixedWindow);

				m_renderDevice = new GL3RenderDevice(m_graphicsDeviceManager);

				ApocLog(LOG_Graphics, L"[GL3]Creating render window.", LOGLVL_Infomation);

				// Initialize() and Load() are called as the device is being created.
				m_graphicsDeviceManager->ChangeDevice(params);

				ApocLog(LOG_Graphics, L"[GL3]Render window created.", LOGLVL_Infomation);
			}

			void GL3RenderWindow::GL_Release()
			{
				// Unload() and Finalize() will be called here
				m_graphicsDeviceManager->ReleaseDevice();
				delete m_renderDevice;
				m_renderDevice = nullptr;
			}

			void GL3RenderWindow::GL_Initialize()    { m_renderDevice->Initialize(); OnInitialize(); }
			void GL3RenderWindow::GL_Finalize()      { OnFinalize(); }
			void GL3RenderWindow::GL_LoadContent()   { OnLoad(); }
			void GL3RenderWindow::GL_UnloadContent() { OnUnload(); }

			void GL3RenderWindow::OnRenderFrame(const AppTime* time)
			{
				if (!m_gameWindow->getIsMinimized() && m_graphicsDeviceManager->EnsureDevice())
				{
					if (!m_active)
						Sleep(50);

					OnFrameStart();
					OnDraw(time);
					OnFrameEnd();
				}
			}

			bool GL3RenderWindow::IsDeviceReady()
			{
				return true;
			}

			void GL3RenderWindow::ExecuteChangeDevice()
			{
				const RenderParameters& params = getRenderParams();
				m_gameWindow->MakeFixedSize(params.IsFixedWindow);

				m_graphicsDeviceManager->ChangeDevice(params);
			}

			const String& GL3RenderWindow::GetHardwareName() const
			{
				return m_graphicsDeviceManager->getHardwareName();
			}
		}
	}
}