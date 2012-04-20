#ifndef MESHBUILD_H
#define MESHBUILD_H

#include "APBCommon.h"

using namespace Apoc3D;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;

namespace APBuild
{
	class MeshBuild
	{
	private:
		static void BuildByASS(const MeshBuildConfig& config);
		static void BuildByFBX(const MeshBuildConfig& config);
		static void BuildByD3D(const MeshBuildConfig& config);
	public:
		static void Build(const ConfigurationSection* sect);

		static void ConvertVertexData(ModelData* data, const MeshBuildConfig& config);
		static void CollapseMeshs(ModelData* data, const MeshBuildConfig& config);
	};

	class TAnimBuild
	{
	public:
		static void Build(const ConfigurationSection* sect);
	};
}

#endif