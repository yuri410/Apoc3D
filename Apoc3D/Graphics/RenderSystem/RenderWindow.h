#pragma once
#ifndef APOC3D_RENDERWINDOW_H
#define APOC3D_RENDERWINDOW_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "DeviceContext.h"

#include "apoc3d/Collections/LinkedList.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 * Implements a simple FPS counter
			 */
			class APAPI FPSCounter
			{
			private:
				LinkedList<float> m_frameTimes;
				float m_fps;
			public:
				float getFPS() const { return m_fps; }
				void Step(const GameTime* const time);
			};

			/**
			 *  Represents a view of rendered graphics.
			 */
			class APAPI RenderView
			{
			public:
				virtual ~RenderView(){}

				float getFPS() const { return m_fpsCounter.getFPS(); }

				virtual void Present(const GameTime* const time);

				void* UserData;

				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				const RenderParameters& getRenderParams() const { return m_presentParams; }
				virtual void ChangeRenderParameters(const RenderParameters& params) { m_presentParams = params; }
				DeviceContext* getDeviceContext() const { return m_deviceContext; }

			protected:
				RenderDevice* m_renderDevice;

				RenderView(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: m_presentParams(pm), m_renderDevice(rd), m_renderTarget(rt), m_deviceContext(dc)
				{
				}
				RenderView(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm)
					: m_presentParams(pm), m_renderDevice(rd), m_renderTarget(0), m_deviceContext(dc)
				{

				}

			private:
				RenderTarget* m_renderTarget;
				RenderParameters m_presentParams;

				FPSCounter m_fpsCounter;
				DeviceContext* m_deviceContext;

			};

			/**
			 *  Represents a window with rendered graphics displayed in its client area.
			 */
			class APAPI RenderWindow : public RenderView
			{
			private:
				RenderWindowHandler* m_evtHandler;
				bool m_isExiting;
			public:

				virtual void Exit()
				{
					m_isExiting = true;
				}

				virtual void Run() = 0;

				virtual ~RenderWindow();

				virtual Size getClientSize() = 0;
				virtual String getTitle() = 0;
				virtual void setTitle(const String& name) = 0;

				void setEventHandler(RenderWindowHandler* handler)
				{
					m_evtHandler = handler;
				}

				bool getIsExiting() const { return m_isExiting; }
				/** Represents if the window is activated.
				*/
				virtual bool getIsActive() const = 0;

				bool getVisisble() const { return m_visisble; }
				virtual void SetVisible(bool v) = 0;

			protected:

				RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: RenderView(dc, rd, pm, rt), m_evtHandler(0), m_isExiting(false), m_visisble(true)
				{

				}
				RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm)
					: RenderView(dc, rd, pm), m_evtHandler(0), m_isExiting(false), m_visisble(true)
				{

				}
				
				void OnFrameStart();
				void OnFrameEnd();

				void OnInitialize();
				void OnFinalize();
				void OnLoad();
				void OnUnload();
				void OnUpdate(const GameTime* const time);
				void OnDraw(const GameTime* const time);

				bool m_visisble;
			};

		}
	}
}

#endif