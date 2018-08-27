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

#ifndef MESHBUILD_H
#define MESHBUILD_H

#include "APBCommon.h"

namespace APBuild
{
	namespace MeshBuild
	{
		void Build(const String& hierarchyPath, const ConfigurationSection* sect);

		void PostProcess(ModelData* data, ProjectResModel& config);
		void ConvertVertexData(ModelData* data, const ProjectResModel& config);
		void CollapseMeshs(ModelData* data, const ProjectResModel& config);
		void ExecuteMaterialConversion(ModelData* data, const ModelPreset& preset, const ProjectResModel& config);
	};

}

#endif