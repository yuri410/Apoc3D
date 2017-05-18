#pragma once
#ifndef APOC3D_PLUGIN_H
#define APOC3D_PLUGIN_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "apoc3d/Common.h"

#ifndef APOC3D_DYNLIB
#	define PLUGINAPI
#else
#	ifdef APOC3D_PLUGIN_DYLIB_EXPORT
#		define PLUGINAPI __declspec( dllexport )
#	else
#		define PLUGINAPI __declspec( dllimport )
#	endif
#endif

namespace Apoc3D
{
	namespace Core
	{
		class APAPI Plugin
		{
		public:
			virtual ~Plugin() { }

			virtual void Load() = 0;
			virtual void Unload() = 0;

			virtual String GetName() = 0;
		};
	}
}

#endif