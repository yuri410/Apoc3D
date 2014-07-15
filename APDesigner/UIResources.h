#pragma once

#include "APDCommon.h"

namespace APDesigner
{
	namespace UIResources
	{
		void Initialize(RenderDevice* device);
		void Finalize();

		Texture* GetTexture(const String& name);
	};
}
