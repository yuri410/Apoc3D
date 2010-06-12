#include "Game.h"
#include "GameTime.h"

namespace Apoc3D
{
	Game::Game(HINSTANCE instance)
		: maxElapsedTime(0.5f)
	{
		time = new GameTime();
	}


	Game::~Game(void)
	{
	}
}