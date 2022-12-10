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

#include "RenderWindow.h"

#include "RenderWindowHandler.h"
#include "apoc3d/Core/AppTime.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			void FPSCounter::Step(float dt)
			{
				if (m_frameTimes.getCount() == 0)
					m_frameTimes.Enqueue(0);

				m_currentWindowTimePos += dt;
				m_frameTimes.Enqueue(m_currentWindowTimePos);


				float begin = m_frameTimes.Head();
				float end = m_frameTimes.Tail();

				float timeLen = end - begin;

				if (timeLen < 0)
				{
					m_frameTimes.Clear();
					m_currentWindowTimePos = 0;
					return;
				}

				m_fps = static_cast<float>(m_frameTimes.getCount()) / (timeLen);

				if (timeLen > 1)
				{
					float stamp = end - 1;
					while (m_frameTimes.getCount() > 0 && m_frameTimes.Head() < stamp)
					{
						m_frameTimes.Dequeue();
					}

					// rebase to zero
					if (m_frameTimes.Head() > 0)
					{
						float base = m_frameTimes.Head();
						for (int32 i = 0; i < m_frameTimes.getCount(); i++)
							m_frameTimes[i] -= base;

						m_currentWindowTimePos -= base;
					}
				}
			}

			void FPSCounter::Step(const AppTime* time)
			{
				Step(time->ElapsedRealTime); 
			}

			void RenderView::UpdateFpsCounter(const AppTime* time)
			{
				m_fpsCounter.Step(time); 
			}

			//////////////////////////////////////////////////////////////////////////

			RenderWindow::~RenderWindow()
			{
				delete m_evtHandler;
			}

			void RenderWindow::SetupTimeStepMode(TimeStepMode type, float refFrameTime)
			{
				m_referenceElapsedTime = refFrameTime;
				m_timeStepMode = type;
			}

			TimeStepMode RenderWindow::GetCurrentTimeStepMode() 
			{
				return m_timeStepMode; 
			}

			void RenderWindow::Base_OnInitialize()
			{
				if (m_evtHandler)
					m_evtHandler->Initialize();
			}

			void RenderWindow::Base_OnFinalize()
			{
				if (m_evtHandler)
					m_evtHandler->Finalize();
			}

			void RenderWindow::Base_OnLoad()
			{
				if (m_evtHandler)
					m_evtHandler->Load();
			}

			void RenderWindow::Base_OnUnload()
			{
				if (m_evtHandler)
					m_evtHandler->Unload();
			}

			void RenderWindow::Base_OnDraw(const AppTime* time)
			{
				if (m_evtHandler)
					m_evtHandler->Draw(time); 
				UpdateFpsCounter(time);
			}
			void RenderWindow::Base_OnUpdate(const AppTime* time)
			{
				if (m_evtHandler)
					m_evtHandler->Update(time);
			}
			void RenderWindow::Base_OnUpdateConstrainedVarTimeStep(const AppTime* time)
			{
				if (m_evtHandler)
					m_evtHandler->UpdateConstrainedVarTimeStep(time);
			}
			void RenderWindow::Base_OnFrameStart()
			{
				if (m_evtHandler)
					m_evtHandler->OnFrameStart();
			}
			void RenderWindow::Base_OnFrameEnd()
			{
				if (m_evtHandler)
					m_evtHandler->OnFrameEnd();
				Present();
			}

			void RenderWindow::Base_DropFiles(const List<String>* files)
			{
				if (m_evtHandler)
					m_evtHandler->OnDropFiles(files);
			}
		}
	}
}