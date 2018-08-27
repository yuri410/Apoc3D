
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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
#include "NRSObjects.h"

#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::NullRenderSystem;

#ifdef APOC3D_DYNLIB
static NRSPlugin plugin;

extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin()
{
	return &plugin;
}
#endif

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSPlugin::NRSPlugin()
			{
				static_assert(sizeof(NRSGraphicsAPIFactory) <= sizeof(m_localData), "");

				m_factory = new (m_localData)NRSGraphicsAPIFactory();
			}
			NRSPlugin::~NRSPlugin()
			{
				m_factory->~NRSGraphicsAPIFactory();
				m_factory = nullptr;
			}

			void NRSPlugin::Load()
			{
				GraphicsAPIManager::getSingleton().RegisterGraphicsAPI(m_factory);
			}
			void NRSPlugin::Unload()
			{
				GraphicsAPIManager::getSingleton().UnregisterGraphicsAPI(m_factory);
			}

		}
	}
}