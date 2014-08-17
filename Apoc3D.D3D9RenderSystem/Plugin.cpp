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

#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"

#include "D3D9GraphicsAPIFactory.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::D3D9RenderSystem;

#ifdef APOC3D_DYNLIB
static D3D9RSPlugin plugin;

extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin()
{
	return &plugin;
}
#endif

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RSPlugin::D3D9RSPlugin()
			{
				static_assert(sizeof(D3D9GraphicsAPIFactory) <= sizeof(m_localData), "");

				m_factory = new (m_localData)D3D9GraphicsAPIFactory();
			}
			D3D9RSPlugin::~D3D9RSPlugin()
			{
				m_factory->~D3D9GraphicsAPIFactory();
				m_factory = nullptr;
			}

			void D3D9RSPlugin::Load()
			{
				GraphicsAPIManager::getSingleton().RegisterGraphicsAPI(m_factory);
			}
			void D3D9RSPlugin::Unload()
			{
				GraphicsAPIManager::getSingleton().UnregisterGraphicsAPI(m_factory);
			}

		}
	}
}