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
#include "Plugin.h"

#include "Input/InputAPI.h"
//#include "D3D9GraphicsAPIFactory.h"

using namespace Apoc3D::Input;
//using namespace Apoc3D::Graphics::D3D9RenderSystem;

static WinInputPlugin* plugin = new WinInputPlugin();

extern "C" PLUGIN Plugin* Apoc3DGetPlugin()
{
	return plugin;
}

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			void WinInputPlugin::Load()
			{
				//GraphicsAPIManager::getSingleton().RegisterGraphicsAPI(new D3D9GraphicsAPIFactory());
			}
			void WinInputPlugin::Unload()
			{
				//GraphicsAPIManager::getSingleton().UnregisterGraphicsAPI(new D3D9GraphicsAPIFactory());
			}

		}
	}
}