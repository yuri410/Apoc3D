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

#ifndef APOC3D_MODELPRESET_H
#define APOC3D_MODELPRESET_H

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Graphics/RenderSystem/VertexElement.h"
#include "apoc3d/Math/Color.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace ProjectSystem
	{
		struct APAPI ModelPresetMaterial
		{
			String NamePattern;
			IO::MaterialData* Data = nullptr;

			ModelPresetMaterial() { }
			~ModelPresetMaterial();

			ModelPresetMaterial(ModelPresetMaterial&&);
			ModelPresetMaterial& operator=(ModelPresetMaterial&&);

			ModelPresetMaterial(const ModelPresetMaterial&) = delete;
			ModelPresetMaterial& operator=(const ModelPresetMaterial&) = delete;

		};

		class APAPI ModelPreset
		{
		public:
			bool UseTextureNameConversion = false;
			bool CopyEntireMaterial = false;

			bool CollapseMeshs = false;
			bool UseVertexFormatConversion = false;
			
			List<VertexElement> ConversionVertexElements;

			List<ModelPresetMaterial> BaseData;
			
			void Parse(Configuration& conf);

			IO::MaterialData* SearchMaterial(const String& name) const;

			ModelPreset() { }
			ModelPreset(const ModelPreset&) = delete;
			ModelPreset& operator=(const ModelPreset&) = delete;
		private:

		};
	}
}

#endif