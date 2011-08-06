// APBuild.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BuildEngine.h"

#include "APBCommon.h"

#include <direct.h>

#include "ErrorCode.h"
#include "Config/XmlConfiguration.h"
#include "Config/ConfigurationSection.h"
#include "Vfs/File.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"
#include "Utility/StringUtils.h"
#include "CompileLog.h"

#include "TextureBuild/TextureBuild.h"
#include "FontBuild/FontBuild.h"
#include "MeshBuild/MeshBuild.h"
#include "PakBuild/PakBuild.h"

//#include <Windows.h>

using namespace std;
using namespace APBuild;
using namespace Apoc3D;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;


int Build(ConfigurationSection* sect)
{
	String buildType = sect->getAttribute(L"Type");
	wcout << L"Building ";
	wcout << buildType;
	wcout << L"...\n";

	StringUtils::ToLowerCase(buildType);

	if (buildType == L"texture")
	{
		TextureBuild::Build(sect);
	}
	else if (buildType == L"mesh")
	{
		MeshBuild::Build(sect);
	}
	else if (buildType == L"effect")
	{

	}
	else if (buildType == L"font")
	{
		FontBuild::Build(sect);
	}
	else if (buildType == L"pak")
	{
		PakBuild::Build(sect);
	}
	else if (buildType == L"uilayout")
	{

	}
	else if (buildType == L"project")
	{
		for (ConfigurationSection::SubSectionIterator iter =  sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			ConfigurationSection* item = iter->second;

			Build(item);
		}
	}
	else
	{
		return ERR_UNSUPPORTED_BUILD;
	}
	
	for (size_t i=0;i<CompileLog::Logs.size();i++)
	{
		switch (CompileLog::Logs[i].Type)
		{
		case COMPILE_Warning:
			wcout << L"[Warning]";
			break;
		case COMPILE_Information:

			break;
		case COMPILE_Error:
			wcout << L"[Error]";
			break;
		}
		wcout << CompileLog::Logs[i].Location;
		wcout << L" : ";
		wcout << CompileLog::Logs[i].Description;
		wcout << L"\n";
	}

	CompileLog::Clear();
	return 0;
}

int Build(int argc, _TCHAR* argv[])
{
	if (argc>1)
	{
		String basePath = argv[0];

		basePath = PathUtils::GetDirectory(basePath);

		_chdir(StringUtils::toString(basePath).c_str());

		String configPath = argv[1];

		if (!File::FileExists(configPath))
		{
			wcout << L"Build file: ";
			wcout << configPath;
			wcout << L" does not exist.";
			return ERR_CONFIG_FILE_NOT_FOUND;
		}
		FileLocation* fl = new FileLocation(configPath);
		XMLConfiguration* config = new XMLConfiguration(fl);

		ConfigurationSection* sect = config->get(L"Build");
		return Build(sect);
	}
	else
	{
		cout << "Usage: APBuild [ConfigFile]\n";
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Initialize();

	int r = 0;

	try
	{
		r = Build(argc, argv);
	}
	catch (const Apoc3DException& e)
	{
		
	}
	
	Finalize();

#ifdef _DEBUG
	getchar();
#endif
	return r;
}

