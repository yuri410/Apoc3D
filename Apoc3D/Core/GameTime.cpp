#include "GameTime.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Core
	{
		GameTime::GameTime() { }
		GameTime::~GameTime() { }

		GameTime::GameTime(float elapsedRTime, float fps)
			: ElapsedTime(elapsedRTime), ElapsedRealTime(elapsedRTime), FPS(fps) 
		{ }

		GameTime::GameTime(float elapsedTime, float elapsedRTime, float elapsedTimeSubstep, int32 iteractionCount, float fps, bool isRenderingSlow)
			: ElapsedTime(elapsedTime), ElapsedTimeSubstep(elapsedTimeSubstep),
			ElapsedRealTime(elapsedRTime), IterationCount(iteractionCount),
			FPS(fps), IsRenderingSlow(isRenderingSlow) 
		{ }


		void GameTime::Write(BinaryWriter* bw)
		{
			bw->WriteSingle(ElapsedTime);
			bw->WriteSingle(ElapsedRealTime);
			bw->WriteSingle(ElapsedTimeSubstep);
			bw->WriteInt32(IterationCount);
			bw->WriteSingle(FPS);
			bw->WriteBoolean(IsRenderingSlow);
		}

		void GameTime::Read(BinaryReader* br)
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