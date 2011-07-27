
#ifndef CLASSICSTYLESKIN_H
#define CLASSICSTYLESKIN_H

#include "UICommon.h"
#include "Math/Rectangle.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI StyleSkin
		{
		public:
			Texture* ButtonTexture;
			Apoc3D::Math::Rectangle BtnSrcRect[3];
			uint BtnDimColor;
			uint BtnHighLightColor;
			uint BtnTextDimColor;
			uint BtnTextHighLightColor;

			int BtnVertPadding;
			int BtnHozPadding;



			Texture* WhitePixelTexture;
			Apoc3D::Math::Rectangle BtnRowSrcRect[2];
			Apoc3D::Math::Rectangle BtnRowSeparator;
			String ControlFontName;

			Texture* FormBorderTexture[9];


			Texture* FormCloseButton;
			Texture* FormMinimizeButton;
			Texture* FormMaximizeButton;
			Texture* FormRestoreButton;

		public:
			// create default skin
			StyleSkin(RenderDevice* device, const FileLocateRule& rule);
			~StyleSkin();
		};
	}
}

#endif