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

#include "AppTime.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Core
	{
		AppTime::AppTime() { }
		AppTime::~AppTime() { }

		AppTime::AppTime(float elapsedRTime, float fps)
			: ElapsedTime(elapsedRTime), ElapsedRealTime(elapsedRTime), FPS(fps) 
		{ }

		AppTime::AppTime(float elapsedTime, float elapsedRTime, float elapsedTimeSubstep, int32 iteractionCount, float fps, bool isRenderingSlow)
			: ElapsedTime(elapsedTime), ElapsedTimeSubstep(elapsedTimeSubstep),
			ElapsedRealTime(elapsedRTime), IterationCount(iteractionCount),
			FPS(fps), IsRenderingSlow(isRenderingSlow) 
		{ }


		void AppTime::Write(BinaryWriter* bw)
		{
			bw->WriteSingle(ElapsedTime);
			bw->WriteSingle(ElapsedRealTime);
			bw->WriteSingle(ElapsedTimeSubstep);
			bw->WriteInt32(IterationCount);
			bw->WriteSingle(FPS);
			bw->WriteBoolean(IsRenderingSlow);
		}

		void AppTime::Read(BinaryReader* br)
		{
			ElapsedTime = br->ReadSingle();
			ElapsedRealTime = br->ReadSingle();
			ElapsedTimeSubstep = br->ReadSingle();
			IterationCount = br->ReadInt32();
			FPS = br->ReadSingle();
			IsRenderingSlow = br->ReadBoolean();
		}
	}
}