#pragma once
#ifndef APOC3D_RENDERWINDOW_H
#define APOC3D_RENDERWINDOW_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "DeviceContext.h"

#include "apoc3d/Collections/Queue.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** Implements a simple FPS counter */
			class APAPI FPSCounter
			{
			public:
				FPSCounter() { }

				void Step(const AppTime* time);
				void Step(float dt);

				float getFPS() const { return m_fps; }

			private:
				Queue<float> m_frameTimes;
				float m_fps = 0;
				float m_currentWindowTimePos = 0;
			};

			/** Represents a view of rendered graphics. */
			class APAPI RenderView
			{
				RTTI_BASE;
			public:
				virtual ~RenderView(){}

				virtual void Present() = 0;
				/**
				 *  Change the device settings. 
				 *  This may cause the device to be reseted/recreated depending on how big the change is.
				 */
				virtual void ChangeRenderParameters(const RenderParameters& params) { m_presentParams = params; }

				void* UserData = nullptr;

				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				const RenderParameters& getRenderParams() const { return m_presentParams; }
				DeviceContext* getDeviceContext() const { return m_deviceContext; }

				float getFPS() const { return m_fpsCounter.getFPS(); }

			protected:
				RenderDevice* m_renderDevice;

				RenderView(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: m_presentParams(pm), m_renderDevice(rd), m_renderTarget(rt), m_deviceContext(dc)
				{
				}

				RenderView(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm)
					: m_presentParams(pm), m_renderDevice(rd), m_deviceContext(dc)
				{

				}

				void UpdateFpsCounter(const AppTime* time);
			private:
				RenderTarget* m_renderTarget = nullptr;
				RenderParameters m_presentParams;

				FPSCounter m_fpsCounter;
				DeviceContext* m_deviceContext;

			};

			enum struct TimeStepMode
			{
				Raw,
				Constrained,
				FixedStep
			};

			/** Represents a window with rendered graphics displayed in its client area. */
			class APAPI RenderWindow : public RenderView
			{
				RTTI_DERIVED(RenderWindow, RenderView);
			public:
				virtual ~RenderWindow();

				/** Quit the rendering app */
				virtual void Exit() { m_isExiting = true; }
				/**
				 *  Enter the main loop. 
				 *  This also include the initialization and finalization in the event handler.
				 */
				virtual void Run() = 0;

				virtual void Minimize() = 0;
				virtual void Restore() = 0;
				virtual void Maximize() = 0;

				virtual Size getClientSize() = 0;
				virtual String getTitle() = 0;
				virtual void setTitle(const String& name) = 0;

				virtual void SetVisible(bool v) = 0;

				/** Represents if the window is activated. */
				virtual bool getIsActive() const = 0;

				void SetupTimeStepMode(TimeStepMode type, float refFrameTime);
				TimeStepMode GetCurrentTimeStepMode();

				void setInactiveSleepTime(int32 ms) { m_inactiveSleepTime = ms; }
				int32 getInactiveSleepTime() const { return m_inactiveSleepTime; }

				bool getVisisble() const { return m_visisble; }
				bool getIsExiting() const { return m_isExiting; }

				void setEventHandler(RenderWindowHandler* handler) { m_evtHandler = handler; }

			protected:

				RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm, RenderTarget* rt)
					: RenderView(dc, rd, pm, rt) { }

				RenderWindow(DeviceContext* dc, RenderDevice* rd, const RenderParameters &pm)
					: RenderView(dc, rd, pm) { }
				
				void OnInitialize();
				void OnFinalize();
				void OnLoad();
				void OnUnload();
				void OnUpdate(const AppTime* time);
				void OnUpdateConstrainedVarTimeStep(const AppTime* time);

				void OnDraw(const AppTime* time);
				void OnFrameStart();
				void OnFrameEnd();

				RenderWindowHandler* m_evtHandler = nullptr;

				bool m_isExiting = false;
				bool m_visisble = true;

				int32 m_inactiveSleepTime = 20;

				TimeStepMode m_timeStepMode = TimeStepMode::FixedStep;

				// fixed time step states
				float m_accumulatedDt_fixedStep = 0;

				float m_referenceElapsedTime = 1.0f / 60.0f;
			};

		}
	}
}

#endif