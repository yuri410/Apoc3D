#pragma once
#ifndef APOC3D_APPTIME_H
#define APOC3D_APPTIME_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/ApocCommon.h"

namespace Apoc3D
{
	namespace Core
	{
		/** Contains the current timing state of the aplication. */
		struct APAPI AppTime
		{
			AppTime();
			AppTime(float elapsedRTime, float fps);
			AppTime(float elapsedTime, float elapsedRTime, float elapsedTimeSubstep, int32 iteractionCount, float fps, bool isRenderingSlow);
			~AppTime();

			/**
			 * The elapsed game time since last update, in seconds.
			 *
			 * In fixed time step system, ElapsedTime is always the intended TargetElapsedTime,
			 * and Update is called 1 or more times to catch up with rendering overhead.
			 *
			 * Otherwise this is same as ElapsedRealTime.
			 */
			float ElapsedTime = 0;

			/**
			 * The elapsed real game time since last update, in seconds.
			 *
			 * This "real" version of ElapsedTime is intended for to get the real ElapsedTime instead of the TargetElapsedTime
			 * when fixed time step is used.
			 *
			 * In fixed time step system, Update() may be called more than one while Render() is only invoked once in a frame.
			 * ElapsedTime is always the intended TargetElapsedTime, and Update is called 1 or more times to catch up with rendering.
			 *
			 */
			float ElapsedRealTime = 0;

			float ElapsedTimeSubstep = 0;

			/** The number of iterations in a fixed time step system. */
			int32 IterationCount = 1;

			float FPS = 0;

			bool IsRenderingSlow = false;


			void Write(IO::BinaryWriter* bw);
			void Read(IO::BinaryReader* br);
		};
	};
};

#endif