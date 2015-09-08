
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