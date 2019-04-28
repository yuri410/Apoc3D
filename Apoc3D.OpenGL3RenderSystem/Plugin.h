#pragma once

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

#ifndef GL3PLUGIN_H
#define GL3PLUGIN_H

#include "GL3Common.h"
#include "apoc3d/Core/Plugin.h"
#include "GL3GraphicsAPIFactory.h"

using namespace Apoc3D::Core;

#ifdef APOC3D_DYNLIB
extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin();
#endif

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3RSPlugin : public Plugin
			{
			private:
				GL3GraphicsAPIFactory m_factory;
			public:
				GL3RSPlugin();
				virtual bool Load() override;
				virtual bool Unload() override;

				virtual String GetName() override { return L"OpenGL 3.1 Render System"; }

			private:
				bool m_glInitialized = false;
			};
		}
	}
}

#endif