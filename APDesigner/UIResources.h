#pragma once

#include "APDCommon.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace APDesigner
{
	class UIResources
	{
	private:
		static HashMap<String, Texture*>* m_maps;
	public:
		static void Initialize(RenderDevice* device);
		static void Finalize();

		static Texture* GetTexture(const String& name) { return m_maps->operator[](name); }
	};
}
