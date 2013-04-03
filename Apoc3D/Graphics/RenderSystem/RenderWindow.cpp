
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
			void FPSCounter::Step(const GameTime* const time)
			{
				m_frameTimes.push_back(time->getTotalRealTime());

				if (m_frameTimes.size()>1)
				{
					float begin = *m_frameTimes.begin();

					list<float>::iterator iter = m_frameTimes.end();
					iter--;
					float end = *iter;

					float timeLen = end - begin;

					if (timeLen<0)
					{
						m_frameTimes.clear();
						return;
					}

					m_fps = static_cast<float>(m_frameTimes.size())/(timeLen);

					if (timeLen>1)
					{
						float stamp = end - 1;
						while (*m_frameTimes.begin()<stamp && !m_frameTimes.empty())
						{
							m_frameTimes.erase(m_frameTimes.begin());
						}
					}
				}
			}

			void RenderView::Present(const GameTime* const time)
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

			void RenderWindow::OnDraw(const GameTime* const time)
			{
				if (m_evtHandler)
					m_evtHandler->Draw(time);
				Present(time); 
			}
			void RenderWindow::OnUpdate(const GameTime* const time)		
			{
				if (m_evtHandler)
					m_evtHandler->Update(time);
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