#ifndef AIIMPORTER_H
#define AIIMPORTER_H

#include "APBCommon.h"
#include "Collections/FastList.h"
#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "IOLib/MaterialData.h"
#include "Graphics/Animation/AnimationTypes.h"
#include "BuildConfig.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::Animation;

namespace APBuild
{
	class AIImporter
	{
	public:
		ModelData* Import(const MeshBuildConfig& config);
	};
}


#endif