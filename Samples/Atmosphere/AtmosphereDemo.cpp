#include "AtmosphereDemo.h"

namespace SampleAtmosphere
{
	void ConvertDataTexture(const String& file, bool useHalfOutput);

	struct QuadVertex
	{
		float Position[3];
	};

	AtmosphereDemo::AtmosphereDemo(RenderWindow* wnd)
		: Game(wnd)
	{
	}

	AtmosphereDemo::~AtmosphereDemo()
	{		

	}

	void AtmosphereDemo::Initialize()
	{
		m_window->setTitle(L"Atmosphere Demo");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void AtmosphereDemo::Finalize()
	{
		Game::Finalize();
	}

	void AtmosphereDemo::Load()
	{
		Game::Load();

		m_console->Minimize();

		FileLocation fl = FileSystem::getSingleton().Locate(L"effectList.xml", FileLocateRule::Effects);
		EffectManager::getSingleton().LoadEffectsFromList(m_device, fl);
		
		Viewport vp = m_device->getViewport();

		m_camera = new FreeCamera(vp.getAspectRatio());
		m_camera->setNear(20);
		m_camera->setFar(30000);
		//m_camera->setPosition(Vector3(100,0,100));
		m_camera->setPosition(Vector3(0, 0, -15000));
		m_camera->setFieldOfView(ToRadian(35));
		m_camera->UpdateTransform();
		m_camera->MaxVelocity = 3000;
		m_camera->Deacceleration = 1500;
		m_camera->Acceleration = 750;

		RendererEffectParams::CurrentCamera = m_camera;
		RendererEffectParams::LightDirection = Vector3(-0.707f, 0, -0).Normalized();
		//RendererEffectParams::LightDirection = Vector3(0, -0.707f, -0.707f);
		//RendererEffectParams::LightPosition = { 0,6300, -6300 };


		fl = FileSystem::getSingleton().Locate(L"inscatterTexture.tex", FileLocateRule::Textures);
		m_inscatterTex = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl);

		fl = FileSystem::getSingleton().Locate(L"irradianceTexture.tex", FileLocateRule::Textures);
		m_irradianceTex = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl);

		fl = FileSystem::getSingleton().Locate(L"transmittanceTexture.tex", FileLocateRule::Textures);
		m_transmittanceTex = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl);

		fl = FileSystem::getSingleton().Locate(L"models/unitsphere.mesh", FileLocateRule::Default);
		m_unitBall = new Model(ModelManager::getSingleton().CreateInstance(m_device, fl));

		m_skyMtrl = new Material(m_device);
		{
			MaterialData md;
			md.Load(FileSystem::getSingleton().Locate(L"materials/atmosphere_sky.mtrl", FileLocateRule::Default));
			m_skyMtrl->Load(md);
		}

		ObjectFactory* fac = m_device->getObjectFactory();

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

		//ConvertDataTexture(L"inscatterTexture.dat", true);
		//ConvertDataTexture(L"irradianceTexture.dat", true);
		//ConvertDataTexture(L"transmittanceTexture.dat", true);

	}

	void ConvertDataTexture(const String& file, bool useHalfOutput)
	{
		FileStream fs(file);
		BinaryReader br(&fs, false);

		int32 width = br.ReadInt32();
		int32 height = br.ReadInt32();
		int32 depth = br.ReadInt32();
		int32 chCount = br.ReadInt32();
		

		String dstFile = PathUtils::Combine(PathUtils::GetDirectory(file), PathUtils::GetFileNameNoExt(file) + L".tex");

		TextureData data;
		data.Format = useHalfOutput ? FMT_A16B16G16R16F : FMT_A32B32G32R32F;
		data.ContentSize = PixelFormatUtils::GetMemorySize(width, height, depth, data.Format);
		data.Flags = 0;// TextureData::TDF_LZ4Compressed;
		data.LevelCount = 1;
		data.Type = depth == 1 ? TextureType::Texture2D : TextureType::Texture3D;
		data.Levels.ReserveDiscard(1);

		TextureLevelData& tld = data.Levels[0];
		tld.Width = width;
		tld.Height = height;
		tld.Depth = depth;
		tld.LevelSize = data.ContentSize;
		tld.ContentData = new char[data.ContentSize];

		int pixCount = width*height*depth;
		float* buf = new float[pixCount*chCount];
		br.ReadBytes((char*)buf, pixCount*chCount*sizeof(float));


		if (useHalfOutput)
		{
			uint16* tempFloatBuffer = new uint16[pixCount*chCount];

			for (int32 i = 0; i < pixCount*chCount;i++)
				tempFloatBuffer[i] = R32ToR16(buf[i]);

			uint16* src = tempFloatBuffer;
			uint16* dst = (uint16*)tld.ContentData;
			for (int32 k = 0; k < depth; k++)
			{
				uint16* sliceDst = dst + (k * width*height * 4);

				for (int32 i = 0; i < height; i++)
				{
					uint16* rowDst = sliceDst + i * width * 4;

					for (int32 j = 0; j < width; j++)
					{
						for (int32 c = 0; c < chCount; c++)
							*rowDst++ = *src++;

						for (int32 c = 0; c < 4 - chCount; c++)
							*rowDst++ = 0;
					}
				}
			}

			delete[] tempFloatBuffer;
		}
		else
		{
			float* src = buf;

			float* dst = (float*)tld.ContentData;
			for (int32 k = 0; k < depth; k++)
			{
				float* sliceDst = dst + (k * width*height * 4);

				for (int32 i = 0; i < height; i++)
				{
					float* rowDst = sliceDst + i * width * 4;

					for (int32 j = 0; j < width; j++)
					{
						for (int32 c = 0; c < chCount; c++)
							*rowDst++ = *src++;

						for (int32 c = 0; c < 4 - chCount; c++)
							*rowDst++ = 0;
					}
				}
			}
		}
		
		delete[] buf;

		data.Save(FileOutStream(dstFile));
	}

	void AtmosphereDemo::Unload()
	{
		delete m_camera;
		
		delete m_sprite;

		Game::Unload();
	}
	void AtmosphereDemo::Update(const GameTime* time)
	{
		Game::Update(time);

		UpdateCamera(time);
	}
	void AtmosphereDemo::Draw(const GameTime* time)
	{
		m_device->BeginFrame();

		RendererEffectParams::CurrentCamera = m_camera;

		//AutomaticEffect* atfx = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"atmosphere"));

		//int32 idx = atfx->FindParameterIndex(L"irradianceSampler");
		//atfx->SetParameterTexture(idx, m_irradianceTex);

		//idx = atfx->FindParameterIndex(L"inscatterSampler");
		//atfx->SetParameterTexture(idx, m_inscatterTex);

		//idx = atfx->FindParameterIndex(L"transmittanceSampler");
		//atfx->SetParameterTexture(idx, m_transmittanceTex);
		//AutomaticEffect* atfx = up_cast<AutomaticEffect*>(EffectManager::getSingleton().getEffect(L"TexturedQuad"));

		//int32 idx = atfx->FindParameterIndex(L"tex");
		//atfx->SetParameterTexture(idx, m_irradianceTex);

		//DrawQuad(atfx);

		m_device->Clear(CLEAR_ColorAndDepth, CV_Black, 1, 0);

		RenderOperationBuffer* ops = m_unitBall->GetRenderOperation(0);
		if (ops)
		{
			m_device->Render(m_skyMtrl, &ops->get(0), ops->getCount(), 0);
		}


		Game::Draw(time);
		
		m_device->EndFrame();

		Viewport vp = m_device->getViewport();
		m_camera->setAspectRatio(vp.Width/(float)(vp.Height));
	}

	void AtmosphereDemo::OnFrameStart() { }
	void AtmosphereDemo::OnFrameEnd() { }

	void AtmosphereDemo::UpdateCamera(const GameTime* time)
	{
		if (SystemUI::TopMostForm == nullptr)
		{
			Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
			if (kb->IsPressing(KEY_W))
			{
				m_camera->MoveForward();
			}
			if (kb->IsPressing(KEY_A))
			{
				m_camera->MoveLeft();
			}
			if (kb->IsPressing(KEY_D))
			{
				m_camera->MoveRight();
			}
			if (kb->IsPressing(KEY_S))
			{
				m_camera->MoveBackward();
			}
			if (kb->IsPressing(KEY_LCONTROL))
			{
				m_camera->MoveDown();
			}
			if (kb->IsPressing(KEY_SPACE))
			{
				m_camera->MoveUp();
			}
			//if (kb->IsPressing(KEY_LSHIFT))
			//	m_camera->Sprint();
			//if (kb->IsKeyDown(KEY_SPACE))
			//	m_camera->Jump();



			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->IsLeftPressedState())
			{
				m_camera->Turn(mouse->getDX()*0.25f, mouse->getDY()*0.25f);
			}
		}

		m_camera->Update(time);

		const float Rg = 6420.0 - 500.0 + 0.1;

		Vector3 camPos = m_camera->getPosition();
		if (camPos.Length() < Rg)
		{
			camPos.NormalizeInPlace();
			camPos *= Rg;

			m_camera->setPosition(camPos);
			m_camera->UpdateTransform();
		}
	}

	void AtmosphereDemo::DrawQuad(AutomaticEffect* fx)
	{
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
}