
#include "MeshBuild.h"

#include "Config/ConfigurationSection.h"
#include "BuildConfig.h"
#include "AIImporter.h"
#include "IOLib/Streams.h"
#include "IOLib/ModelData.h"
#include "CompileLog.h"
#include "Vfs/File.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	void MeshBuild::BuildByFBX(const MeshBuildConfig& config)
	{

	}
	void MeshBuild::BuildByASS(const MeshBuildConfig& config)
	{
		AIImporter importer;
		ModelData* data = importer.Import(config);

		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);


		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}

	void MeshBuild::Build(const ConfigurationSection* sect)
	{
		MeshBuildConfig config;
		config.Parse(sect);
		
		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Can not file input file.");
			return;
		}

		switch (config.Method)
		{
		case MESHBUILD_ASS:
			BuildByASS(config);
			break;
		case MESHBUILD_FBX:
			BuildByFBX(config);
			break;
		}
	}
}