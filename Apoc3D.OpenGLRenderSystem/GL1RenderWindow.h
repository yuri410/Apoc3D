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
#ifndef GL1RENDERWINDOW_H
#define GL1RENDERWINDOW_H

#include "GL1Common.h"
#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderWindowHandler.h"
#include "Game.h"
#include "GL1RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class GL1RenderView : public RenderView
			{
			private:
				GL1RenderDevice* m_device;

				HANDLE m_controlHandle;

			public:

				GL1RenderView(GL1RenderDevice* device, const RenderParameters& pm);
				~GL1RenderView();


				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present(const GameTime* const time);
			};


			class GL1RenderWindow : public RenderWindow
			{
			private:
				class GLGame : public Game
				{
				private:
					GL1RenderWindow* m_window;

				public:
					GLGame(GL1RenderWindow* wnd, GL1DeviceContent* devCont)
						: Game(devCont, L"Apoc3D Engine - OpenGL 1.x"), m_window(wnd)
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
						if (!((GL1RenderDevice*)m_window->getRenderDevice())->isInitialized())
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

				GL1RenderWindow(GL1RenderDevice* device, GL1DeviceContent* dc, const RenderParameters& pm);
				~GL1RenderWindow();

				virtual void Exit();
				virtual void Run();

				virtual String getTitle();
				virtual void setTitle(const String& name);

				virtual Size getClientSize();

			private:
				GL1DeviceContent* m_dc;
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