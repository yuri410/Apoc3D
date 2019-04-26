#pragma once

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

#ifndef GL3RENDERWINDOW_H
#define GL3RENDERWINDOW_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"
#include "Game.h"
#include "GL3RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3RenderView : public RenderView
			{
			public:
				GL3RenderView(GL3RenderDevice* device, const RenderParameters& pm);
				~GL3RenderView();

				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present(const GameTime* const time);

			private:
				GL3RenderDevice* m_device;

				HANDLE m_controlHandle;
			};


			class GL3RenderWindow final : public RenderWindow
			{
			private:
				class GLGame : public Game
				{
				private:
					GL3RenderWindow* m_window;

				public:
					GLGame(GL3RenderWindow* wnd, GL3DeviceContext* devCont)
						: Game(devCont, L"Apoc3D Engine - OpenGL 3.1"), m_window(wnd)
					{
					}
					virtual void Create();

					virtual void Release() 
					{
						Game::Release();
						m_window->OnFinalize();
					}
					virtual void Initialize()
					{
						if (!((GL3RenderDevice*)m_window->getRenderDevice())->isInitialized())
						{
							m_window->getRenderDevice()->Initialize();
							m_window->OnInitialize();
						}
					}
					virtual void LoadContent()
					{
						m_window->OnLoad();
					}
					virtual void UnloadContent()
					{
						m_window->OnUnload();
					}
					virtual void Render(const GameTime* const time)
					{
						m_window->OnDraw(time);
					}
					virtual void Update(const GameTime* const time)
					{
						m_window->OnUpdate(time);
					}
					virtual bool OnFrameStart()
					{
						if (Game::OnFrameStart())
						{
							m_window->OnFrameStart();
							return true;
						}
						return false;
					}
					virtual void OnFrameEnd()
					{
						m_window->OnFrameEnd();
						Game::OnFrameEnd();
					}
				};
			public:

				virtual void ChangeRenderParameters(const RenderParameters& params);

				GL3RenderWindow(GL3RenderDevice* device, GL3DeviceContext* dc, const RenderParameters& pm);
				~GL3RenderWindow();

				virtual void Exit();
				virtual void Run();

				virtual String getTitle();
				virtual void setTitle(const String& name);

				virtual Size getClientSize();

			private:
				GL3DeviceContext* m_dc;
				GLGame* m_game;

				void setDevice(RenderDevice* device)
				{
					m_renderDevice = device;
				}
			};
		}
	}
}

#endif