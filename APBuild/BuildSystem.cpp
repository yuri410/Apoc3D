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
			
			String DirectMessage;
		};
		List<CompileLogEntry> logs;
		String outputRelativeBase;

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
			for (int32 i = 0; i < paths.getCount(); i++)
			{
				PathUtils::Append(subPath, paths[i]);

				if (!File::DirectoryExists(subPath))
				{
					CreateDirectory(subPath.c_str(), 0);
				}
			}
		}

		int BuildDirectCopy(const String& hierarchyPath, ConfigurationSection* sect);
		int CollectBuildIssues();

		static String previousBuildType;

		int Build(const String& hierarchyPath, ConfigurationSection* sect)
		{
			String buildType = sect->getAttribute(L"Type");
			if (!StringUtils::EqualsNoCase(buildType, previousBuildType))
			{
				previousBuildType = buildType;
				wcout << L"Building ";
				wcout << buildType;
				wcout << L"...\n";
			}
			

			StringUtils::ToLowerCase(buildType);

			if (buildType == L"fontmap")
			{
				FontBuild::BuildToFontMap(hierarchyPath, sect);
			}
			else if (buildType == L"fontmapst")
			{
				FontBuild::BuildFromFontMap(hierarchyPath, sect);
			}
			else if (buildType == L"pak")
			{
				PakBuild::Build(hierarchyPath, sect);
			}
			else if (buildType == L"border")
			{
				BorderBuilder::Build(hierarchyPath, sect);
			}
			else if (buildType == L"project" || buildType == L"folder")
			{
				// If the node is a project or folder node, building sub-nodes recursively
				for (ConfigurationSection* item : sect->getSubSections())
				{
					String subPath = PathUtils::Combine(hierarchyPath, item->getName());
					Build(subPath, item);
				}
			}
			else if (ProjectUtils::ProjectItemTypeConv.SupportsName(buildType))
			{
				ProjectItemType pit = ProjectUtils::ProjectItemTypeConv.Parse(buildType);

				switch (pit)
				{
				case ProjectItemType::Custom:
					break;
				case ProjectItemType::Folder: break;
				
				case ProjectItemType::Material:
					MaterialStub::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::MaterialSet:
					MaterialBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::Texture:
					TextureBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::Model:
					MeshBuild::Build(hierarchyPath, sect);
					break;

				case ProjectItemType::TransformAnimation:
					TAnimBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::MaterialAnimation:
					MAnimBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::Effect:
					AFXBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::EffectList:
					FXListBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::CustomEffect:
					CFXBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::ShaderNetwork:
					break;
				case ProjectItemType::Font:
					FontBuild::Build(hierarchyPath, sect);
					break;
				case ProjectItemType::UILayout:
					break;
				case ProjectItemType::Copy:
					BuildDirectCopy(hierarchyPath, sect);
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

		int Build(ConfigurationSection* sect, ConfigurationSection* attachmentSect)
		{
			previousBuildType.clear();

			if (attachmentSect)
			{
				String baseOutputPath;
				attachmentSect->tryGetValue(L"BaseOutputPath", baseOutputPath);
				SetLoggingOutputPathRelativeBase(baseOutputPath);
			}

			return Build(sect->getName(), sect);
		}

		int BuildDirectCopy(const String& hierarchyPath, ConfigurationSection* sect)
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

			LogEntryProcessed(destFile, hierarchyPath, L"Copied");

			return 0;
		}

		int CollectBuildIssues()
		{
			bool thereAreWarnings = false;
			bool thereAreErrors = false;
			for (const CompileLogEntry& cle : logs)
			{
				switch (cle.Type)
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

				if (cle.DirectMessage.size())
				{
					wcout << cle.DirectMessage;
					wcout << L"\n";
				}
				else
				{
					wcout << cle.Location;
					wcout << L" : ";
					wcout << cle.Description;
					wcout << L"\n";
				}
			}
			wcout.flush();

			LogClear();
			if (thereAreErrors)
				return ERR_THERE_ARE_ERRORS;
			if (thereAreWarnings)
				return ERR_THERE_ARE_WARNINGS;
			return 0;
		}

		void SetLoggingOutputPathRelativeBase(const String& basePath)
		{
			outputRelativeBase = basePath;
		}

		void Log(CompileLogType type, const String& message, const String& location)
		{
			CompileLogEntry ent = { type, location, message };
			logs.Add(ent);
		}

		void LogEntryProcessed(const String& destFile, const String& virtualItemPath, const String& prefix)
		{
			String dm = L"> " + prefix;
			if (prefix.size())
				dm.append(1, ' ');
			dm.append(virtualItemPath);
			dm.append(L"  =>  ");

			if (outputRelativeBase.size())
			{
				String temp;
				PathUtils::GetRelativePath(outputRelativeBase, destFile, temp);
				dm.append(temp);
			}
			else
			{
				dm.append(destFile);
			}

			CompileLogEntry ent = { COMPILE_Information, L"", L"", dm };
			logs.Add(ent);
		}

		void LogInformation(const String& message, const String& location) { Log(COMPILE_Information, message, location); }
		void LogError(const String& message, const String& location) { Log(COMPILE_Error, message, location); }
		void LogWarning(const String& message, const String& location) { Log(COMPILE_Warning, message, location); }
		void LogClear() { logs.Clear(); }
	}
}

