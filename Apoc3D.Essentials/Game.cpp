
#include "Game.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/ObjectFactory.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Console.h"

#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::VFS;

namespace Apoc3DEx
{
	Game::Game(RenderWindow* wnd)
		: m_window(wnd), m_console(0), m_device(0), m_UIskin(0)
	{
		m_inputCreationParam.UseKeyboard = true;
		m_inputCreationParam.UseMouse = true;

	}

	Game::~Game()
	{
		
	}

	void Game::Exit()
	{
		m_window->Exit();
	}

	void Game::OnFrameStart()
	{

	}
	void Game::OnFrameEnd()
	{

	}
	void Game::Draw(const GameTime* const time)
	{
		UIRoot::Draw();
	}
	void Game::Initialize()
	{
		m_device = m_window->getRenderDevice();
		
		InputAPIManager::getSingleton().InitializeInput(m_window, m_inputCreationParam);
	}
	void Game::Finalize()
	{
		InputAPIManager::getSingleton().FinalizeInput();
	}
	void Game::Load()
	{
		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"system.pak\\classic_skin.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);

			m_UIskin->ControlFontName = L"english";
			FileLocation* fl = FileSystem::getSingleton().Locate(L"english.fnt", rule);
			FontManager::getSingleton().LoadFont(m_device, L"english", fl);
		}

		UIRoot::Initialize(m_device);

		m_console = new Console(m_device, m_UIskin, Point(600,100), Point(400,400));
	}

	void Game::Unload()
	{
		UIRoot::Finalize();
		delete m_UIskin;
		delete m_console;
	}

	void Game::Update(const GameTime* const time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);
		
		EffectManager::getSingleton().Update(time);

		m_console->Update(time);

		UIRoot::Update(time);
	}
}