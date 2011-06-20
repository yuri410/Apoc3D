
#ifndef CLASSICSTYLESKIN_H
#define CLASSICSTYLESKIN_H

#include "UICommon.h"
#include "Math/Rectangle.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI StyleSkin
		{
		public:
			Texture* ButtonTexture;
			Rectangle BtnSrcRect[3];
			uint BtnDimColor;
			uint BtnHighLightColor;
			uint BtnTextDimColor;
			uint BtnTextHighLightColor;

			int BtnVertPadding;
			int BtnHozPadding;


			String ControlFontName;

		public:
			// create default skin
			StyleSkin();

		};
	}
}

#endif