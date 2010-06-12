#include "Game.h"
#include "GameTime.h"
#include "Graphics\GraphicsDeviceManager.h"
#include "GameWindow.h"

using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	Game::Game(HINSTANCE instance, int nCmdShow, const wchar_t* const &name)
		: m_maxElapsedTime(0.5f)
	{
		
		m_graphicsDeviceManager = new GraphicsDeviceManager(this);
	
		m_gameWindow = new GameWindow(instance, nCmdShow, name, name);
	}


	Game::~Game(void)
	{
	}
}