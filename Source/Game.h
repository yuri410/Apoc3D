
#ifndef GAME_H
#define GAME_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	class _Export Game
	{
	private:
		const GameTime* time;
		
		HINSTANCE hInst;


		float maxElapsedTime;
		float totalGameTime;
		float accumulatedElapsedGameTime;
		float lastFrameElapsedGameTime;
		float lastFrameElapsedRealTime;

	public:
		Game(HINSTANCE instance);
		~Game(void);
	};

}

#endif