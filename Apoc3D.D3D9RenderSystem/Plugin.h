#pragma once

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

#ifndef D3DPLUGIN_H
#define D3DPLUGIN_H

#include "apoc3d/Core/Plugin.h"

using namespace Apoc3D::Core;

#ifdef APOC3D_DYNLIB
extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin();
#endif

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RSPlugin : public Plugin
			{
			public:
				D3D9RSPlugin();
				~D3D9RSPlugin();

				virtual bool Load() override;
				virtual bool Unload() override;

				virtual String GetName() override { return L"Direct3D9 Render System"; }

			private:
				class D3D9GraphicsAPIFactory* m_factory;
				char m_localData[256];

			};
		}
	}
}

#endif