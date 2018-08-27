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

#ifndef TEXTUREBUILD_H
#define TEXTUREBUILD_H

#include "../APBCommon.h"
#include "../BuildConfig.h"

using namespace Apoc3D::Config;

namespace APBuild
{
	namespace TextureBuild
	{
		void Build(const String& hierarchyPath, const ConfigurationSection* sect);
	};
}
#endif