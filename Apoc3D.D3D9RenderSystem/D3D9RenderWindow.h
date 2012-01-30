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
#include "D3D9RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RenderView : public RenderView
			{
			private:
				D3D9RenderViewSet* m_viewSet;
				D3D9RenderDevice* m_device;
				IDirect3DSwapChain9* m_swapChain;
				HANDLE m_controlHandle;

			public:

				D3D9RenderView(D3D9RenderDevice* device, D3D9RenderViewSet* viewSet, IDirect3DSwapChain9* chain, const RenderParameters& pm);
				~D3D9RenderView();


				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present(const GameTime* const time);
			};


			class D3D9RenderWindow : public RenderWindow
			{
			private:
				/** This Game classes and its dependent classes were once used in labtd. 
				 *  Now they are just wrapped to fit the new render system interface. This D3D9Game is only the wrapper class.
				 *  The Game class does the real business. It has its own GameWindow class which is the real place to
				 *  handle the windows messages. The Game class also has the GraphicsDeviceManager class,
				 *  which is in charge of creating suitable device and handling device lost.
				 *
				 *  Most overrided methods in this class are called passively by the GraphicsDeviceManager, the Game::Create() method,
				 *  and the GameWindow class. As being said above, these classes works on their own. D3D9Game is the "implementation" but as
				 *  a wrapper redirects the calls from Game and etc to the D3D9RenderWindow class, which is associated with the 
				 *  client application event handler to handle the Load(), Initialize() operations.
				 *  Thus, make sure to draw a line here. Maybe that helps understanding or reviewing the code.
				 */
				class D3D9Game : public Game
				{
				private:
					D3D9RenderWindow* m_window;

				public:
					D3D9Game(D3D9RenderWindow* wnd, IDirect3D9* d3d9)
						: Game(L"Apoc3D Engine - Direct3D9", d3d9), m_window(wnd)
					{
					}
					virtual void Create();
					
					virtual void Release() 
					{
						// First use this to make sure the Unload is handled first
						Game::Release();
						// then finalize
						m_window->OnFinalize();
					}
					virtual void Initialize()
					{
						// The window will be only initialized once, even in some cases, like device losts
						// when this is called again.
						if (!((D3D9RenderDevice*)m_window->getRenderDevice())->isInitialized())
						{
							m_window->getRenderDevice()->Initialize();
							m_window->OnInitialize(); // will make the event handler interface to process the event
						}
					}
					virtual void LoadContent()
					{
						m_window->OnLoad();
					}
					virtual void OnDeviceLost() 
					{
						D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_window->getRenderDevice());
						if (device)
							device->OnDeviceLost();
					}
					virtual void OnDeviceReset()
					{
						D3D9RenderDevice* device = static_cast<D3D9RenderDevice*>(m_window->getRenderDevice());
						if (device)
							device->OnDeviceReset();
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

				D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContent* dc, const RenderParameters& pm);
				~D3D9RenderWindow();

				virtual void Exit();
				virtual void Run();

				virtual String getTitle();
				virtual void setTitle(const String& name);

				virtual Size getClientSize();

				virtual bool getIsActive() const;
			private:
				D3D9Game* m_game;
				D3D9DeviceContent* m_dc;
				void setDevice(RenderDevice* device)
				{
					m_renderDevice = device;
				}
			};
		}
	}
}

#endif