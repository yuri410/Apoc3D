// APBuild.cpp : 定义控制台应用程序的入口点。
//

#include <SDKDDKVer.h>

#include <Windows.h>
#include <tchar.h>

#include "BuildSystem.h"

#include "APBCommon.h"

#include "ErrorCode.h"

#include <direct.h>
#include <iostream>

using namespace std;
using namespace APBuild;

int PrepareBuild(int argc, _TCHAR* argv[]);
void PrintUsage();

// This APBuild program may be executed by Apoc3D Designer multiple time
// base on the dependency of project items. For instance, a first time build 
// could used to process basic textures and models. Then a second
// build will be called to pack them into archive files.
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, ".ACP");

	BuildSystem::Initialize();

	int r = PrepareBuild(argc, argv);

	BuildSystem::Finalize();
	
	fflush(stdout);

	return r;
}

int PrepareBuild(int argc, _TCHAR* argv[])
{
	if (argc>1)
	{
		String basePath = argv[0];
		basePath = PathUtils::GetDirectory(basePath);

		_chdir(StringUtils::toPlatformNarrowString(basePath).c_str());
		
		if (argv[1] == L"/d")
		{
			if (argc>3)
			{
				ConfigurationSection* sect = new ConfigurationSection(L"BuildItem");
				sect->AddAttributeString(L"Type", argv[2]);

				for (int i=3;i<argc;i++)
				{
					String a = argv[i];
					String::size_type pos = a.find_first_of('=');
					if (pos != String::npos)
					{
						String key = a.substr(0, pos);
						String value = a.substr(pos+1);
						sect->AddAttributeString(key, value);
					}
					else
					{
						BuildSystem::LogError(L"Invalid Parameter: " + a, L"Command line");
						break;
					}
				}

				int ret = BuildSystem::Build(sect);
				delete sect;
				return ret;
			}
			else PrintUsage();
		}
		else
		{
			String configPath = argv[1];

			if (!File::FileExists(configPath))
			{
				wcout << L"Build file: ";
				wcout << configPath;
				wcout << L" does not exist.";
				return ERR_CONFIG_FILE_NOT_FOUND;
			}

			FileLocation fl(configPath);
			Configuration* config = XMLConfigurationFormat::Instance.Load(fl);

			// find the first section in the build config
			Configuration::ChildTable::Enumerator e = config->GetEnumerator();
			e.MoveNext();
		
			// build the node
			int ret = BuildSystem::Build(*e.getCurrentValue());
			delete config;

			return ret;
		}
	}
	else PrintUsage();

	return 0;
}

void PrintUsage()
{
	cout << "Apoc3D Build. "; cout << " Software Date: "; cout << __DATE__; cout << "\n";

	cout << "Usage: APBuild [Options]\n";
	cout << "  Options:\n";
	cout << "    /d [Type] [Params]    Directly build from command line.\n";
	cout << "    [ConfigFile]          Build from project build configuration.\n";
}
