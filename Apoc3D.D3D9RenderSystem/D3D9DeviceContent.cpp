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

#include "D3D9DeviceContent.h"

#include "D3D9RenderDevice.h"
#include "D3D9RenderWindow.h"
#include "Apoc3DException.h"



namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9DeviceContent::D3D9DeviceContent()
				: DeviceContent(true)
			{

			}

			RenderView* D3D9DeviceContent::create(const RenderParameters &pm)
			{
				if (!pm.IsFullForm)
				{

				}
				else
				{
					if (!m_device)
					{
						m_window = new D3D9RenderWindow(0, pm);
						return m_window;
					}
					Apoc3DException::createException(EX_InvalidOperation, L"Only one render window can be created.");
				}
				
			}

			RenderDevice* D3D9DeviceContent::getRenderDevice()
			{
				return m_device;
			}
		}
	}
}