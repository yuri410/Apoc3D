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

#include "Plugin.h"
#include "Win32InputFactory.h"

#include "apoc3d/Input/InputAPI.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::Input::Win32;


#ifdef APOC3D_DYNLIB
static WinInputPlugin plugin;

extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin()
{
	return &plugin;
}
#endif

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			WinInputPlugin::WinInputPlugin()
			{
				static_assert(sizeof(Win32InputFactory) <= sizeof(m_localData), "");

				m_factory = new (m_localData)Win32InputFactory();
			}
			WinInputPlugin::~WinInputPlugin()
			{
				m_factory->~Win32InputFactory();
				m_factory = nullptr;
			}

			bool WinInputPlugin::Load()
			{
				return InputAPIManager::getSingleton().RegisterInputAPI(m_factory);
			}
			bool WinInputPlugin::Unload()
			{
				return InputAPIManager::getSingleton().UnregisterInputAPI(m_factory);
			}

		}
	}
}