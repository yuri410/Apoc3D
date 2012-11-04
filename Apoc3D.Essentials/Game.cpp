
#include "Game.h"

#include "Core/GameTime.h"
#include "Config/XmlConfiguration.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "UILib/StyleSkin.h"
#include "UILib/FontManager.h"
#include "UILib/Control.h"
#include "UILib/Button.h"
#include "UILib/Form.h"
#include "UILib/Menu.h"
#include "UILib/List.h"
#include "UILib/Console.h"

#include "Input/InputAPI.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;

namespace Apoc3DEx
{
	Game::Game(RenderWindow* wnd)
		: m_window(wnd), m_console(0), m_device(0), m_UIskin(0)
	{
		
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
		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);
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
		
		InputAPIManager::getSingleton().Update(time);
		EffectManager::getSingleton().Update(time);

		m_console->Update(time);

		UIRoot::Update(time);
	}
}