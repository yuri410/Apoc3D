/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#include "BuildSystem.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#include "Utils/D3DHelper.h"
#include <GdiPlus.h>

#include <iostream>

#include "Border/BorderBuilder.h"
#include "TextureBuild/TextureBuild.h"
#include "FontBuild/FontBuild.h"
#include "MeshBuild/MeshBuild.h"
#include "PakBuild/PakBuild.h"
#include "MaterialScript/MaterialBuild.h"
#include "MaterialScript/MaterialStub.h"
#include "EffectCompiler/AFXBuild.h"
#include "EffectCompiler/CFXBuild.h"
#include "EffectCompiler/FXListBuild.h"
#include "AnimationBuild/MAnimBuild.h"
#include "AnimationBuild/TAnimBuild.h"

#include "ErrorCode.h"

using namespace Gdiplus;
using namespace std;

static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR           gdiplusToken;

namespace APBuild
{
	namespace BuildSystem
	{
		struct CompileLogEntry
		{
			BuildSystem::CompileLogType Type;
			String Location;
			String Description;
		};
		List<CompileLogEntry> logs;


		int Initialize()
		{
			if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
				iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
				ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
			{
				wcout << L"DevIL version is different...!\n";
				return 1;
			}

			ilInit();
			iluInit();

			Utils::D3DHelper::Initalize();


			// Initialize GDI+.
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


			LogManager::Initialize();
			LogManager::getSingleton().WriteLogToStd = true;

			return 0;
		}
		void Finalize()
		{
			GdiplusShutdown(gdiplusToken);
			LogManager::Finalize();
			Utils::D3DHelper::Finalize();
		}

		void EnsureDirectory(const String& path)
		{
			List<String> paths = PathUtils::Split(path);
			String subPath;
			subPath.reserve(path.size());
			for (int32 i=0;i<paths.getCount();i++)
			{
				PathUtils::Append(subPath, paths[i]);

				if (!File::DirectoryExists(subPath))
				{
					CreateDirectory(subPath.c_str(), 0);
				}
			}
		}

		int BuildDirectCopy(ConfigurationSection* sect);
		int CollectBuildIssues();

		int Build(ConfigurationSection* sect)
		{
			String buildType = sect->getAttribute(L"Type");
			wcout << L"Building ";
			wcout << buildType;
			wcout << L"...\n";

			StringUtils::ToLowerCase(buildType);

			if (buildType == L"fontmap")
			{
				FontBuild::BuildToFontMap(sect);
			}
			else if (buildType == L"fontmapst")
			{
				FontBuild::BuildFromFontMap(sect);
			}
			else if (buildType == L"pak")
			{
				PakBuild::Build(sect);
			}
			else if (buildType == L"border")
			{
				BorderBuilder::Build(sect);
			}
			else if (buildType == L"project" || buildType == L"folder")
			{
				// If the node is a project or folder node, building sub-nodes recursively
				for (ConfigurationSection::SubSectionEnumerator iter =  sect->GetSubSectionEnumrator();
					iter.MoveNext();)
				{
					ConfigurationSection* item = iter.getCurrentValue();

					Build(item);
				}
			}
			else if (ProjectTypeUtils::SupportsProjectItemType(buildType))
			{
				ProjectItemType pit = ProjectTypeUtils::ParseProjectItemType(buildType);

				switch (pit)
				{
				case ProjectItemType::Custom:
					break;
				case ProjectItemType::Folder:
					break;
				case ProjectItemType::Material:
					MaterialStub::Build(sect);
					break;
				case ProjectItemType::MaterialSet:
					MaterialBuild::Build(sect);
					break;
				case ProjectItemType::Texture:
					TextureBuild::Build(sect);
					break;
				case ProjectItemType::Model:
					MeshBuild::Build(sect);
					break;
				case ProjectItemType::TransformAnimation:
					TAnimBuild::Build(sect);
					break;
				case ProjectItemType::MaterialAnimation:
					MAnimBuild::Build(sect);
					break;
				case ProjectItemType::Effect:
					AFXBuild::Build(sect);
					break;
				case ProjectItemType::EffectList:
					FXListBuild::Build(sect);
					break;
				case ProjectItemType::CustomEffect:
					CFXBuild::Build(sect);
					break;
				case ProjectItemType::ShaderNetwork:
					break;
				case ProjectItemType::Font:
					FontBuild::Build(sect);
					break;
				case ProjectItemType::FontGlyphDist:
					FontBuild::BuildGlyphAvailabilityRanges(sect);
					break;
				case ProjectItemType::UILayout:
					break;
				case ProjectItemType::Copy:
					BuildDirectCopy(sect);
					break;
				default:
					assert(0);
				}
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
				LogError(sourceFile, L"Could not find source file.");
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

			LogInformation(sect->getName(), L"> Copied: ");
			return 0;
		}

		int CollectBuildIssues()
		{
			bool thereAreWarnings = false;
			bool thereAreErrors = false;
			for (int32 i=0;i<logs.getCount();i++)
			{
				switch (logs[i].Type)
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
				wcout << logs[i].Location;
				wcout << L" : ";
				wcout << logs[i].Description;
				wcout << L"\n";
			}

			LogClear();
			if (thereAreErrors)
				return ERR_THERE_ARE_ERRORS;
			if (thereAreWarnings)
				return ERR_THERE_ARE_WARNINGS;
			return 0;
		}

		void Log(CompileLogType type, const String& message, const String& location)
		{
			CompileLogEntry ent = { type, location, message };
			logs.Add(ent);
		}
		void LogInformation(const String& message, const String& location) { Log(COMPILE_Information, message, location); }
		void LogError(const String& message, const String& location) { Log(COMPILE_Error, message, location); }
		void LogWarning(const String& message, const String& location) { Log(COMPILE_Warning, message, location); }
		void LogClear() { logs.Clear(); }
	}
}

