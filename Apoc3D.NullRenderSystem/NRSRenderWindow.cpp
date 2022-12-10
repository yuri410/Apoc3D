
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

#include "NRSRenderWindow.h"
#include "NRSRenderDevice.h"
#include "NRSDeviceContext.h"
#include "NRSObjects.h"
#include "NRSPlatform.h"

#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Platform/Thread.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSRenderView::NRSRenderView(NRSRenderDevice* device, NRSDeviceContext* dc, const RenderParameters& pm)
				: RenderView(dc, device, pm), m_device(device), m_controlHandle(pm.TargetHandle)
			{
			}
			NRSRenderView::~NRSRenderView()
			{
			}

			void NRSRenderView::ChangeRenderParameters(const RenderParameters& params)
			{
			}
			void NRSRenderView::Present()
			{
			}

			/************************************************************************/
			/*   NRSRenderWindow                                                    */
			/************************************************************************/


			NRSRenderWindow::NRSRenderWindow(NRSRenderDevice* device, NRSDeviceContext* dc, const RenderParameters& pm)
				: RenderWindow(dc, device, pm), m_dc(dc)
			{
				m_gameClock = new GameClock();

				m_gameWindow = new GameWindow(L"");
			}
			NRSRenderWindow::~NRSRenderWindow()
			{
				// clean up traces.
				m_dc->NotifyWindowClosed(this);

				delete m_gameWindow;

				delete m_gameClock;
			}

			void NRSRenderWindow::ChangeRenderParameters(const RenderParameters& params)
			{
				RenderWindow::ChangeRenderParameters(params);
			}

			void NRSRenderWindow::Present()
			{

			}

			void NRSRenderWindow::Exit()
			{
				RenderWindow::Exit();
				
				m_gameWindow->Close();
			}

			void NRSRenderWindow::Run()
			{
				// This Run method is called from outside client code to create a render window
				// for the first time and right after when the constructor is called. No other methods are
				// needed. 
				
				// Creates almost every thing
				NRS_Create(getRenderParams());

				NRS_MainLoop();
				// Releases almost every thing
				NRS_Release();
			}

			String NRSRenderWindow::getTitle() { return m_gameWindow->getWindowTitle(); }
			void NRSRenderWindow::setTitle(const String& name) { m_gameWindow->setWindowTitle(name); }

			bool NRSRenderWindow::getDragAcceptFiles() { return m_gameWindow->getDragAcceptFiles(); }
			void NRSRenderWindow::setDragAcceptFiles(bool value) { m_gameWindow->setDragAcceptFiles(value); }

			Size NRSRenderWindow::getClientSize() { return m_gameWindow->getCurrentSize(); }
			
			void NRSRenderWindow::SetVisible(bool v)
			{
				m_gameWindow->SetVisible(v);
				m_visisble = v;
			}

			void NRSRenderWindow::Minimize()
			{
				if (m_visisble)
					m_gameWindow->Minimize();
			}
			void NRSRenderWindow::Restore()
			{
				if (m_visisble)
					m_gameWindow->Restore();
			}
			void NRSRenderWindow::Maximize()
			{
				if (m_visisble)
					m_gameWindow->Maximize();
			}

			//////////////////////////////////////////////////////////////////////////


			void NRSRenderWindow::NRS_Create(const RenderParameters& params)
			{
				m_gameWindow->Load(params.BackBufferWidth, params.BackBufferHeight, params.IsFixedWindow);

				m_renderDevice = new NRSRenderDevice();

				ApocLog(LOG_Graphics, L"[NRS]Creating render window.", LOGLVL_Infomation);

				NRS_Initialize();
				NRS_LoadContent();

				ApocLog(LOG_Graphics, L"[NRS]Render window created.", LOGLVL_Infomation);
			}

			void NRSRenderWindow::NRS_Release()
			{
				// Unload() and Finalize() will be called here
				NRS_UnloadContent();
				NRS_Finalize();
				delete m_renderDevice;
			}


			void NRSRenderWindow::NRS_Initialize()
			{
				m_hardwareName = L"Null Device";

				{
					// The window will be only initialized once, even in some cases, like device lost
					// when this is called again.
					NRSRenderDevice* device = static_cast<NRSRenderDevice*>(m_renderDevice);

					assert(!device->isInitialized());

					device->Initialize();
					Base_OnInitialize(); // will make the event handler interface to process the event
				}
				
			}
			void NRSRenderWindow::NRS_Finalize()
			{
				Base_OnFinalize();
			}

			void NRSRenderWindow::NRS_LoadContent() { Base_OnLoad(); }
			void NRSRenderWindow::NRS_UnloadContent() { Base_OnUnload(); }

			void NRSRenderWindow::NRS_MainLoop()
			{
				while (!m_gameWindow->isClosed())
				{
					NRS_Tick();
					ApocSleep(1);
				}
			}

			void NRSRenderWindow::NRS_Tick()
			{
				if (m_isExiting)
					return;

				if (!m_active && m_inactiveSleepTime > 0)
					Platform::ApocSleep(m_inactiveSleepTime);

				m_gameClock->Step();

				float dt = (float)m_gameClock->getElapsedTime();
				float totalRealTime = (float)m_gameClock->getCurrentTime();

				if (dt < 0)
					dt = 0;

				float fps = getFPS();

				int32 maxFrameSkip = m_maxSkipFrameCount;
#if _DEBUG
				if (maxFrameSkip > 5)
					maxFrameSkip = 5;
#endif

				bool renderingSlow = m_accumulatedDt_fixedStep > m_referenceElapsedTime * 1.2f;

				if (renderingSlow)
					m_slowRenderFrameHits++;
				else
					m_slowRenderFrameHits--;

				renderingSlow = m_slowRenderFrameHits > 5;

				if (m_timeStepMode == TimeStepMode::FixedStep)
				{
					m_accumulatedDt_fixedStep += dt;

					// keep up update calls:
					// update until it's time to draw the next frame

					int32 numUpdatesNeeded = (int32)(m_accumulatedDt_fixedStep / m_referenceElapsedTime);
					if (numUpdatesNeeded > maxFrameSkip)
						numUpdatesNeeded = maxFrameSkip;

					float iterationFullDt = m_accumulatedDt_fixedStep;
					m_accumulatedDt_fixedStep = fmod(fabs(m_accumulatedDt_fixedStep), m_referenceElapsedTime);
					iterationFullDt -= m_accumulatedDt_fixedStep;

					for (int32 i = 0; i < numUpdatesNeeded; i++)
					{
						AppTime gt(m_referenceElapsedTime, iterationFullDt / numUpdatesNeeded, m_accumulatedDt_fixedStep, numUpdatesNeeded, fps, renderingSlow);
						NRS_Update(&gt);
					}

					if (dt > 0)
					{
						numUpdatesNeeded = (int32)(ceil(dt / m_referenceElapsedTime));
						if (numUpdatesNeeded > maxFrameSkip)
							numUpdatesNeeded = maxFrameSkip;
						if (numUpdatesNeeded < 1)
							numUpdatesNeeded = 1;

						float dtPerIteration = dt / numUpdatesNeeded;
						if (dtPerIteration > m_referenceElapsedTime)
							dtPerIteration = m_referenceElapsedTime;

						for (int32 i = 0; i < numUpdatesNeeded; i++)
						{
							AppTime gt(dtPerIteration, dt / numUpdatesNeeded, m_accumulatedDt_fixedStep, numUpdatesNeeded, fps, renderingSlow);
							NRS_UpdateConstrainedVarTimeStep(&gt);
						}
					}

					AppTime gt(iterationFullDt, dt, m_accumulatedDt_fixedStep, 1, fps, renderingSlow);
					NRS_DrawFrame(&gt);
				}
				else if (m_timeStepMode == TimeStepMode::Constrained)
				{
					if (dt > 0)
					{
						int32 numUpdatesNeeded = (int32)(ceil(dt / m_referenceElapsedTime));
						if (numUpdatesNeeded > maxFrameSkip)
							numUpdatesNeeded = maxFrameSkip;
						if (numUpdatesNeeded < 1)
							numUpdatesNeeded = 1;

						float dtPerIteration = dt / numUpdatesNeeded;
						if (dtPerIteration > m_referenceElapsedTime)
							dtPerIteration = m_referenceElapsedTime;

						for (int32 i = 0; i < numUpdatesNeeded; i++)
						{
							AppTime gt(dtPerIteration, dt / numUpdatesNeeded, 0, numUpdatesNeeded, fps, renderingSlow);
							NRS_Update(&gt);
						}
					}

					AppTime gt(dt, dt, 0, 1, fps, renderingSlow);
					NRS_DrawFrame(&gt);
				}
				else
				{
					AppTime gt(dt, fps);

					NRS_Update(&gt);
					NRS_DrawFrame(&gt);
				}
			}

			void NRSRenderWindow::NRS_DrawFrame(const AppTime* time)
			{
				Base_OnFrameStart();
				NRS_Render(time);
				Base_OnFrameEnd();
			}

			void NRSRenderWindow::NRS_Render(const AppTime* time) { Base_OnDraw(time); }
			void NRSRenderWindow::NRS_Update(const AppTime* time) { Base_OnUpdate(time); }
			void NRSRenderWindow::NRS_UpdateConstrainedVarTimeStep(const AppTime* time) { Base_OnUpdateConstrainedVarTimeStep(time); }

		}
	}
}