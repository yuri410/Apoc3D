#pragma once

#include "APDCommon.h"

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
