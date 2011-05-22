
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

#include "RenderWindow.h"

#include "RenderWindowHandler.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			void FPSCounter::Step()
			{

			}

			void RenderView::Present()
			{
				m_fpsCounter.Step(); 
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

			void RenderWindow::OnDraw()
			{
				if (m_evtHandler)
					m_evtHandler->Draw();
				Present(); 
			}
			void RenderWindow::OnUpdate(const GameTime* const time)		
			{
				if (m_evtHandler)
					m_evtHandler->Update(time);
			}
		}
	}
}