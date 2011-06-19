#ifndef MESHBUILD_H
#define MESHBUILD_H

#include "APBCommon.h"

using namespace Apoc3D;
using namespace Apoc3D::Config;

namespace APBuild
{
	class MeshBuild
	{
	public:
		void Build(const ConfigurationSection* sect);
	};
}

#endif