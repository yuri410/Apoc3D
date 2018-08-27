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
#include "apoc3d/Core/GameTime.h"

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

			void FPSCounter::Step(const GameTime* time)
			{
				Step(time->getElapsedRealTime()); 
			}

			void RenderView::Present(const GameTime* time)
			{
				m_fpsCounter.Step(time); 
			}

			RenderWindow::~RenderWindow()
			{
				delete m_evtHandler;
			}
			void RenderWindow::OnInitialize()
			{
				if (m_evtHandler)
					m_evtHandler->Initialize();
			}
			void RenderWindow::OnFinalize()
			{
				if (m_evtHandler)
					m_evtHandler->Finalize();
			}


			void RenderWindow::OnLoad()
			{
				if (m_evtHandler)
					m_evtHandler->Load();
			}
			void RenderWindow::OnUnload()
			{
				if (m_evtHandler)
					m_evtHandler->Unload();
			}

			void RenderWindow::OnDraw(const GameTime* time)
			{
				if (m_evtHandler)
					m_evtHandler->Draw(time);
				Present(time); 
			}
			void RenderWindow::OnUpdate(const GameTime* time)		
			{
				if (m_evtHandler)
					m_evtHandler->Update(time);
			}
			void RenderWindow::OnUpdateConstrainedVarTimeStep(const GameTime* time)
			{
				if (m_evtHandler)
					m_evtHandler->UpdateConstrainedVarTimeStep(time);
			}
			void RenderWindow::OnFrameStart()
			{
				if (m_evtHandler)
					m_evtHandler->OnFrameStart();
			}
			void RenderWindow::OnFrameEnd()
			{
				if (m_evtHandler)
					m_evtHandler->OnFrameEnd();
			}
		}
	}
}