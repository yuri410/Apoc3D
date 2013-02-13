#include "TAnimBuild.h"

#include "../BuildEngine.h"
#include "../BuildConfig.h"
#include "../CompileLog.h"
#include "XafImporter.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void TAnimBuild::Build(const ConfigurationSection* sect)
	{
		TransformAnimBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find source file.");
			return;
		}
		if (config.ObjectIndexMapping.getCount()==0)
		{
			CompileLog::WriteError(config.SrcFile, L"The t-anim build config does not have object index mapping.");
			return;
		}
		EnsureDirectory(PathUtils::GetDirectory(config.DstFile));

		XafImporter importer;
		AnimationData* data = importer.Import(config);
		if (!data)
		{
			CompileLog::WriteError(config.SrcFile, L"Unknown error.");
			return;
		}

		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);

		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
}