#include "TAnimBuild.h"

#include "BuildSystem.h"
#include "BuildConfig.h"
#include "XafImporter.h"

namespace APBuild
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void TAnimBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		TransformAnimBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SrcFile))
		{
			BuildSystem::LogError(config.SrcFile, L"Could not find source file.");
			return;
		}
		if (config.ObjectIndexMapping.getCount()==0)
		{
			BuildSystem::LogError(config.SrcFile, L"The t-anim build config does not have object index mapping.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DstFile));

		AnimationData* data = XafImporter::Import(config);
		if (data == nullptr)
		{
			BuildSystem::LogError(config.SrcFile, L"Unknown error.");
			return;
		}

		data->Save(FileOutStream(config.DstFile));

		delete data;

		BuildSystem::LogEntryProcessed(config.DstFile, hierarchyPath);
	}
}