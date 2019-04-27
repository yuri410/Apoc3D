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

			bool D3D9RSPlugin::Load()
			{
				return GraphicsAPIManager::getSingleton().RegisterGraphicsAPI(m_factory);
			}
			bool D3D9RSPlugin::Unload()
			{
				return GraphicsAPIManager::getSingleton().UnregisterGraphicsAPI(m_factory);
			}

		}
	}
}