#ifndef GAMETIME_H
#define GAMETIME_H

#pragma once
#include "Common.h"

namespace Apoc3D
{
	/* Contains the current timing state of the game.

	   
	*/
	class _Export GameTime
	{
	private:
		float m_totalTime;
		float m_elapsedTime;

	public:
		/*  Gets the total game time, in seconds.
		*/
		float getTotalTime() const { return m_totalTime; }
		/* Gets the elapsed game time, in seconds.
		*/
		float getElapsedTime() const { return m_elapsedTime; }
	

		GameTime(const float elapsedTime, const float totalTime)
		{
			m_totalTime = totalTime;
			m_elapsedTime = elapsedTime;
		}

		~GameTime(void)
		{
		}
	};
};

#endif