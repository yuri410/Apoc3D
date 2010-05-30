#pragma once

namespace Apoc3D
{
	class _Export GameTime
	{
	private:
		float m_totalTime;
		float m_elapsedTime;

	public:
		const float getTotalTime() { return m_totalTime; }
		const float getElapsedTime() { return m_elapsedTime; }
	
		//void setTotalTime(const float value) { m_totalTime = value; }
		//void setElapsedTime(const float value) { m_elapsedTime = value; }


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
