
#ifndef GAME_H
#define GAME_H

#pragma once

#include "Common.h"

namespace Apoc3D
{
	class _Export Game
	{
	private:
		HINSTANCE m_inst;

		float m_maxElapsedTime;
		float m_totalGameTime;
		float m_accumulatedElapsedGameTime;
		float m_lastFrameElapsedGameTime;
		float m_lastFrameElapsedRealTime;

		GraphicsDeviceManager* m_graphicsDeviceManager;
		GameWindow* m_gameWindow;

	public:
		Game(HINSTANCE instance, int nCmdShow, const wchar_t* const &name);
		~Game(void);
	};

}

#endif