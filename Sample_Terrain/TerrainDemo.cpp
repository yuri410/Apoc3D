#include "TerrainDemo.h"

#include "Core/GameTime.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "Graphics/TextureManager.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"

#include "Input/InputAPI.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"
#include "Math/ColorValue.h"
#include "Math/RandomUtils.h"
#include "Utility/StringUtils.h"
#include "Platform/Thread.h"

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


	}
	void TerrainDemo::Unload()
	{
		delete m_sprite;

		Game::Unload();
	}
	void TerrainDemo::Update(const GameTime* const time)
	{
		Game::Update(time);

	}
	void TerrainDemo::Draw(const GameTime* const time)
	{
		m_device->BeginFrame();

		Game::Draw(time);

		m_device->EndFrame();
	}

	void TerrainDemo::OnFrameStart() { }
	void TerrainDemo::OnFrameEnd() { }
}