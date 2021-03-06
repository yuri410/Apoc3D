#include "FontDemo.h"

namespace SampleFont
{
	const int32 MaxPressure = 10;
	const String mlText =
		L"This font supports multiple languages.\n"
		L"这种字体支持多国语言。\n"
		L"このフォントは、複数の言語をサポートしています。\n"
		L"ta czcionka obsługuje wiele języków.";

	FontDemo::FontDemo(RenderWindow* wnd)
		: Application(wnd), m_currentPressure(5)
	{
		//wnd->SetupFixedFrameTime(false);
	}

	FontDemo::~FontDemo()
	{		

	}

	void FontDemo::Initialize()
	{
		m_window->setTitle(L"Font Demo");
		Application::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void FontDemo::Finalize()
	{
		Application::Finalize();
	}

	void FontDemo::Load()
	{
		Application::Load();

		{
			FileLocation fl = FileSystem::getSingleton().Locate(L"uming14.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming14", fl);

			fl = FileSystem::getSingleton().Locate(L"uming22.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"uming22", fl);

			fl = FileSystem::getSingleton().Locate(L"HS14.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"HS14", fl);
			fl = FileSystem::getSingleton().Locate(L"HS14B.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"HS14B", fl);
			fl = FileSystem::getSingleton().Locate(L"HS14BB.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"HS14BB", fl);

			//fl = FileSystem::getSingleton().Locate(L"HSTC14.fnt", FileLocateRule::Default);
			//FontManager::getSingleton().LoadFont(m_device, L"HSTC14", fl);
			//fl = FileSystem::getSingleton().Locate(L"HSTC14B.fnt", FileLocateRule::Default);
			//FontManager::getSingleton().LoadFont(m_device, L"HSTC14B", fl);
			//fl = FileSystem::getSingleton().Locate(L"HSTC14BB.fnt", FileLocateRule::Default);
			//FontManager::getSingleton().LoadFont(m_device, L"HSTC14BB", fl);

			FontManager::getSingleton().ReportComplexFonts();
		}
		
		m_console->Minimize();

	}
	void FontDemo::Unload()
	{
		delete m_sprite;

		Application::Unload();
	}
	void FontDemo::Update(const AppTime* time)
	{
		Application::Update(time);

		Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();

		if (kb->IsKeyDown(KEY_EQUALS))
		{
			m_currentPressure++;
			if (m_currentPressure > MaxPressure)
				m_currentPressure = MaxPressure;
		}
		if (kb->IsKeyDown(KEY_MINUS))
		{
			m_currentPressure--;
			if (m_currentPressure < 1)
				m_currentPressure = 1;
		}

		m_lengthDissolveProgress += time->ElapsedTime * 60;
		if (m_lengthDissolveProgress > 200)
			m_lengthDissolveProgress = 0;

		m_allDissolveProgress += time->ElapsedTime;
		if (m_allDissolveProgress > 2)
			m_allDissolveProgress = 0;

		m_wordDissolveProgress += time->ElapsedTime * 5;
		if (m_wordDissolveProgress > 5)
			m_wordDissolveProgress = 0;

		

	}
	void FontDemo::UpdateConstrainedVarTimeStep(const AppTime* time)
	{
		float d = m_testDirection*0.1f;
		m_test += time->ElapsedTime * d;
		if (m_test > 1)
			m_testDirection = -1;
		else if (m_test < 0)
			m_testDirection = 1;

	}
	void FontDemo::Draw(const AppTime* time)
	{
		m_device->BeginFrame();
		m_device->Clear(CLEAR_ColorBuffer, 0, 1, 0);

		m_sprite->Begin(Sprite::SPRMix_ManageStateAlphaBlended);
		
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
		english->DrawString(m_sprite, L"FPS: " + StringUtils::SingleToString(time->FPS, StrFmt::fp<2>::val), Point(0, areaSize.Height - 50), CV_White);

		int32 disCurLen = Math::Round(m_lengthDissolveProgress);

		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 110.0f, CV_White, disCurLen*0.5f, 10, Point(3, 3), 0.3f);
		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 90.0f, CV_White, m_wordDissolveProgress, -2, Point(3, 3), 0.3f);
		english->DrawStringDissolving(m_sprite, L"THIS IS DISSOLVING TEXT.", 5.0f, areaSize.Height - 70.0f, CV_White, 1 - Math::Saturate(m_allDissolveProgress), 0, Point(3, 3), 0.3f);

		english->DrawString(m_sprite, L"Color Control Code: " + ControlCodes::MakeColorControl(CV_Red) + L"Red " + ControlCodes::MakeColorControl(CV_Green) + L"Green",
			Point(200, areaSize.Height - 110), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + ControlCodes::MakeMoveControl({ 115, 0 }, false) + L"[P]",
			Point(200, areaSize.Height - 90), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + ControlCodes::MakeMoveControl({ 115, 0 }, true) + L"[P]",
			Point(200, areaSize.Height - 70), CV_White);

		english->DrawString(m_sprite, L"Move Control Code: " + ControlCodes::MakeMoveControl({ 15, 0 }, false, true) + L"[P]",
			Point(200, areaSize.Height - 50), CV_White);

		if (0)
		{
			Apoc3D::Math::Rectangle testArea = { { (int32)(m_test*areaSize.Width), 0 }, { 5, areaSize.Height } };
			m_sprite->Draw(SystemUI::GetWhitePixel(), testArea, nullptr, CV_Red);
			english->DrawString(m_sprite, L"No stuttering should occur", testArea.getBottomRight() - Point(-3, 35), CV_Red);
			english->DrawString(m_sprite, L"on the movement of this line.", testArea.getBottomRight() - Point(-3, 20), CV_Red);
		}

		m_sprite->End();

		Application::Draw(time);

		m_device->EndFrame();
	}

	void FontDemo::OnFrameStart() { }
	void FontDemo::OnFrameEnd() { }

}