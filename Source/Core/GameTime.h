/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#ifndef GAMETIME_H
#define GAMETIME_H

#include "Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/* Contains the current timing state of the game.
		*/
		class APAPI GameTime
		{
		private:
			float m_totalTime;
			float m_elapsedTime;

			float m_totalRealTime;
			float m_elapsedRealTime;
			float m_fps;

			bool m_slowly;
		public:
			/*  Gets the total game time, in seconds.
			*/
			float getTotalTime() const { return m_totalTime; }
			/* Gets the elapsed game time, in seconds.
			*/
			float getElapsedTime() const { return m_elapsedTime; }
	
			float getTotalRealTime() const { return m_totalRealTime; }

			float getElapsedRealTime() const { return m_elapsedRealTime; }

			float getFPS() const { return m_fps; }

			bool getIsRunningSlowly() const { return m_slowly; }

			GameTime(const float elapsedTime, const float totalTime,
				const float elapsedRTime, const float totalRTime,
				const float fps, const bool isRunningSlowly)
			{
				m_totalTime = totalTime;
				m_elapsedTime = elapsedTime;
				m_totalRealTime = totalRTime;
				m_elapsedRealTime = elapsedRTime;
		
				m_fps = fps;
				m_slowly = isRunningSlowly;
			}

			~GameTime(void)
			{
			}
		};
	};
};

#endif