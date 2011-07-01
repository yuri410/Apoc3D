
#include "MeshBuild.h"

#include "Config/ConfigurationSection.h"
#include "BuildConfig.h"
#include "AIImporter.h"
#include "IOLib/Streams.h"
#include "IOLib/ModelData.h"
#include "CompileLog.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	void MeshBuild::Build(const ConfigurationSection* sect)
	{
		MeshBuildConfig config;
		config.Parse(sect);

		AIImporter importer;
		ModelData* data = importer.Import(config);
		
		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);


		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");

	}
}