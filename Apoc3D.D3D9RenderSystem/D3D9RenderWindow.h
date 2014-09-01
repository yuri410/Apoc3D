#pragma once
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
#ifndef D3D9RENDERWINDOW_H
#define D3D9RENDERWINDOW_H

#include "D3D9Common.h"
#include "Game.h"
#include "D3D9RenderDevice.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"

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
			public:

				D3D9RenderView(D3D9RenderDevice* device, D3D9DeviceContext* dc, D3D9RenderViewSet* viewSet, IDirect3DSwapChain9* chain, const RenderParameters& pm);
				~D3D9RenderView();


				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present(const GameTime* time);

			private:
				D3D9RenderViewSet* m_viewSet;
				D3D9RenderDevice* m_device;
				IDirect3DSwapChain9* m_swapChain;
				HANDLE m_controlHandle;

			};


			class D3D9RenderWindow final : public RenderWindow
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
				 */
				class D3D9Game : public Game
				{
				public:
					D3D9Game(D3D9RenderWindow* wnd, IDirect3D9* d3d9)
						: Game(L"", d3d9), m_window(wnd)
					{
					}
					virtual void Create(const RenderParameters& params);
					
					virtual void Release() 
					{
						// First use this to make sure the Unload is handled first
						Game::Release();

						// then finalize
						m_window->OnFinalize();
					}
					virtual void Initialize();
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
					virtual void Render(const GameTime* time)
					{
						m_window->OnDraw(time);
					}
					virtual void Update(const GameTime* time)
					{
						m_window->OnUpdate(time);
					}
					virtual bool OnFrameStart()
					{
						if (!Game::OnFrameStart())
						{
							m_window->OnFrameStart();
							return false;
						}
						return true;
					}
					virtual void OnFrameEnd()
					{
						m_window->OnFrameEnd();
						Game::OnFrameEnd();
					}
				private:
					D3D9RenderWindow* m_window;
					D3D9RenderDevice* m_device;
				};
			public:

				D3D9RenderWindow(D3D9RenderDevice* device, D3D9DeviceContext* dc, const RenderParameters& pm);
				~D3D9RenderWindow();

				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Exit();
				virtual void Run();

				virtual String getTitle();
				virtual void setTitle(const String& name);

				virtual Size getClientSize();

				virtual bool getIsActive() const;

				const String& getHardwareName() const { return m_hardwareName; }

				virtual void SetVisible(bool v);

			private:
				D3D9Game* m_game;
				D3D9DeviceContext* m_dc;
				String m_hardwareName;

				void setDevice(RenderDevice* device);
			};
		}
	}
}

#endif