#ifndef UIRESOURCE_H
#define UIRESOURCE_H

#include "APDCommon.h"
#include "Collections/FastMap.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace APDesigner
{
	class UIResources
	{
	private:
		static FastMap<String, Texture*>* m_maps;
	public:
		static void Initialize(RenderDevice* device);
		static void Finalize();

		static Texture* GetTexture(const String& name) { return m_maps->operator[](name); }
	};
}
#endif