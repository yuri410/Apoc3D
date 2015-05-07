#pragma once
#ifndef APOC3D_MODELPRESET_H
#define APOC3D_MODELPRESET_H

#include "apoc3d/Common.h"
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
		struct ModelPresetMaterial
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