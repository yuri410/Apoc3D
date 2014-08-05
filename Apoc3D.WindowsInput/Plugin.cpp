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
				: m_factory(new Win32InputFactory())
			{

			}
			WinInputPlugin::~WinInputPlugin()
			{
				delete m_factory;
				m_factory = nullptr;
			}

			void WinInputPlugin::Load()
			{
				InputAPIManager::getSingleton().RegisterInputAPI(m_factory);
			}
			void WinInputPlugin::Unload()
			{
				InputAPIManager::getSingleton().UnregisterInputAPI(m_factory);
			}

		}
	}
}