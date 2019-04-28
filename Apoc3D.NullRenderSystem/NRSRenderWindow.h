#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#ifndef NRSRENDERWINDOW_H
#define NRSRENDERWINDOW_H

#include "NRSCommon.h"
#include "NRSRenderDevice.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindowHandler.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class GameClock;
			class GameWindow;
			
			class NRSRenderView : public RenderView
			{
			public:
				NRSRenderView(NRSRenderDevice* device, NRSDeviceContext* dc, const RenderParameters& pm);
				~NRSRenderView();

				virtual void ChangeRenderParameters(const RenderParameters& params);

				virtual void Present() override;

			private:
				NRSRenderDevice* m_device;
				uint64 m_controlHandle;

			};

			class NRSRenderWindow final : public RenderWindow
			{
			public:
				NRSRenderWindow(NRSRenderDevice* device, NRSDeviceContext* dc, const RenderParameters& pm);
				~NRSRenderWindow();

				virtual void ChangeRenderParameters(const RenderParameters& params) override;
				virtual void Present() override;

				virtual void Exit() override;
				virtual void Run() override;

				virtual void Minimize() override;
				virtual void Restore() override;
				virtual void Maximize() override;

				virtual String getTitle() override;
				virtual void setTitle(const String& name) override;

				virtual Size getClientSize() override;

				virtual bool getIsActive() const { return m_active; }

				virtual void SetVisible(bool v) override;

				void NRS_Initialize();
				void NRS_Finalize();

				void NRS_LoadContent();
				void NRS_UnloadContent();


				const String& getHardwareName() const { return m_hardwareName; }
				GameWindow* getWindow() const { return m_gameWindow; }

			private:

				/** When derived, this method should be overridden to initialize the graphics device,
				*  with the specific parameters and settings provided.
				*  As GraphicsDeviceManager has creates the device, Game::LoadContent and Game::Initialize
				*  will be called.
				*/
				void NRS_Create(const RenderParameters& params);

				/** This method will ask the GraphicsDeviceManager to release resources. And
				*  will cause the Game::UnloadContent to be called.
				*/
				void NRS_Release();

				/** Enters the main loop. */
				void NRS_MainLoop();

				/** Run one frame, which includes one render and X updates  */
				void NRS_Tick();

				void NRS_DrawFrame(const GameTime* time);

				/** This should be overridden to draw a frame */
				void NRS_Render(const GameTime* time);
				/** This should be overridden to allow the game to run logic such as updating the world,
				*  checking for collisions, gathering input, playing audio and etc.
				*/
				void NRS_Update(const GameTime* time);
				void NRS_UpdateConstrainedVarTimeStep(const GameTime* time);

				NRSDeviceContext* m_dc;
				String m_hardwareName;

				GameWindow* m_gameWindow;
				GameClock* m_gameClock;

				int32 m_maxSkipFrameCount = 10;
				int32 m_slowRenderFrameHits = 0;

				bool m_active = true;
			};
		}
	}
}

#endif