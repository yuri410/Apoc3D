/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#include "GL3DeviceContext.h"

#include "GL1RenderDevice.h"
#include "GL1RenderWindow.h"
#include "apoc3d/Exception.h"

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3DeviceContext::GL3DeviceContext()
				: DeviceContext(true), m_window(0)
			{


			}
			GL3DeviceContext::~GL3DeviceContext()
			{

			}


			void GL3DeviceContext::NotifyWindowClosed(GL3RenderWindow* wnd)
			{
				if (m_window != wnd)
					m_window = NULL;
			}

			RenderView* GL3DeviceContext::create(const RenderParameters &pm)
			{
				if (m_window)
				{
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Cannot create more render view when a render window has been created.");
				}

				if (!pm.IsFullForm)
				{
					
				}
				else
				{
					if (!m_window)
					{
						m_window = new GL3RenderWindow(0, this, pm);

						return m_window;
					}
				}
				// keep the compiler happy
				return 0;
			}

			RenderDevice* GL3DeviceContext::getRenderDevice()
			{
				if (m_window)
					return m_window->getRenderDevice();
				return 0;
			}
		}
	}
}