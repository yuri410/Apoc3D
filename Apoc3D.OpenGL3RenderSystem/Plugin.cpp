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

#include "Plugin.h"

#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"
#include "GL3GraphicsAPIFactory.h"

#include "GL/wgl.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3RSPlugin::GL3RSPlugin()
				: m_factory()
			{
				m_glInitialized = (gl3wInit() == GL3W_OK && wglInit());
			}

			bool GL3RSPlugin::Load()
			{
				if (!m_glInitialized)
					return false;

				return GraphicsAPIManager::getSingleton().RegisterGraphicsAPI(&m_factory);
			}
			bool GL3RSPlugin::Unload()
			{
				if (!m_glInitialized)
					return false;
				return GraphicsAPIManager::getSingleton().UnregisterGraphicsAPI(&m_factory);
			}

		}
	}
}