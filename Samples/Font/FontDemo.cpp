#include "FontDemo.h"

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
			FileLocation fl = FileSystem::getSingleton().Locate(L"uming14.fnt", FileLocateRule::Default);
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
	void FontDemo::Update(const GameTime* time)
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

		m_lengthDissolveProgress += time->ElapsedTime * 60;
		if (m_lengthDissolveProgress > 200)
			m_lengthDissolveProgress = 0;

		m_allDissolveProgress += time->getElapsedTime();
		if (m_allDissolveProgress > 2)
			m_allDissolveProgress = 0;

		m_wordDissolveProgress += time->getElapsedTime() * 5;
		if (m_wordDissolveProgress > 5)
			m_wordDissolveProgress = 0;

	}
	void FontDemo::Draw(const GameTime* time)
	{
		m_device->BeginFrame();
		m_device->Clear(CLEAR_ColorBuffer, 0, 1, 0);

		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_AlphaBlended | Sprite::SPR_RestoreState));
		
		Size areaSize = m_window->getClientSize();
		Font* uming14 = FontManager::getSingleton().getFont(L"uming14");
		//Font* uming22 = FontManager::getSingleton().getFont(L"uming22");

		uming14->DrawString(m_sprite, mlText, 5, 5, areaSize.Width, CV_White);

		
		Apoc3D::Math::Rectangle dstRect(areaSize.Width - 512, 30, 512, 512);
		uming14->DrawString(m_sprite, L"Glyph Cache Map(dynamic):", Point(dstRect.X, 10), CV_White);
		m_sprite->Draw(uming14->getInternalTexture(), dstRect, nullptr, CV_White);
		
		uming14->DrawString(m_sprite, L"Random Text:", Point(5, 100), CV_White);

		int unicodeStep = (0x6d00 - 0x673A) / 5;
		int unicodeBegin = 0x673a;
		int unicodeEnd = 0x673a + unicodeStep * m_currentPressure;

		for (int i=0;i<24;i++)
		{
			wchar_t randomText[32];
			ZeroArray(randomText);

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
		english->DrawString(m_sprite, L"FPS: " + StringUtils::SingleToString(time->getFPS(), StrFmt::fp<2>::val), Point(0, areaSize.Height - 50), CV_White);

		int32 disCurLen = Math::Round(m_lengthDissolveProgress);

		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 110.0f, CV_White, disCurLen*0.5f, 10, Point(3, 3), 0.3f);
		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 90.0f, CV_White, m_wordDissolveProgress, -2, Point(3, 3), 0.3f);
		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 70.0f, CV_White, 1 - Math::Saturate(m_allDissolveProgress), 0, Point(3, 3), 0.3f);

		english->DrawString(m_sprite, L"Color Control Code: " + Font::MakeColorControl(CV_Red) + L"Red " + Font::MakeColorControl(CV_Green) + L"Green",
			Point(200, areaSize.Height - 110), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + Font::MakeMoveControl({ 115, 0 }, false) + L"[P]",
			Point(200, areaSize.Height - 90), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + Font::MakeMoveControl({ 115, 0 }, true) + L"[P]",
			Point(200, areaSize.Height - 70), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + Font::MakeMoveControl({ 15, 0 }, false, true) + L"[P]",
			Point(200, areaSize.Height - 50), CV_White);

		m_sprite->End();

		Game::Draw(time);

		m_device->EndFrame();
	}

	void FontDemo::OnFrameStart() { }
	void FontDemo::OnFrameEnd() { }

}