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
#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H


#include "Common.h"
#include "DeviceContent.h"
#include "Math\Point.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			//template class APAPI list<float>;

			/* Implements a simple FPS counter
			*/
			class APAPI FPSCounter
			{
			private:
				list<float> m_frameTimes;
				float m_fps;
			public:
				float getFPS() const { return m_fps; }
				void Step(const GameTime* const time);
			};

			/* Represents a view of rendered graphics.
			*/
			class APAPI RenderView
			{
			private:
				RenderTarget* m_renderTarget;
				RenderParameters m_presentParams;

				FPSCounter m_fpsCounter;


			protected:
				RenderDevice* m_renderDevice;

				RenderView(RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: m_presentParams(pm), m_renderDevice(rd), m_renderTarget(rt)
				{
				}
				RenderView(RenderDevice* rd, const RenderParameters &pm)
					: m_presentParams(pm), m_renderDevice(rd), m_renderTarget(0)
				{

				}

			public:
				void* UserData;

				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				const RenderParameters& getRenderParams() const { return m_presentParams; }
				virtual void ChangeRenderParameters(const RenderParameters& params) { m_presentParams = params; }

				virtual ~RenderView(){}

				
				float getFPS() const { return m_fpsCounter.getFPS(); }

				virtual void Present(const GameTime* const time);
				

			};

			class APAPI RenderWindow : public RenderView
			{
			private:
				RenderWindowHandler* m_evtHandler;
				bool m_isExiting;

			public:
				virtual Size getClientSize() = 0;
				virtual String getTitle() = 0;
				virtual void setTitle(const String& name) = 0;

				void setEventHandler(RenderWindowHandler* handler)
				{
					m_evtHandler = handler;
				}

				virtual void Exit()
				{
					m_isExiting = true;
				}

				virtual void Run() = 0;

				~RenderWindow();

			protected:
				RenderWindow(RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: RenderView(rd, pm, rt), m_evtHandler(0), m_isExiting(false)
				{

				}
				RenderWindow(RenderDevice* rd, const RenderParameters &pm)
					: RenderView(rd, pm), m_evtHandler(0), m_isExiting(false)
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
				
			};

		}
	}
}

#endif