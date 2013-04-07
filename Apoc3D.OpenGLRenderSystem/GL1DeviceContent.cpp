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

#include "GL1DeviceContent.h"

#include "GL1RenderDevice.h"
#include "GL1RenderWindow.h"
#include "Apoc3DException.h"

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GLRenderSystem
		{
			GL1DeviceContent::GL1DeviceContent()
				: DeviceContent(true), m_window(0)
			{


			}
			GL1DeviceContent::~GL1DeviceContent()
			{

			}


			void GL1DeviceContent::NotifyWindowClosed(GL1RenderWindow* wnd)
			{
				if (m_window != wnd)
					m_window = NULL;
			}

			RenderView* GL1DeviceContent::create(const RenderParameters &pm)
			{
				if (m_window)
				{
					throw Apoc3DException::createException(EX_InvalidOperation, L"Cannot create more render view when a render window has been created.");
				}

				if (!pm.IsFullForm)
				{
					
				}
				else
				{
					if (!m_window)
					{
						m_window = new GL1RenderWindow(0, this, pm);

						return m_window;
					}
				}
				// keep the compiler happy
				return 0;
			}

			RenderDevice* GL1DeviceContent::getRenderDevice()
			{
				if (m_window)
					return m_window->getRenderDevice();
				return 0;
			}
		}
	}
}