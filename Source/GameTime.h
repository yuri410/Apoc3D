#pragma once

namespace Apoc3D
{
	/*
	 Contains the current timing state of the game.
	*/
	class _Export GameTime
	{
	private:
		float m_totalTime;
		float m_elapsedTime;

	public:
		/*
		 Gets the total game time, in seconds.
		*/
		const float getTotalTime() { return m_totalTime; }
		/*
		 Gets the elapsed game time, in seconds.
		*/
		const float getElapsedTime() { return m_elapsedTime; }
	

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
