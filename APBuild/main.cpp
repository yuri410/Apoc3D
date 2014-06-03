// APBuild.cpp : 定义控制台应用程序的入口点。
//

#include <SDKDDKVer.h>

#include <Windows.h>
#include <tchar.h>

#include "BuildEngine.h"

#include "APBCommon.h"

#include "ErrorCode.h"
#include "CompileLog.h"

#include "MaterialStub.h"
#include "Border/BorderBuilder.h"
#include "TextureBuild/TextureBuild.h"
#include "FontBuild/FontBuild.h"
#include "MeshBuild/MeshBuild.h"
#include "PakBuild/PakBuild.h"
#include "MaterialScript/MaterialBuild.h"
#include "EffectCompiler/AFXBuild.h"
#include "EffectCompiler/CFXBuild.h"
#include "EffectCompiler/FXListBuild.h"
#include "AnimationBuild/MAnimBuild.h"
#include "AnimationBuild/TAnimBuild.h"

#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/Streams.h"

#include <direct.h>
#include <iostream>

using namespace std;
using namespace APBuild;
using namespace Apoc3D;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;


int Build(ConfigurationSection* sect);
int Build(int argc, _TCHAR* argv[]);
void PrintUsage();

int RunSpecialBuild(const String& type, List<std::pair<String, String>>& params);

int CollectBuildIssues();

// This APBuild program may be executed by Apoc3D Designer multiple time
// base on the dependency of project items. For instance, a first time build 
// could used to process basic textures and models. Then a second
// build will be called to pack them into archive files.
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, ".ACP");

	Initialize();

	int r = Build(argc, argv);

	Finalize();
	
	fflush(stdout);

	return r;
}

int Build(int argc, _TCHAR* argv[])
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
						CompileLog::WriteError(L"Invalid Parameter: " + a, L"Command line");
						break;
					}
				}

				int ret = Build(sect);
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
			Configuration* config = XMLConfigurationFormat::Instance.Load(&fl);

			// find the first section in the build config
			Configuration::ChildTable::Enumerator e = config->GetEnumerator();
			e.MoveNext();
		
			// build the node
			int ret = Build(*e.getCurrentValue());
			delete config;

			return ret;
		}
	}
	else PrintUsage();

	return 0;
}

int BuildDirectCopy(ConfigurationSection* sect);

int Build(ConfigurationSection* sect)
{
	String buildType = sect->getAttribute(L"Type");
	wcout << L"Building ";
	wcout << buildType;
	wcout << L"...\n";

	StringUtils::ToLowerCase(buildType);

	if (buildType == L"material")
	{
		MaterialStub::Build(sect);
	}
	else if (buildType == L"materialset")
	{
		MaterialBuild::Build(sect);
	}
	else if (buildType == L"texture")
	{
		TextureBuild::Build(sect);
	}
	else if (buildType == L"mesh")
	{
		MeshBuild::Build(sect);
	}
	else if (buildType == L"effect")
	{
		AFXBuild::Build(sect);
	}
	else if (buildType == L"customeffect")
	{
		CFXBuild::Build(sect);
	}
	else if (buildType == L"font")
	{
		fbBuild(sect);
	}
	else if (buildType == L"fontcheck")
	{
		fbBuildGlyphCheck(sect);
	}
	else if (buildType == L"fontmap")
	{
		fbBuildToFontMap(sect);
	}
	else if (buildType == L"fontmapst")
	{
		fbBuildFromFontMap(sect);
	}
	else if (buildType == L"tanim")
	{
		TAnimBuild::Build(sect);
	}
	else if (buildType == L"manim")
	{
		MAnimBuild::Build(sect);
	}
	else if (buildType == L"uilayout")
	{

	}
	else if (buildType == L"project" || buildType == L"folder")
	{
		// If the node is a project or folder node, building sub-nodes recursively
		for (ConfigurationSection::SubSectionEnumerator iter =  sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			ConfigurationSection* item = *iter.getCurrentValue();

			Build(item);
		}
	}
	else if (buildType == L"pak")
	{
		PakBuild::Build(sect);
	}
	else if (buildType == L"projectfxlist")
	{
		FXListBuild::Build(sect);
	}
	else if (buildType == L"border")
	{
		BorderBuilder::Build(sect);
	}
	else if (buildType == L"copy")
	{
		BuildDirectCopy(sect);
	}
	else
	{
		return ERR_UNSUPPORTED_BUILD;
	}

	return CollectBuildIssues();
}

int BuildDirectCopy(ConfigurationSection* sect)
{
	String sourceFile = sect->getAttribute(L"SourceFile");
	String destFile = sect->getAttribute(L"DestinationFile");

	if (!File::FileExists(sourceFile))
	{
		CompileLog::WriteError(sourceFile, L"Could not find source file.");
		return 1;
	}
	EnsureDirectory(PathUtils::GetDirectory(destFile));

	FileStream* input = new FileStream(sourceFile);
	FileOutStream* output = new FileOutStream(destFile);

	bool finished = false;
	while (!finished)
	{
		char buffer[4096];
		int64 count = input->Read(buffer, 4096);
		output->Write(buffer, count);

		finished |= count < 4096;
	}

	delete input;
	delete output;

	CompileLog::WriteInformation(sect->getName(), L"> Copied: ");
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

int CollectBuildIssues()
{
	bool thereAreWarnings = false;
	bool thereAreErrors = false;
	for (int32 i=0;i<CompileLog::Logs.getCount();i++)
	{
		switch (CompileLog::Logs[i].Type)
		{
		case COMPILE_Warning:
			wcout << L"[Warning]";
			thereAreWarnings = true;
			break;
		case COMPILE_Information:

			break;
		case COMPILE_Error:
			wcout << L"[Error]";
			thereAreErrors = true;
			break;
		}
		wcout << CompileLog::Logs[i].Location;
		wcout << L" : ";
		wcout << CompileLog::Logs[i].Description;
		wcout << L"\n";
	}

	CompileLog::Clear();
	if (thereAreErrors)
		return ERR_THERE_ARE_ERRORS;
	if (thereAreWarnings)
		return ERR_THERE_ARE_WARNINGS;
	return 0;
}