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

#include "HardwareBuffer.h"

using namespace Apoc3D;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{

			
			void* HardwareBuffer::Lock(LockMode mode)
			{
				return Lock(0, m_size, mode);
			}
			void* HardwareBuffer::Lock(int offset, int size, LockMode mode)
			{
				if (!m_isLocked)
				{
					void* ptr = lock(offset, size, mode);
					m_isLocked = true;
					return ptr;
				}
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Buffer already Locked.");
			}

			void HardwareBuffer::Unlock()
			{
				if (m_isLocked)
				{
					unlock();
					m_isLocked = false;
				}
				else
				{
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Buffer is not locked");
				}
			}

		}
	}
}