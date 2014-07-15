#include "TerrainDemo.h"

#include "GameCamera.h"
#include "Terrain.h"
#include "TerrainMesh.h"
#include "TerrainCommands.h"

namespace SampleTerrain
{
	TerrainDemo::TerrainDemo(RenderWindow* wnd)
		: Game(wnd), m_allowTakingDownTrees(true), m_isMoving(false), m_isLoading(true), m_sceneContentLoaded(2), 
		m_zeroOpFrameCounter(0), m_helpShowState(0)
	{
		memset(m_terrainBlocks, 0, sizeof(m_terrainBlocks));
	}

	TerrainDemo::~TerrainDemo()
	{		

	}

	void TerrainDemo::Initialize()
	{
		m_window->setTitle(L"Terrain Demo");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void TerrainDemo::Finalize()
	{
		Game::Finalize();
	}

	void TerrainDemo::Load()
	{
		Game::Load();

		{
			// This will change the default FileLocateRule 'Textures' to our own one, which
			// can search in the pak file.
			LocateCheckPoint cp;
			cp.AddPath(L"textures.pak");
			FileLocateRule::Textures.AddCheckPoint(cp);
		}
		{
			// Add a comic font to the font manager. Use of FileLocation is explained here.

			// Locate the file, using "comic.fnt" as the name, FileLocateRule::Default as the rule.
			// In this case, Default is to search the file in the same dir as the application
			FileLocation fl = FileSystem::getSingleton().Locate(L"comic.fnt", FileLocateRule::Default);

			// Giving the font a name and telling FontManager the location, it will load it.
			FontManager::getSingleton().LoadFont(m_device, L"comic", fl);
		}

		m_console->Minimize();

		// using the effect list generated by the build system, APBuild, is very convenience
		// for only 3 lines of code to load all the shader effects
		FileLocation fl = FileSystem::getSingleton().Locate(L"effectList.xml", FileLocateRule::Effects);
		EffectManager::getSingleton().LoadEffectFromList(m_device, fl);
		
		Viewport vp = m_device->getViewport();

		m_camera = new GameCamera(vp.Width/(float)(vp.Height));
		m_camera->setPosition(Vector3(100,0,100));
		m_camera->UpdateTransform();

		m_sceneRenderer = new SceneRenderer(m_device);

		// The scene render script is located
		fl = FileSystem::getSingleton().Locate(L"Renderer.xml", FileLocateRule::Default);
		// Then loaded by SceneRenderer
		Configuration* config = XMLConfigurationFormat::Instance.Load(fl);
		m_sceneRenderer->Load(config);
		delete config;

		m_sceneRenderer->RegisterCamera(m_camera);

		// setting the parameters to be used in shader effects here
		// In this demo, as it is very simple, light source is fixed
		RendererEffectParams::CurrentCamera = m_camera;
		RendererEffectParams::LightDirection = Vector3(-0.707f,0.707f,0);

		// our terrain mesh manager, see the header file for details
		TerrainMeshManager::Initialize();
		TerrainMeshManager::getSingleton().InitializeResources(m_device);
		Terrain::NewSeed();

		m_scene = new OctreeSceneManager(OctreeBox(20000), 20000/256);

		RegisterTerrainCommands();
		
		LoadUI();

		//List<String> modes;
		//m_device->getCapabilities()->EnumerateRenderTargetMultisampleModes(FMT_A8R8G8B8, m_device->GetDefaultDepthStencilFormat(), modes);
	}
	void TerrainDemo::LoadScene()
	{
		// load NumBlockOnEdge by NumBlockOnEdge of terrain chunks
		for (int i=MinBlockCoord;i<=MaxBlockCoord;i++)
		{
			for (int j=MinBlockCoord;j<=MaxBlockCoord;j++)
			{
				Terrain* t1 = new Terrain(m_device, i,j);
				m_scene->AddObject(t1);

				m_terrainBlocks[i - MinBlockCoord][j - MinBlockCoord] = t1;
			}
		}

		{
			// now load up the Sky, a sphere mesh. 
			// A big radius ensures its always regarded in sight by the engine.
			StaticObject* obj = new StaticObject();
			obj->setRadius(9999999);

			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"models");
			rule.AddCheckPoint(cp);

			FileLocation fl = FileSystem::getSingleton().Locate(L"SkySphere.mesh", rule);
			Model* sky = new Model(ModelManager::getSingleton().CreateInstance(m_device, fl));
			obj->setModel(0, sky);

			m_scene->AddObject(obj);
		}
	}
	void TerrainDemo::LoadUI()
	{

		Viewport vp = m_device->getViewport();
		int x = vp.Width - 180;
		int y = vp.Height - 120;

		m_cbPushTrees = new CheckBox(Point(x,y), L"", false);
		m_cbPushTrees->SetSkin(m_UIskin);
		m_cbPushTrees->Initialize(m_device);
		m_cbPushTrees->setFontRef(FontManager::getSingleton().getFont(L"comic"));
		m_cbPushTrees->setValue(true);
		
		FileLocation fl = FileSystem::getSingleton().Locate(L"loading.tex", FileLocateRule::Default);
		m_loadingScreen = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"helpmove.tex", FileLocateRule::Default);
		m_helpMove = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"helplook.tex", FileLocateRule::Default);
		m_helpLook = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);
	}
	void TerrainDemo::Unload()
	{
		m_sceneRenderer->UnregisterCamera(m_camera);

		
		delete m_sceneRenderer;
		delete m_scene;
		delete m_camera;
		
		for (int i=MinBlockCoord;i<=MaxBlockCoord;i++)
		{
			for (int j=MinBlockCoord;j<=MaxBlockCoord;j++)
			{
				delete m_terrainBlocks[i - MinBlockCoord][j - MinBlockCoord];
			}
		}

		delete m_sprite;



		delete m_cbPushTrees;
		delete m_helpLook;
		delete m_helpMove;


		TerrainMeshManager::getSingleton().Shutdown();
		TerrainMeshManager::getSingleton().FinalizeResources();
		TerrainMeshManager::Finalize();
		Game::Unload();
	}
	void TerrainDemo::Update(const GameTime* const time)
	{
		Game::Update(time);

		// pushing trees. Camera's world coord is converted to chunk space.
		// Once the chunk is found, it will be notified to do the work on trees.
		if (m_allowTakingDownTrees)
		{
			Vector3 camPos = m_camera->getPosition();
			int bx, bz;
			Terrain::GetBlockCoordinate(camPos, bx, bz);

			if (bx >=MinBlockCoord && bz >=MinBlockCoord && bx <= MaxBlockCoord && bz <= MaxBlockCoord)
			{
				Terrain* t = m_terrainBlocks[bx - MinBlockCoord][bz - MinBlockCoord];
				if (t) t->Push(camPos, m_camera->isSprinting() && m_isMoving);
			}
		}

		// the scene will be loaded after sound count down on,
		// to ensure the loading screen shown up before "the application
		// stops responding" if taking too long.
		if (m_sceneContentLoaded>0)
		{
			m_sceneContentLoaded--;
		}
		else if (m_sceneContentLoaded == 0)
		{
			LoadScene();
			// assign this to prevent call LoadScene every update
			m_sceneContentLoaded = -1;
		}
		else if (m_isLoading)
		{
			// the OperationCount from TerrainMeshManager(or other resource manager)
			// is very fluctuant some times.
			// We use a counter to do the trick. If the counter is counting up to a
			// bigg enough value. That means the terrainMeshManager has been idle for some
			// time period. Then the loading screen should gone.
			if (TerrainMeshManager::getSingleton().GetCurrentOperationCount()==0)
				m_zeroOpFrameCounter++;
			if (m_zeroOpFrameCounter>20)
			{
				m_isLoading = false;
			}
		}

		m_camera->Update(time);

		m_scene->Update(time);

		UpdateCamera();
		UpdateUI(time);
	}
	void TerrainDemo::Draw(const GameTime* const time)
	{
		m_device->BeginFrame();

		m_sceneRenderer->RenderScene(m_scene);

		Game::Draw(time);
		
		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_AlphaBlended | Sprite::SPR_RestoreState));
		DrawUI(m_sprite);


		m_sprite->End();

		m_device->EndFrame();

		Viewport vp = m_device->getViewport();
		m_camera->setAspectRatio(vp.Width/(float)(vp.Height));
	}

	void TerrainDemo::OnFrameStart() { }
	void TerrainDemo::OnFrameEnd() { }

	void TerrainDemo::UpdateCamera()
	{
		m_isMoving = false;

		if (!m_isLoading)
		{
			if (!UIRoot::getActiveForm() && !UIRoot::getTopMostForm())
			{
				Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
				if (kb->IsPressing(KEY_W))
				{
					m_camera->MoveForward();
					m_isMoving = true;
				}
				if (kb->IsPressing(KEY_A))
				{
					m_camera->MoveLeft();
					m_isMoving = true;
				}
				if (kb->IsPressing(KEY_D))
				{
					m_camera->MoveRight();
					m_isMoving = true;
				}
				if (kb->IsPressing(KEY_S))
				{
					m_camera->MoveBackward();
					m_isMoving = true;
				}
				if (kb->IsPressing(KEY_LSHIFT))
					m_camera->Sprint();
				if (kb->IsKeyDown(KEY_SPACE))
					m_camera->Jump();



				Mouse* mouse = InputAPIManager::getSingleton().getMouse();
				if (mouse->IsLeftPressedState())
				{
					m_camera->Turn(mouse->getDX()*0.25f, mouse->getDY()*0.25f);
				}
			}


		}
		

	}
	void TerrainDemo::UpdateUI(const GameTime* const time)
	{
		if (!m_isLoading)
		{
			m_cbPushTrees->Update(time);

			m_allowTakingDownTrees = m_cbPushTrees->getValue();

			if (m_helpShowState<20)
				m_helpShowState += time->getElapsedTime();
			
		}
		
	}

	void TerrainDemo::DrawUI(Sprite* sprite)
	{
		//return;
		int usage = (int)TerrainMeshManager::getSingleton().getUsedCacheSize();
		int total = (int)TerrainMeshManager::getSingleton().getTotalCacheSize();

		int op = TerrainMeshManager::getSingleton().GetCurrentOperationCount();

		Font* fnt = FontManager::getSingleton().getFont(L"comic");
		
		Viewport vp = m_device->getViewport();
		int x = 26;
		int y = vp.Height - (675-480);

		//m_device->getRenderState()->SetAlphaBlend(true, BLFUN_Add, BLEND_SourceAlpha, BLEND_InverseSourceAlpha, 0);

		fnt->DrawString(m_sprite, L"Operations: " + StringUtils::IntToString(op), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Cache Usage: " + StringUtils::IntToString(usage/1048576) + L"/" + StringUtils::IntToString(total/1048576) + L"MB", Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Batch Count: " + StringUtils::UIntToString(m_device->getBatchCount()), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Primitive Count: " + StringUtils::UIntToString(m_device->getPrimitiveCount()), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"FPS: " + StringUtils::SingleToString(m_window->getFPS(), 0, 0), Point(x,y), CV_White);

		x = vp.Width - 180;
		y = vp.Height - 120;
		m_cbPushTrees->Position = Point(x,y);
		m_cbPushTrees->Draw(sprite);

		x+= 30;
		y-= 7;
		fnt->DrawString(m_sprite, L"Push Trees", Point(x,y), CV_White);


		if (m_isLoading)
		{
			Apoc3D::Math::Rectangle dstRect(vp.X, vp.Y, vp.Width, vp.Height);
			m_sprite->Draw(m_loadingScreen, dstRect, CV_White);
		}


		x = vp.Width - 400;
		y = 100;

		// m_helpShowState, an accumulated time, is used to calculate the alpha value of the help instruction
		// images using the mapping below.
		// These are also used in terrain texturing in the pixel shader.
		float moveBlend = 1- Math::Saturate(2*  fabs(m_helpShowState - 5)/4.5f-1);
		float lookBlend = 1- Math::Saturate(2*  fabs(m_helpShowState - 15)/4.5f-1);

		uint moveAlpha = (uint)(255 * moveBlend);
		uint lookAlpha = (uint)(255 * lookBlend);

		if (moveAlpha>0)
		{
			m_sprite->Draw(m_helpMove, x,y , CV_PackLA(0xff,moveAlpha));
		}
		if (lookAlpha>0)
		{
			m_sprite->Draw(m_helpLook, x,y , CV_PackLA(0xff,lookAlpha));
		}
		
	}
}