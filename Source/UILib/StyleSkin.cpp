#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/TextureManager.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		StyleSkin::StyleSkin(RenderDevice* device, const FileLocateRule& rule)
		{
			FileLocation* fl = FileSystem::getSingleton().TryLocate(L"ctl_btn_default.tex", rule);
			ButtonTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			
			
			BtnSrcRect[0] = Apoc3D::Math::Rectangle(0, 0, ButtonTexture->getWidth() - 1, ButtonTexture->getHeight());
			BtnSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, 1, ButtonTexture->getHeight());
			BtnSrcRect[2] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, -ButtonTexture->getWidth() + 1, ButtonTexture->getHeight());

			BtnHozPadding = 5;
			BtnVertPadding = 20;




			BtnRowSrcRect[0] = Apoc3D::Math::Rectangle(0,0,ButtonTexture->getWidth(),ButtonTexture->getHeight());
			BtnRowSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth()-1,0,1,ButtonTexture->getHeight());
			BtnRowSeparator = Apoc3D::Math::Rectangle(0,0,2,ButtonTexture->getHeight());


			WhitePixelTexture = device->getObjectFactory()->CreateTexture(1,1,1, TU_StaticWriteOnly, FMT_A8R8G8B8);

			DataRectangle rect = WhitePixelTexture->Lock(0, LOCK_Discard);
			*(uint*)rect.getDataPointer() = PACK_COLOR(0xff,0xff,0xff,0xff);
			WhitePixelTexture->Unlock(0);
		}

		StyleSkin::~StyleSkin()
		{
			delete ButtonTexture;
			delete WhitePixelTexture;
		}
	}
}