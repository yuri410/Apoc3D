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
		: Game(wnd)
	{
		
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

		TerrainMeshManager::Initialize();
		TerrainMeshManager::getSingleton().InitializeResources(m_device);


		m_scene = new OctreeSceneManager(OctreeBox(20000), 20000/256);
		//PerlinNoise::Frequency = 0.01f;
		//PerlinNoise::Persistency = 0.3f;
		//PerlinNoise::NumInterations = 8;
		//PerlinNoise::ComputeRandomTable();
		
		for (int i=-15;i<=15;i++)
		{
			for (int j=-15;j<=15;j++)
			{
				Terrain* t1 = new Terrain(m_device, i,j);
				m_scene->AddObject(t1);
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
		//m_device->getRenderState()->SetFillMode(FILL_Solid);
	}
	void TerrainDemo::Unload()
	{
		m_sceneRenderer->UnregisterCamera(m_camera);

		delete m_sprite;
		delete m_sceneRenderer;
		delete m_scene;
		delete m_camera;

		TerrainMeshManager::getSingleton().FinalizeResources();
		TerrainMeshManager::Finalize();
		Game::Unload();
	}
	void TerrainDemo::Update(const GameTime* const time)
	{
		Game::Update(time);
		m_camera->Update(time);
		m_scene->Update(time);

		UpdateCamera();
	}
	void TerrainDemo::Draw(const GameTime* const time)
	{
		m_device->BeginFrame();

		m_sceneRenderer->RenderScene(m_scene);

		Game::Draw(time);

		m_device->EndFrame();
	}

	void TerrainDemo::OnFrameStart() { }
	void TerrainDemo::OnFrameEnd() { }

	void TerrainDemo::UpdateCamera()
	{
		Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
		if (kb->IsPressing(KEY_W))
			m_camera->MoveForward();
		if (kb->IsPressing(KEY_A))
			m_camera->MoveLeft();
		if (kb->IsPressing(KEY_D))
			m_camera->MoveRight();
		if (kb->IsPressing(KEY_S))
			m_camera->MoveBackward();
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