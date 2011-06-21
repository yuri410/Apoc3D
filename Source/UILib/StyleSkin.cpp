#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"



namespace Apoc3D
{
	namespace UI
	{
		StyleSkin::StyleSkin()
		{
			// ButtonTexture

			BtnSrcRect[0] = Apoc3D::Math::Rectangle(0, 0, ButtonTexture->getWidth() - 1, ButtonTexture->getHeight());
			BtnSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, 1, ButtonTexture->getHeight());
			BtnSrcRect[2] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, -ButtonTexture->getWidth() + 1, ButtonTexture->getHeight());

			BtnHozPadding = 5;
			BtnVertPadding = 20;
		}
	}
}