#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"

#include "Graphics/TextureManager.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		StyleSkin::StyleSkin(RenderDevice* device)
		{
			FileLocation* fl = FileSystem::getSingleton().TryLocate(L"classicUI\ctl_btn_default.tex", FileLocateRule::Default);
			ButtonTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			
			
			BtnSrcRect[0] = Apoc3D::Math::Rectangle(0, 0, ButtonTexture->getWidth() - 1, ButtonTexture->getHeight());
			BtnSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, 1, ButtonTexture->getHeight());
			BtnSrcRect[2] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, -ButtonTexture->getWidth() + 1, ButtonTexture->getHeight());

			BtnHozPadding = 5;
			BtnVertPadding = 20;




			BtnRowSrcRect[0] = Apoc3D::Math::Rectangle(0,0,ButtonTexture->getWidth(),ButtonTexture->getHeight());
			BtnRowSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth()-1,0,1,ButtonTexture->getHeight());
			BtnRowSeparator = Apoc3D::Math::Rectangle(0,0,2,ButtonTexture->getHeight());




		}
	}
}