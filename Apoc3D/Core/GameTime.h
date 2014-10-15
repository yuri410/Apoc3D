#pragma once
#ifndef APOC3D_GAMETIME_H
#define APOC3D_GAMETIME_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/**
		 *  Contains the current timing state of the game.
		 */
		struct GameTime
		{
			GameTime(float elapsedTime, float totalTime, float elapsedRTime, float totalRTime, 
				float fps, bool isRenderingSlow)
				: ElapsedTime(elapsedTime), TotalTime(totalTime), 
				ElapsedRealTime(elapsedRTime),TotalRealTime(TotalRealTime), 
				FPS(fps), IsRenderingSlow(isRenderingSlow) { }

			~GameTime() { }

			/**
			*  Gets the amount of game time since the start of the game, in seconds.
			*/
			float getTotalTime() const { return TotalTime; }
			/**
			*  Gets 
			*/
			float getElapsedTime() const { return ElapsedTime; }

			float getTotalRealTime() const { return TotalRealTime; }
			
			float getElapsedRealTime() const { return ElapsedRealTime; }

			float getFPS() const { return FPS; }

			/**
			 * The elapsed game time since last update, in seconds.
			 *
			 * In fixed time step system, ElapsedTime is always the intended TargetElapsedTime,
			 * and Update is called 1 or more times to catch up with rendering.
			 *
			 * Otherwise this is same as ElapsedRealTime.
			 */
			float ElapsedTime;
			float TotalTime;

			/**
			 * The elapsed real game time since last update, in seconds.
			 *
			 * This "real" version of ElapsedTime is intended for use in fixed time system configurations 
			 * to get the real ElapsedTime instead of the TargetElapsedTime.
			 *
			 * In fixed time step system, Update() may be called more than one while Render() is only invoked once in a frame.
			 * ElapsedTime is always the intended TargetElapsedTime, and Update is called 1 or more times to catch up with rendering.
			 *
			 */
			float ElapsedRealTime;
			float TotalRealTime;

			float FPS;

			bool IsRenderingSlow = false;

		};
	};
};

#endif