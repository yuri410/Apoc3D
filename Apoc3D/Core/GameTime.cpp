#include "GameTime.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Core
	{
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