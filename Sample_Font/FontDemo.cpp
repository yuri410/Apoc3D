#include "FontDemo.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/CommandInterpreter.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/ModelManager.h"
#include "apoc3d/Graphics/Model.h"
#include "apoc3d/Graphics/Mesh.h"
#include "apoc3d/Scene/OctreeSceneManager.h"
#include "apoc3d/Scene/SceneRenderer.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Keyboard.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/PerlinNoise.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/Platform/Thread.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


namespace SampleFont
{
	const int32 MaxPressure = 10;
	const String mlText = 
		L"This font supports multiple language.\n"
		L"这种字体支持多国语言\n"
		L"このフォントは、複数の言語をサポートしています\n"
		L"этот шрифт поддерживает несколько языков";

	FontDemo::FontDemo(RenderWindow* wnd)
		: Game(wnd), m_currentPressure(5)
	{
	}

	FontDemo::~FontDemo()
	{		

	}

	void FontDemo::Initialize()
	{
		m_window->setTitle(L"Font Demo");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void FontDemo::Finalize()
	{
		Game::Finalize();
	}

	void FontDemo::Load()
	{
		Game::Load();

		{
			FileLocation* fl = FileSystem::getSingleton().Locate(L"uming14.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming14", fl);

			fl = FileSystem::getSingleton().Locate(L"uming22.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming22", fl);

			FontManager::getSingleton().ReportComplexFonts();
		}
		
		m_console->Minimize();

	}
	void FontDemo::Unload()
	{
		delete m_sprite;

		Game::Unload();
	}
	void FontDemo::Update(const GameTime* const time)
	{
		Game::Update(time);

		Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();

		if (kb->IsKeyDown(KEY_EQUALS))
		{
			m_currentPressure++;
			if (m_currentPressure>MaxPressure)
				m_currentPressure = MaxPressure;
		}
		if (kb->IsKeyDown(KEY_MINUS))
		{
			m_currentPressure--;
			if (m_currentPressure<1)
				m_currentPressure = 1;
		}
	}
	void FontDemo::Draw(const GameTime* const time)
	{
		m_device->BeginFrame();
		m_device->Clear(CLEAR_ColorBuffer, 0, 1, 0);

		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_AlphaBlended | Sprite::SPR_RestoreState));
		
		Font* uming14 = FontManager::getSingleton().getFont(L"uming14");
		//Font* uming22 = FontManager::getSingleton().getFont(L"uming22");

		uming14->DrawString(m_sprite, mlText, 5,5, m_window->getClientSize().Width, CV_White);

		
		Apoc3D::Math::Rectangle dstRect(getWindow()->getClientSize().Width - 512,30,512, 512);
		uming14->DrawString(m_sprite, L"Glyph Cache Map(dynamic):", Point(dstRect.X, 10), CV_White);
		m_sprite->Draw(uming14->getInternalTexture(), dstRect, nullptr, CV_White);
		
		uming14->DrawString(m_sprite, L"Random Text:", Point(5, 100), CV_White);

		int unicodeStep = (0x6d00 - 0x673A) / 5;
		int unicodeBegin = 0x673a;
		int unicodeEnd = 0x673a + unicodeStep * m_currentPressure;

		for (int i=0;i<24;i++)
		{
			wchar_t randomText[32];
			memset(randomText, 0, sizeof(randomText));

			for (int j=0;j<31;j++)
			{
				randomText[j] = unicodeBegin + rand() % (unicodeEnd-unicodeBegin);
			}

			uming14->DrawString(m_sprite, randomText, Point(5, 100 + uming14->getLineHeightInt() * (i+1)), CV_White);
		}

		String helperText[3] =
		{
			L"Current Pressure: " + StringUtils::IntToString(unicodeEnd - unicodeBegin) + L" glyphs",
			L" [-] : Decrease Pressure ",
			L" [+] : Increase Pressure "
		};
		for (int i=0;i<3;i++)
		{
			uming14->DrawString(m_sprite, helperText[i], Point(dstRect.X, dstRect.getBottom() + 15 + i*uming14->getLineHeightInt()), CV_White);
		}


		Font* english = FontManager::getSingleton().getFont(L"english");
		english->DrawString(m_sprite, L"FPS: " + StringUtils::SingleToString(time->getFPS(),1), Point(0, m_window->getClientSize().Height-50), CV_White);

		static int32 disCurLen = 0;
		disCurLen++;
		if (disCurLen>200)
			disCurLen = 0;
		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, m_window->getClientSize().Height-110.0f, CV_White, disCurLen*0.5f, 10, Point(3,3), 0.3f);

		static float wordDisProgress = 0;
		wordDisProgress += time->getElapsedTime()*5;
		if (wordDisProgress>5)
			wordDisProgress = 0;

		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, m_window->getClientSize().Height-90.0f, CV_White, wordDisProgress, -2, Point(3,3), 0.3f);

		static float allDisProgress = 0;
		allDisProgress += time->getElapsedTime();
		if (allDisProgress>2)
			allDisProgress = 0;

		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, m_window->getClientSize().Height-70.0f, CV_White, 1-Math::Saturate(allDisProgress), 0, Point(3,3), 0.3f);


		m_sprite->End();

		Game::Draw(time);

		m_device->EndFrame();
	}

	void FontDemo::OnFrameStart() { }
	void FontDemo::OnFrameEnd() { }

}