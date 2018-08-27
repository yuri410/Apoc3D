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