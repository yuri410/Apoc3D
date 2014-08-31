
#include "Game.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Input/FrameInputData.h"
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
		: m_window(wnd), m_correctedFrameTime(0,0,0,0,0,false)
	{
		m_inputCreationParam.UseKeyboard = true;
		m_inputCreationParam.UseMouse = true;
		m_inputCreationParam.PreferredGamepadCount = 0;
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
	void Game::Draw(const GameTime* time)
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
			pt.AddPath(L"system.pak\\skin.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);
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

	void Game::Update(const GameTime* time)
	{
		if (m_player && m_player->isFinished())
		{
			DELETE_AND_NULL(m_player);
		}

		time = GetRecordCorrectedTime(time);

		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		if (m_recorder)
		{
			m_recorder->Update(time);
		}


		EffectManager::getSingleton().Update(time);

		m_console->Update(time);

		UIRoot::Update(time);

		ResourceManager::PerformAllPostSync(time->getElapsedRealTime());
	}

	const GameTime* Game::GetRecordCorrectedTime(const GameTime* time)
	{
		if (m_player)
		{
			m_player->Update(time, m_correctedFrameTime);
			return &m_correctedFrameTime;
		}
		return time;
	}
}