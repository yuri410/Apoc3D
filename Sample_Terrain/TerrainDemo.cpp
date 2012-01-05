#include "TerrainDemo.h"

#include "Core/GameTime.h"
#include "Config/XmlConfiguration.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/RenderStateManager.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "Graphics/EffectSystem/EffectParameter.h"
#include "Graphics/TextureManager.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/ModelManager.h"
#include "Graphics/Model.h"
#include "Scene/OctreeSceneManager.h"
#include "Scene/SceneRenderer.h"
#include "Input/InputAPI.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"
#include "Math/ColorValue.h"
#include "Math/RandomUtils.h"
#include "Math/PerlinNoise.h"
#include "Utility/StringUtils.h"
#include "UILib/FontManager.h"
#include "UILib/Console.h"
#include "Platform/Thread.h"

#include "GameCamera.h"
#include "Terrain.h"
#include "TerrainMesh.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;


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
			LocateCheckPoint cp;
			cp.AddPath(L"textures.pak");
			FileLocateRule::Textures.AddCheckPoint(cp);
		}
		{
			FileLocation* fl = FileSystem::getSingleton().Locate(L"comic.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"comic", fl);
		}

		m_console->Minimize();

		FileLocation* fl = FileSystem::getSingleton().Locate(L"effectList.xml", FileLocateRule::Effects);
		EffectManager::getSingleton().LoadEffectFromList(m_device, fl);
		delete fl;
		
		Viewport vp = m_device->getViewport();

		m_camera = new GameCamera(vp.Width/(float)(vp.Height));
		m_camera->setPosition(Vector3Utils::LDVector(100,0,100));
		m_camera->UpdateTransform();

		m_sceneRenderer = new SceneRenderer(m_device);

		fl = FileSystem::getSingleton().Locate(L"Renderer.xml", FileLocateRule::Default);

		XMLConfiguration* config = new XMLConfiguration(fl);
		m_sceneRenderer->Load(config);
		delete config;
		delete fl;

		m_sceneRenderer->RegisterCamera(m_camera);

		RendererEffectParams::CurrentCamera = m_camera;
		RendererEffectParams::LightDirection = Vector3Utils::LDVector(-0.707f,0.707f,0);

		TerrainMeshManager::Initialize();
		TerrainMeshManager::getSingleton().InitializeResources(m_device);
		Terrain::NewSeed();

		m_scene = new OctreeSceneManager(OctreeBox(20000), 20000/256);

		
		LoadUI();

	}
	void TerrainDemo::LoadScene()
	{
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
			StaticObject* obj = new StaticObject();
			obj->setRadius(9999999);

			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"models");
			rule.AddCheckPoint(cp);

			FileLocation* fl = FileSystem::getSingleton().Locate(L"SkySphere.mesh", rule);
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
		
		FileLocation* fl = FileSystem::getSingleton().Locate(L"loading.tex", FileLocateRule::Default);
		
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

		if (m_sceneContentLoaded>0)
		{
			m_sceneContentLoaded--;
		}
		else if (m_sceneContentLoaded == 0)
		{
			LoadScene();
			m_sceneContentLoaded = -1;
		}
		else if (m_isLoading)
		{
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
		
		m_sprite->Begin(true, false);
		DrawUI(m_sprite);


		m_sprite->End();

		m_device->EndFrame();
	}

	void TerrainDemo::OnFrameStart() { }
	void TerrainDemo::OnFrameEnd() { }

	void TerrainDemo::UpdateCamera()
	{
		m_isMoving = false;

		if (!m_isLoading)
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


		fnt->DrawString(m_sprite, L"Operation: " + StringUtils::ToString(op), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Cache Usage: " + StringUtils::ToString(usage/1048576) + L"/" + StringUtils::ToString(total/1048576) + L"MB", Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Batch Count: " + StringUtils::ToString(m_device->getBatchCount()), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"Primitive Count: " + StringUtils::ToString(m_device->getPrimitiveCount()), Point(x,y), CV_White);
		y+=35;
		fnt->DrawString(m_sprite, L"FPS: " + StringUtils::ToString(m_window->getFPS(), 2, 2), Point(x,y), CV_White);

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

		float moveBlend = 1- Math::Saturate(2*  fabs(m_helpShowState - 5)/4.5f-1);
		float lookBlend = 1- Math::Saturate(2*  fabs(m_helpShowState - 15)/4.5f-1);

		uint moveAlpha = (uint)(255 * moveBlend);
		uint lookAlpha = (uint)(255 * lookBlend);

		if (moveAlpha>0)
		{
			m_sprite->Draw(m_helpMove, x,y , PACK_COLOR(0xff,0xff,0xff,moveAlpha));
		}
		if (lookAlpha>0)
		{
			m_sprite->Draw(m_helpLook, x,y , PACK_COLOR(0xff,0xff,0xff,lookAlpha));
		}
		
	}
}