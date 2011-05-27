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
#ifndef D3D9RENDERWINDOW_H
#define D3D9RENDERWINDOW_H

#include "D3D9Common.h"
#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderWindowHandler.h"
#include "Game.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RenderWindow : public RenderWindow
			{
			private:
				class D3D9Game : public Game
				{
				private:
					D3D9RenderWindow* m_window;
				public:
					D3D9Game(D3D9RenderWindow* wnd)
						: Game(L"Apoc3D Engine - Direct3D9"), m_window(wnd)
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
						m_window->OnInitialize();
					}
					virtual void LoadContent()
					{
						m_window->OnLoad();
					}
					virtual void OnDeviceLost() {}
					virtual void OnDeviceReset() {}
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
				};
			private:
				D3D9Game* m_game;

				void setDevice(RenderDevice* device)
				{
					m_renderDevice = device;
				}
			public:

				D3D9RenderWindow(D3D9RenderDevice* device, const PresentParameters& pm);
				
				virtual void Run();

				virtual String getTitle();
				virtual void setTitle(const String& name);

				virtual Size getClientSize();
			};
		}
	}
}

#endif