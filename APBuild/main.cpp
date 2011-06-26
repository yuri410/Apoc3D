// APBuild.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BuildEngine.h"

#include "APBCommon.h"

#include "ErrorCode.h"
#include "Config/XmlConfiguration.h"
#include "Config/ConfigurationSection.h"
#include "Vfs/File.h"
#include "Vfs/ResourceLocation.h"
#include "Utility/StringUtils.h"



using namespace std;
using namespace Apoc3D;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;

int Build(int argc, _TCHAR* argv[])
{
	if (argc)
	{
		if (!File::FileExists(argv[0]))
		{
			return ERR_CONFIG_FILE_NOT_FOUND;
		}
		FileLocation* fl = new FileLocation(argv[0]);
		XMLConfiguration* config = new XMLConfiguration(fl);

		String buildType = config->get(L"BuildType")->getAttribute(L"Value");

		StringUtils::ToLowerCase(buildType);

		if (buildType == L"texture")
		{

		}
		else
		{
			return ERR_UNSUPPORTED_BUILD;
		}
	}
	else
	{
		cout << "Usage: APBuild [ConfigFile]\n";
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Initialize();

	int r = Build(argc, argv);

	Finalize();
	return r;
}

