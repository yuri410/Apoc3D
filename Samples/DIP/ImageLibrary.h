#pragma once

#include "DIPCommon.h"

namespace dip
{
	class ImageLibrary
	{
		SINGLETON_DECL(ImageLibrary);
	public:
		void Load(RenderDevice* device);
		void Free();

		Texture* m_white16;

		Texture* m_colorPhoto;
		Texture* m_original256;
		Texture* m_original512;
		Texture* m_original;

		Texture* m_originalGrad;
		Texture* m_originalAlt;
		Texture* m_originalAperture256;
		Texture* m_originalAlt256;
	private:
		ImageLibrary() { }
	};
}