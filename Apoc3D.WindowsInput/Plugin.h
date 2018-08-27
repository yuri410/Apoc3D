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

#ifndef WININPUTPLUGIN_H
#define WININPUTPLUGIN_H

#include "apoc3d/Core/Plugin.h"

using namespace Apoc3D::Core;

#ifdef APOC3D_DYNLIB
extern "C" PLUGINAPI Plugin* Apoc3DGetPlugin();
#endif

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class WinInputPlugin : public Plugin
			{
			public:
				WinInputPlugin();
				~WinInputPlugin();

				virtual void Load();
				virtual void Unload();

				virtual String GetName() { return L"Windows Input System"; }

			private:
				class Win32InputFactory* m_factory;
				char m_localData[256];
			};
		}
	}
}

#endif