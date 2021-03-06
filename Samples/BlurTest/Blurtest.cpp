
#include "Blurtest.h"

#include <cstddef>

namespace Sample_GUI
{

	Point GetProperHalfSize(Point sz)
	{
		Point r = sz / 2;

		if (r.X % 2) r.X++;
		if (r.Y % 2) r.Y++;

		return r;
	}

	struct QuadVertex
	{
		float Position[3];
	};

    BlurTest::BlurTest(RenderWindow* wnd)
		: Application(wnd), m_blur13(13, 1.5f, 640, 360), m_blur25(25, 2.5f, 1280, 720)
    {
		GaussBlurFilter test(25, 1, 512, 512, 1);

		float testW[25];
		FillArray(testW, test.getSampleWeights());
    }

	BlurTest::~BlurTest()
	{		

	}

	void BlurTest::OnFrameStart() { }
	void BlurTest::OnFrameEnd() { }
	void BlurTest::Draw(const AppTime* time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();

		Vector2 texSize;
		Vector2 texSizeMOT;
		Vector2 texUVScale;
		{
			Point sz(m_original->getWidth(), m_original->getHeight());
			texSize = Vector2((float)sz.X, (float)sz.Y);
			 texSizeMOT = texSize;

			if (sz.X % 2) texSizeMOT.X++;
			if (sz.Y % 2) texSizeMOT.Y++;

			texUVScale = Vector2(texSizeMOT.X / texSize.X, texSizeMOT.Y / texSize.Y);
		}

		AutomaticEffect* fxBlur13 = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"p_blur_color13"));
		AutomaticEffect* fxBlur25 = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"p_blur_color25"));

		AutomaticEffect* fxPack = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"blur_pack"));
		AutomaticEffect* fxUnpack = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"blur_unpack"));

		AutomaticEffect* fxTexCopy = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"TexturedQuad"));

		if (0)
		{
			if (m_mode == 0)
			{
				m_device->SetRenderTarget(0, m_blur2);
				DrawQuad(fxTexCopy, m_original);
			}
			else if (m_mode == 1)
			{
				m_device->SetRenderTarget(0, m_blur1);
				DrawQuad(fxBlur25, m_original, m_blur25, false);

				m_device->SetRenderTarget(0, m_blur2);
				DrawQuad(fxBlur25, m_blur1->GetColorTexture(), m_blur25, true);

				//m_device->SetRenderTarget(0, m_blur1);
				//DrawQuad(fxBlur25, m_blur2->GetColorTexture(), m_blur25, false);

				//m_device->SetRenderTarget(0, m_blur2);
				//DrawQuad(fxBlur25, m_blur1->GetColorTexture(), m_blur25, true);
			}
			else
			{
				m_device->SetRenderTarget(0, m_blur1SM);
				fxPack->SetParameterValueByName(L"texSize", texSize);
				//fxPack->SetParameterValueByName(L"uvScale", texUVScale);
				DrawQuad(fxPack, m_original);

				m_device->SetRenderTarget(0, m_blur2SM);
				DrawQuad(fxBlur13, m_blur1SM->GetColorTexture(), m_blur13, false);

				m_device->SetRenderTarget(0, m_blur1SM);
				DrawQuad(fxBlur13, m_blur2SM->GetColorTexture(), m_blur13, true);


				m_device->SetRenderTarget(0, m_blur2);
				fxUnpack->SetParameterValueByName(L"texSize", texSize);
				//fxUnpack->SetParameterValueByName(L"uvScale", texUVScale);
				DrawQuad(fxUnpack, m_blur1SM->GetColorTexture());
			}
		}
		

		DataRectangle dr = m_uploadTest->Lock(0, LOCK_Discard);
		char* ptr = (char*)dr.getDataPointer();

		for (int32 i = 0; i < dr.getHeight(); i++)
		{
			uint32* row = (uint32*)ptr;
			for (int32 j = 0; j < dr.getWidth(); j++)
			{
				//float x = j * 10.0f / dr.getWidth();
				//float y = i * 2.0f / dr.getHeight() - 1;

				//float v = sinf(x);

				//*row++ = fabs(v-y)<0.01f ? 0xffffffffU : 0xff000000;

				byte gray = i * 255 / dr.getHeight();
				*row++ = CV_PackLA(gray, 0xff);
			}
			ptr += dr.getPitch();
		}
		m_uploadTest->Unlock(0);

		m_device->SetRenderTarget(0, nullptr);


		m_sprite->Begin(Sprite::SPRMix_ManageStateAlphaBlended);
		
		//m_sprite->Draw(m_blur2->GetColorTexture(), Point(0, 0), CV_White);
		
		m_sprite->Draw(m_uploadTest, m_device->getViewport().getArea(), nullptr, CV_White);

		Font* fnt = FontManager::getSingleton().getFont(L"english");
		Viewport vp = m_device->getViewport();

		int x = 26;
		int y = vp.Height - (675 - 480);

		fnt->DrawString(m_sprite, L"FPS: " + StringUtils::SingleToString(m_window->getFPS(), StrFmt::fp<0>::val), Point(x, y), CV_White);
		y += 25;


		fnt->DrawString(m_sprite, L"Mode: " + String(m_mode == 0 ? L"source" : (m_mode == 1 ? L"traditional" : L"packed")), Point(x, y), CV_White);

		//m_sprite->Draw(m_blur2->GetColorTexture(), Point(520, 0), CV_White);

		m_sprite->End();

		Application::Draw(time);

		m_device->EndFrame();

	}

	void BlurTest::DrawQuad(AutomaticEffect* fx, Texture* tex)
	{
		fx->SetParameterTextureByName(L"tex", tex);

		m_quadTempMtrl->Cull = CullMode::None;
		m_quadTempMtrl->SetPassEffect(0, fx);
		m_quadTempMtrl->setPassFlags(1);

		m_quadTempGeoData.BaseVertex = 0;
		m_quadTempGeoData.IndexBuffer = nullptr;
		m_quadTempGeoData.PrimitiveCount = 2;
		m_quadTempGeoData.PrimitiveType = PrimitiveType::TriangleList;
		m_quadTempGeoData.VertexBuffer = m_quadBuffer;
		m_quadTempGeoData.VertexCount = 6;
		m_quadTempGeoData.VertexDecl = m_quadVtxDecl;
		m_quadTempGeoData.VertexSize = m_quadVtxDecl->GetVertexSize();

		RenderOperation rop;
		rop.Material = m_quadTempMtrl;
		rop.GeometryData = &m_quadTempGeoData;
		rop.RootTransform.LoadIdentity();

		m_device->Render(m_quadTempMtrl, &rop, 1, 0);
	}
	void BlurTest::DrawQuad(AutomaticEffect* fx, Texture* tex, const GaussBlurFilter& flt, bool blurY)
	{
		Vector4 buf[30];
		for (int32 i = 0; i < flt.getSampleCount();i++)
		{
			buf[i].X = flt.getSampleWeights()[i];
		}
		fx->SetParameterValueByName(L"weights", (float*)buf, flt.getSampleCount() * 4);
		fx->SetParameterValueByName(L"offsets", (float*)(blurY ? flt.getSampleOffsetY() : flt.getSampleOffsetX()), flt.getSampleCount() * 4);

		DrawQuad(fx, tex);
	}

	void BlurTest::Initialize()
	{
		m_window->setTitle(L"Blur Test");
		Application::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void BlurTest::Finalize()
	{
		Application::Finalize();
	}


	void BlurTest::Load()
	{
		Application::Load();
		
		m_console->Minimize();

		FileLocation fl = FileSystem::getSingleton().Locate(L"effectList.xml", FileLocateRule::Effects);
		EffectManager::getSingleton().LoadEffectsFromList(m_device, fl);

		fl = FileSystem::getSingleton().Locate(L"image1.tex", FileLocateRule::Textures);
		m_original = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

		Point sz = { m_original->getWidth(), m_original->getHeight() };

		ObjectFactory* fac = m_device->getObjectFactory();
		m_blur1 = fac->CreateRenderTarget(sz, FMT_Luminance8);
		m_blur2 = fac->CreateRenderTarget(sz, FMT_Luminance8);
		m_blur1SM = fac->CreateRenderTarget(GetProperHalfSize(sz), FMT_A8R8G8B8);
		m_blur2SM = fac->CreateRenderTarget(GetProperHalfSize(sz), FMT_A8R8G8B8);

		m_uploadTest = fac->CreateTexture(1920, 1080, 1, TU_Dynamic, FMT_A8R8G8B8);

		List<VertexElement> elements;
		elements.Add(VertexElement(0, VEF_Vector3, VEU_Position));
		m_quadVtxDecl = fac->CreateVertexDeclaration(elements);

		m_quadBuffer = fac->CreateVertexBuffer(6, m_quadVtxDecl, BU_Static);
		QuadVertex* vtxData = reinterpret_cast<QuadVertex*>(m_quadBuffer->Lock(LOCK_None));
		vtxData[0].Position[0] = 0; vtxData[0].Position[1] = 0; vtxData[0].Position[2] = 0;
		vtxData[1].Position[0] = 1; vtxData[1].Position[1] = 0; vtxData[1].Position[2] = 0;
		vtxData[2].Position[0] = 1; vtxData[2].Position[1] = 1; vtxData[2].Position[2] = 0;

		vtxData[3].Position[0] = 0; vtxData[3].Position[1] = 0; vtxData[3].Position[2] = 0;
		vtxData[4].Position[0] = 1; vtxData[4].Position[1] = 1; vtxData[4].Position[2] = 0;
		vtxData[5].Position[0] = 0; vtxData[5].Position[1] = 1; vtxData[5].Position[2] = 0;

		m_quadBuffer->Unlock();

		m_quadTempMtrl = new Material(m_device);
		m_quadTempMtrl->SourceBlend = Blend::SourceAlpha;
		m_quadTempMtrl->DestinationBlend = Blend::InverseSourceAlpha;

	}

	void BlurTest::Unload()
	{
		Application::Unload();


		delete m_sprite;
	}

	void BlurTest::Update(const AppTime* time)
	{
		Application::Update(time);

		Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
		if (kb->IsKeyUp(KEY_SPACE))
		{
			m_mode++;
			if (m_mode > 2)
				m_mode = 0;
		}
	}

}
