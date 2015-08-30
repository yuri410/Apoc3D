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
#include "Project.h"
#include "apoc3d/Config/ConfigurationManager.h"
#include "apoc3d/Config/ABCConfigurationFormat.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/VertexFormats.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/TextData.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Utility/TypeConverter.h"
#include "apoc3d/Utility/StringUtils.h"

#include <ctime>

using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void ProjectParseSubItems(Project* prj, List<ProjectItem*>& parentContainer, ProjectFolder* parentFolder, const ConfigurationSection* sect);
	void ProjectSaveSubItems(ConfigurationSection* parentSect, List<ProjectItem*>& items, bool savingBuild);
	void GenerateFolderPackaging(int& startNo, List<ConfigurationSection*>& result, List<ProjectItem*>& items);

	static ProjectItem* GetItemGeneric(const String& path, const List<ProjectItem*>& subItems, ProjectFolder* parentFolder);

	/************************************************************************/
	/*  ProjectFolder                                                       */
	/************************************************************************/

	void ProjectFolder::Parse(const ConfigurationSection* sect)
	{
		sect->tryGetAttribute(L"Pack", PackType);

		if (PackType.size())
		{
			DestinationPack = sect->getAttribute(L"DestinationPack");
		}

		sect->tryGetAttribute(L"SubItemsSourceRelativeBase", SubItemsSourceRelativeBase);
		sect->tryGetAttribute(L"SubItemsDestinationRelativeBase", SubItemsDestinationRelativeBase);

		sect->TryGetAttributeBool(L"IncludeUnpackedSubFolderItems", IncludeUnpackedSubFolderItems);

		ProjectParseSubItems(m_project, SubItems, this, sect);
	}
	void ProjectFolder::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (PackType.size())
		{
			sect->AddAttributeString(L"Pack", PackType);
			sect->AddAttributeString(L"DestinationPack", DestinationPack);
			sect->AddAttributeBool(L"IncludeUnpackedSubFolderItems", IncludeUnpackedSubFolderItems);
		}

		if (SubItemsSourceRelativeBase.size())
			sect->AddAttributeString(L"SubItemsSourceRelativeBase", SubItemsSourceRelativeBase);

		if (SubItemsDestinationRelativeBase.size())
			sect->AddAttributeString(L"SubItemsDestinationRelativeBase", SubItemsDestinationRelativeBase);

		ProjectSaveSubItems(sect, SubItems, savingBuild);
	}
	void ProjectFolder::SavePackBuildConfig(ConfigurationSection* sect)
	{
		sect->AddAttributeString(L"Type", PackType);
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationPack, true));

		// a package file/archive build config section needs every item's path
		int pakEntryIndex = 0;
		AddPackBuildSubItems(sect, pakEntryIndex);
	}

	void ProjectFolder::AddPackBuildSubItems(ConfigurationSection* sect, int32& idx)
	{
		// this will also search sub folder which is not packed
		for (ProjectItem* pi : SubItems)
		{
			if (pi->getData())
			{
				ProjectItemData* item = pi->getData();

				if (item->getType() == ProjectItemType::Folder)
				{
					ProjectFolder* fol = static_cast<ProjectFolder*>(item);
					if (fol->PackType.empty() && IncludeUnpackedSubFolderItems)
					{
						fol->AddPackBuildSubItems(sect, idx);
					}
				}

				List<String> itemOutputs = item->GetAllOutputFiles();
				for (const String& itmFile : itemOutputs)
				{
					ConfigurationSection* e = new ConfigurationSection(L"Entry" + StringUtils::IntToString(idx++));
					e->AddAttributeString(L"FilePath", itmFile);
					sect->AddSection(e);
				}
			}
		}
	}

	List<String> ProjectFolder::GetAllInputFiles() { return{}; }
	List<String> ProjectFolder::GetAllOutputFiles()  { return MakeOutputFileList(DestinationPack); }

	ProjectItem* ProjectFolder::GetItem(const String& path) { return GetItemGeneric(path, SubItems, m_parentItem->getParentFolder()); }

	/************************************************************************/
	/*  ProjectResTexture                                                   */
	/************************************************************************/

	void ProjectResTexture::Parse(const ConfigurationSection* sect)
	{
		String tmp;

		Method = TextureBuildMethod::D3D;
		if (sect->tryGetAttribute(L"Method", tmp))
		{
			Method = ProjectUtils::TextureBuildMethodConv.Parse(tmp);
		}

		AssembleCubemap = false;
		AssembleVolumeMap = false;

		if (!sect->tryGetAttribute(L"Assemble", tmp))
		{
			SourceFile = sect->getAttribute(L"SourceFile");
		}
		else
		{
			StringUtils::ToLowerCase(tmp);
			if (tmp == L"cubemap")
			{
				AssembleCubemap = true;
				AssembleVolumeMap = false;

				SubMapTable.Add((uint)CUBE_NegativeX, sect->getAttribute(L"NegX"));
				SubMapTable.Add((uint)CUBE_NegativeY, sect->getAttribute(L"NegY"));
				SubMapTable.Add((uint)CUBE_NegativeZ, sect->getAttribute(L"NegZ"));

				SubMapTable.Add((uint)CUBE_PositiveX, sect->getAttribute(L"PosX"));
				SubMapTable.Add((uint)CUBE_PositiveY, sect->getAttribute(L"PosY"));
				SubMapTable.Add((uint)CUBE_PositiveZ, sect->getAttribute(L"PosZ"));

				String file;
				if (sect->tryGetAttribute(L"NegXAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeX, file);
				if (sect->tryGetAttribute(L"NegYAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeY, file);
				if (sect->tryGetAttribute(L"NegZAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeZ, file);

				if (sect->tryGetAttribute(L"PosXAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveX, file);
				if (sect->tryGetAttribute(L"PosYAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveY, file);
				if (sect->tryGetAttribute(L"PosZAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveZ, file);
			}
			else
			{
				AssembleCubemap = false;
				AssembleVolumeMap = true;

				uint i = 0;
				ConfigurationSection* srcsect = sect->getSection(L"Source");
				for (const ConfigurationSection* ss : srcsect->getSubSections())
				{
					SubMapTable.Add(i++, ss->getAttribute(L"FilePath"));
				}

				srcsect = sect->getSection(L"AlphaSource");
				i = 0;
				for (const ConfigurationSection* ss : srcsect->getSubSections())
				{
					SubAlphaMapTable.Add(i++, ss->getAttribute(L"FilePath"));
				}

			}

		}

		DestinationFile = sect->getAttribute(L"DestinationFile");

		GenerateMipmaps = false;
		sect->TryGetAttributeBool(L"GenerateMipmaps", GenerateMipmaps);

		if (sect->tryGetAttribute(L"Resizing", tmp))
		{
			Resizing.Parse(tmp);
		}

		ResizeFilterType = TextureFilterType::BSpline;
		if (sect->tryGetAttribute(L"ResizeFilter", tmp))
		{
			ResizeFilterType = ProjectUtils::TextureFilterTypeConv.Parse(tmp);
		}


		NewFormat = FMT_Unknown;
		if (sect->tryGetAttribute(L"PixelFormat", tmp))
		{
			NewFormat = PixelFormatUtils::ConvertFormat(tmp);
		}

		CompressionType = TextureCompressionType::None;
		if (sect->tryGetAttribute(L"Compression", tmp))
		{
			CompressionType = ProjectUtils::TextureCompressionTypeConv.Parse(tmp);
		}

	}
	void ProjectResTexture::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (Method != TextureBuildMethod::D3D)
		{
			sect->AddAttributeString(L"Method", ProjectUtils::TextureBuildMethodConv.ToString(Method));
		}

		if (AssembleCubemap || AssembleVolumeMap)
		{
			if (AssembleCubemap)
			{
				sect->AddAttributeString(L"Assemble", L"cubemap");

				sect->AddAttributeString(L"NegX", WrapSourcePath(SubMapTable[CUBE_NegativeX], savingBuild));
				sect->AddAttributeString(L"NegY", WrapSourcePath(SubMapTable[CUBE_NegativeY], savingBuild));
				sect->AddAttributeString(L"NegZ", WrapSourcePath(SubMapTable[CUBE_NegativeZ], savingBuild));
				sect->AddAttributeString(L"PosX", WrapSourcePath(SubMapTable[CUBE_PositiveX], savingBuild));
				sect->AddAttributeString(L"PosY", WrapSourcePath(SubMapTable[CUBE_PositiveY], savingBuild));
				sect->AddAttributeString(L"PosZ", WrapSourcePath(SubMapTable[CUBE_PositiveZ], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_NegativeX))
					sect->AddAttributeString(L"NegXAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_NegativeX], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_NegativeY))
					sect->AddAttributeString(L"NegYAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_NegativeY], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_NegativeZ))
					sect->AddAttributeString(L"NegZAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_NegativeZ], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_PositiveX))
					sect->AddAttributeString(L"PosXAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_PositiveX], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_PositiveY))
					sect->AddAttributeString(L"PosYAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_PositiveY], savingBuild));

				if (SubAlphaMapTable.Contains(CUBE_PositiveZ))
					sect->AddAttributeString(L"PosZAlpha", WrapSourcePath(SubAlphaMapTable[CUBE_PositiveZ], savingBuild));
			}
			else
			{
				sect->AddAttributeString(L"Assemble", L"volume");
				
				ConfigurationSection* srcsect = new ConfigurationSection(L"Source");
				int32 idx = 0;
				for (const String& fn : SubMapTable.getValueAccessor())
				{
					ConfigurationSection* es = new ConfigurationSection(L"Slice" + StringUtils::IntToString(idx++));
					es->AddAttributeString(L"FilePath", WrapSourcePath(fn, savingBuild));

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);

				srcsect = new ConfigurationSection(L"AlphaSource");
				idx = 0;
				for (const String& fn : SubAlphaMapTable.getValueAccessor())
				{
					ConfigurationSection* es = new ConfigurationSection(L"Slice" + StringUtils::IntToString(idx++));
					es->AddAttributeString(L"FilePath", WrapSourcePath(fn, savingBuild));

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);
			}
			
		}
		else
		{
			sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		}

		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, savingBuild));
		if (GenerateMipmaps)
			sect->AddAttributeString(L"GenerateMipmaps", StringUtils::BoolToString(GenerateMipmaps));

		if (Resizing.IsResizing())
		{
			sect->AddAttributeString(L"Resizing", Resizing.ToString());

			sect->AddAttributeString(L"ResizeFilter", ProjectUtils::TextureFilterTypeConv.ToString(ResizeFilterType));
		}

		if (NewFormat != FMT_Unknown)
		{
			sect->AddAttributeString(L"PixelFormat", PixelFormatUtils::ToString(NewFormat));
		}

		if (CompressionType != TextureCompressionType::None)
		{
			sect->AddAttributeString(L"Compression", ProjectUtils::TextureCompressionTypeConv.ToString(CompressionType));
		}
	}
	//bool ProjectResTexture::IsOutdated()
	//{
	//	time_t destFileTime;
	//	
	//	if (IsSettingsNewerThan(DestinationFile, destFileTime))
	//		return true;
	//	
	//	if (AssembleCubemap || AssembleVolumeMap)
	//	{
	//		for (const String& srcFile : SubMapTable.getValueAccessor())
	//		{
	//			if (IsSourceFileNewer(srcFile, destFileTime))
	//				return true;
	//		}
	//	}
	//	else
	//	{
	//		if (IsSourceFileNewer(SourceFile, destFileTime))
	//			return true;
	//	}
	//	return false;
	//}

	List<String> ProjectResTexture::GetAllInputFiles() 
	{
		if (AssembleCubemap || AssembleVolumeMap)
		{
			List<String > srcFiles;
			SubMapTable.FillValues(srcFiles);

			return MakeInputFileList(srcFiles);
		}
		return MakeInputFileList(SourceFile);
	}
	List<String> ProjectResTexture::GetAllOutputFiles() { return MakeOutputFileList(DestinationFile); }

	/************************************************************************/
	/*  ProjectResMaterial                                                  */
	/************************************************************************/

	void ProjectResMaterial::Parse(const ConfigurationSection* sect)
	{
		DestinationFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResMaterial::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, savingBuild));
	}

	List<String> ProjectResMaterial::GetAllInputFiles() { return{}; }
	List<String> ProjectResMaterial::GetAllOutputFiles() { return MakeOutputFileList(DestinationFile); }
	
	/************************************************************************/
	/*   ProjectResMaterialSet                                              */
	/************************************************************************/

	void ProjectResMaterialSet::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationLocation = sect->getAttribute(L"DestinationLocation");
		DestinationToken = sect->getAttribute(L"DestinationToken");
	}
	void ProjectResMaterialSet::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationLocation", WrapDestinationPath(DestinationLocation, savingBuild));
		sect->AddAttributeString(L"DestinationToken", WrapDestinationPath(DestinationToken, savingBuild));
	}

	static void ParseMaterialTree(List<String>& mtrlList, const String& baseMtrlName, const ConfigurationSection* sect)
	{
		String name = baseMtrlName;
		if (name.size())
		{
			name.append(L"_");
		}
		name.append(sect->getName());

		// go into sub sections
		for (ConfigurationSection* sub : sect->getSubSections())
		{
			ParseMaterialTree(mtrlList, name, sub);
		}

		mtrlList.Add(name);
	}

	List<String> ProjectResMaterialSet::GetAllInputFiles() { return MakeInputFileList(SourceFile); }

	List<String> ProjectResMaterialSet::GetAllOutputFiles()
	{
		List<String> res;

		String path = WrapSourcePath(SourceFile, true);
		if (File::FileExists(path))
		{
			FileLocation floc(path);
			Configuration config(path);
			XMLConfigurationFormat::Instance.Load(floc, &config);

			ConfigurationSection* mSect = config[L"Materials"];
			ConfigurationSection* incSec = config[L"Parts"];

			IncludeTable includeSources;

			if (incSec)
			{
				for (ConfigurationSection* subSect : incSec->getSubSections())
				{
					includeSources.Add(subSect->getName(), subSect);
				}
			}

			List<String> names;
			for (ConfigurationSection* sub : mSect->getSubSections())
			{
				ParseMaterialTreeWithPreprocessing(names, L"", sub, includeSources);
			} 
			
			String basePath = WrapDestinationPath(DestinationLocation, true);

			for (const String& name : names)
			{
				res.Add(PathUtils::Combine(basePath, name + L".mtrl"));
			}
		}

		if (DestinationToken.size())
			res.Add(WrapDestinationPath(DestinationToken, true));
		return res;
	}

	void ProjectResMaterialSet::ParseMaterialTreeWithPreprocessing(List<String>& result, const String& baseMtrlName, const ConfigurationSection* sect, const IncludeTable& includeSources)
	{
		bool hasInclude = sect->hasAttribute(L"Include");
		bool hasGenerate = sect->hasAttribute(L"Generate");

		if (hasInclude)
		{
			String includeTxt = sect->getAttribute(L"Include");

			ConfigurationSection* includeSect = MakeIncludedSection(sect, includeTxt, includeSources);

			if (includeSect == nullptr)
				return;

			ParseMaterialTreeWithPreprocessing(result, baseMtrlName, includeSect, includeSources);

			delete includeSect;
			return;
		}

		if (hasGenerate)
		{
			// for generation rules, make temporary sections as the generated result

			NumberRange numRange;
			sect->GetAttributeGeneric(L"Generate", numRange);

			for (int32 i = numRange.Start; i <= numRange.End; i++)
			{
				ConfigurationSection genSect(sect->getName() + StringUtils::IntToString(i));

				if (!ResolveGenerateExpressionsInSubtree(sect, genSect, i, baseMtrlName))
					return;

				ParseMaterialTree(result, baseMtrlName, &genSect, includeSources);
			}
		}
		else
		{
			ParseMaterialTree(result, baseMtrlName, sect, includeSources);
		}
	}
	void ProjectResMaterialSet::ParseMaterialTree(List<String>& result, const String& baseMtrlName, const ConfigurationSection* sect, const IncludeTable& includeSources)
	{
		// build a name
		String name = baseMtrlName;
		if (name.size())
		{
			name.append(L"_");
		}
		name.append(sect->getName());

		// go into sub sections
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ParseMaterialTreeWithPreprocessing(result, name, ss, includeSources);
		}

		result.Add(name);
	}


	bool ProjectResMaterialSet::ResolveGenerateExpressions(String& val, int32 curIdx)
	{
		// conditional
		if (val.find('@') != String::npos)
		{
			bool noMaches = true;

			List<String> conds = StringUtils::Split(val, L"@");

			for (const String& c : conds)
			{
				size_t pos = c.find('{');
				if (pos != String::npos)
				{
					String matchIdxStr = c.substr(0, pos);
					NumberRange nr;
					nr.Parse(matchIdxStr);

					if (nr.isInRange(curIdx))
					{
						noMaches = false;

						size_t pos2 = c.find_last_of('}');

						if (pos2 != String::npos)
						{
							pos++;
							val = c.substr(pos, pos2 - pos);
							break;
						}
						else return false;
					}
				}
				else
				{
					return false;
				}
			}

			if (noMaches)
			{
				val = L"";
				return true;
			}
		}

		// escape seq
		if (val.find(L"%d") != String::npos)
		{
			StringUtils::ReplaceAll(val, L"%d", StringUtils::IntToString(curIdx));
		}

		return true;
	}
	bool ProjectResMaterialSet::ResolveGenerateExpressionsInSubtree(const ConfigurationSection* src, ConfigurationSection& dst, int32 curIdx, const String& errName)
	{
		for (auto e : src->getAttributes())
		{
			if (e.Key == L"Generate")
				continue;

			String val = e.Value;

			if (!ResolveGenerateExpressions(val, curIdx))
				return false;

			if (val.size())
				dst.AddAttributeString(e.Key, val);
		}

		for (ConfigurationSection* ss : src->getSubSections())
		{
			ConfigurationSection* genSubSect = new ConfigurationSection(ss->getName());

			if (!ResolveGenerateExpressionsInSubtree(ss, *genSubSect, curIdx, errName + L"_" + ss->getName()))
			{
				delete genSubSect;
				return false;
			}

			dst.AddSection(genSubSect);
		}
		return true;
	}

	ConfigurationSection* ProjectResMaterialSet::MakeIncludedSection(const String& includeText, const IncludeTable& includeSources)
	{
		String::size_type posL = includeText.find_first_of('[');
		String::size_type posR = includeText.find_first_of(']');

		if (posL != String::npos && posR != String::npos)
		{
			String sectName = includeText.substr(0, posL);
			StringUtils::Trim(sectName);

			String paramListTxt = includeText.substr(posL + 1, posR - posL - 1);
			List<String> params = StringUtils::Split(paramListTxt, L",");

			ConfigurationSection* src;
			if (includeSources.TryGetValue(sectName, src))
			{
				src = new ConfigurationSection(*src);

				if (params.getCount() > 0)
				{
					for (int32 i = 0; i < params.getCount(); i++)
					{
						StringUtils::Trim(params[i]);
						ProcessIncludeParamInSubtree(src, L"$" + StringUtils::IntToString(i + 1), params[i]);
					}
				}
				return src;
			}
		}
		else
		{
			String sectName = includeText;
			StringUtils::Trim(sectName);

			ConfigurationSection* src;
			if (includeSources.TryGetValue(includeText, src))
			{
				return new ConfigurationSection(*src);
			}
		}
		return nullptr;
	}
	ConfigurationSection* ProjectResMaterialSet::MakeIncludedSection(const ConfigurationSection* sect, const String& includeText, const IncludeTable& includeSources)
	{
		ConfigurationSection* includeSect = MakeIncludedSection(includeText, includeSources);

		if (includeSect == nullptr)
			return nullptr;

		includeSect->Merge(sect, true, sect->getName());

		String temp;
		if (includeSect->tryGetAttribute(L"Include", temp) && temp == includeText)
		{
			includeSect->RemoveAttribute(L"Include");
		}
		return includeSect;
	}

	void ProjectResMaterialSet::ProcessIncludeParamInSubtree(ConfigurationSection* sect, const String& paramName, const String& paramValue)
	{
		for (auto e : sect->getAttributes())
		{
			StringUtils::ReplaceAll(e.Value, paramName, paramValue);
		}

		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ProcessIncludeParamInSubtree(ss, paramName, paramValue);
		}
	}

	void ProjectResMaterialSet::NumberRange::Parse(const String& txt)
	{
		int32 bounds[2];
		int32 count = StringUtils::SplitParseInts(txt, bounds, 2, L"-");
		if (count == 1)
			Start = End = bounds[0];
		else
		{
			Start = bounds[0];
			End = bounds[1];
		}
	}

	/************************************************************************/
	/*   ProjectResFont                                                     */
	/************************************************************************/
	
	void ProjectResFont::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		Size = sect->GetAttributeSingle(L"Size");

		AntiAlias = true;
		sect->TryGetAttributeBool(L"AntiAlias", AntiAlias);
		
		DestFile = sect->getAttribute(L"DestinationFile");

		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}
	}
	void ProjectResFont::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"Size", StringUtils::SingleToString(Size));

		sect->AddAttributeBool(L"AntiAlias", AntiAlias);

		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));

		for (int32 i = 0; i < Ranges.getCount(); i++)
		{
			const CharRange& cr = Ranges[i];

			ConfigurationSection* ss = new ConfigurationSection(String(L"Range") + StringUtils::IntToString(i));
			ss->AddAttributeString(L"Start", StringUtils::IntToString(cr.MinChar));
			ss->AddAttributeString(L"End", StringUtils::IntToString(cr.MaxChar));

			sect->AddSection(ss);
		}
	}

	/************************************************************************/
	/*   ProjectResFontGlyphDist                                            */
	/************************************************************************/

	void ProjectResFontGlyphDist::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResFontGlyphDist::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
	}

	/************************************************************************/
	/*   ProjectResEffect                                                   */
	/************************************************************************/

	void ProjectResEffect::Parse(const ConfigurationSection* sect)
	{
		int32 sourceCount = 0;
		String srcDesc = sect->getAttribute(L"Source");
		for (const auto& e : Split(srcDesc))
		{
			sourceCount++;

			if (!SpecifySource(e.first, e.second))
			{
				if (e.first.find(',') != String::npos)
				{
					for (const auto& f : StringUtils::Split(e.first, L", "))
					{
						SpecifySource(f, e.second);
					}
				}
			}
		}

		if (sourceCount==1)
		{
			EntryPointVS = L"vs_main";
			EntryPointPS = L"ps_main";
			EntryPointGS = L"gs_main";
		}
		else
			EntryPointVS = EntryPointPS = EntryPointGS = L"main";

		String entryPointsDesc;
		if (sect->tryGetAttribute(L"EntryPoints", entryPointsDesc))
		{
			for (const auto& e : Split(entryPointsDesc))
			{
				if (e.first == L"VS")
					EntryPointVS = e.second;
				else if (e.first == L"PS")
					EntryPointPS = e.second;
				else if (e.first == L"GS")
					EntryPointGS = e.second;
				else if (e.first == L"ALL")
				{
					EntryPointVS = EntryPointPS = EntryPointGS = e.second;
					break;
				}
			}
		}

		DestFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"ParamList", PListFile);

		StringUtils::Split(sect->getAttribute(L"Targets"), Targets, L"|");
		for (String& s : Targets)
		{
			StringUtils::Trim(s);
			StringUtils::ToLowerCase(s);
		}

		sect->TryGetAttributeBool(L"IsDebug", IsDebug);
		sect->TryGetAttributeBool(L"NoOptimization", NoOptimization);

		String defineDesc;
		if (sect->tryGetAttribute(L"Defines", defineDesc))
		{
			Defines = Split(defineDesc);
		}
	}
	void ProjectResEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (VS == PS && PS == GS)
		{
			sect->AddAttributeString(L"Source", L"ALL:" + WrapSourcePath(VS, savingBuild));
		}
		else
		{
			SettingList lst;
			lst.Add({ L"VS", WrapSourcePath(VS, savingBuild) });
			lst.Add({ L"PS", WrapSourcePath(PS, savingBuild) });
			if (GS.size())
				lst.Add({ L"GS", WrapSourcePath(GS, savingBuild) });
			
			sect->AddAttributeString(L"Source", Pack(lst));
		}

		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
		if (PListFile.size())
			sect->AddAttributeString(L"ParamList", WrapSourcePath(PListFile, savingBuild));

		if (EntryPointVS == EntryPointPS && EntryPointGS == EntryPointPS)
		{
			sect->AddAttributeString(L"EntryPoints", L"ALL:" + EntryPointVS);
		}
		else
		{
			SettingList lst;
			lst.Add({ L"VS", EntryPointVS });
			lst.Add({ L"PS", EntryPointPS });
			if (GS.size())
				lst.Add({ L"GS", EntryPointGS });

			sect->AddAttributeString(L"EntryPoints", Pack(lst));
		}

		sect->AddAttributeString(L"Targets", StringUtils::PackStrings(Targets, false, '|'));

		sect->AddAttributeBool(L"IsDebug", IsDebug);
		sect->AddAttributeBool(L"NoOptimization", NoOptimization);

		sect->AddAttributeString(L"Defines", Pack(Defines));

	}

	std::pair<String, String> ParseFXSettingItem(const String& p)
	{
		size_t pos = p.find(':');
		if (pos != String::npos)
		{
			String first = p.substr(0, pos);
			String second = p.substr(pos + 1);

			StringUtils::Trim(first);
			StringUtils::Trim(second);

			return{ first, second };
		}
		else
		{
			String c = p;
			StringUtils::Trim(c);
			return{c, L""};
		}
	}

	ProjectResEffect::SettingList ProjectResEffect::Split(const String& text)
	{
		return StringUtils::SplitParse<String, SettingList, std::pair<String, String>, ParseFXSettingItem>(text, L"|");
	}
	String ProjectResEffect::Pack(const SettingList& lst)
	{
		String result;
		for (int32 i = 0; i < lst.getCount();i++)
		{
			result += lst[i].first;
			if (lst[i].second.size())
			{
				result.append(1, ':');
				result += lst[i].second;
			}

			if (i != lst.getCount() - 1)
				result.append(L" | ");
		}
		return result;
	}

	static void FindIncludeFiles(HashSet<String>& fileList, const String& curFilePath)
	{
		if (curFilePath.size() == 0)
			return;

		if (File::FileExists(curFilePath))
		{
			if (!fileList.Contains(curFilePath))
			{
				fileList.Add(curFilePath);

				String basePath = PathUtils::GetDirectory(curFilePath);

				List<String> lines = StringUtils::Split(IO::Encoding::ReadAllText(FileLocation(curFilePath), Encoding::TEC_Unknown), L"\n\r");

				for (String l : lines)
				{
					StringUtils::Trim(l);

					if (StringUtils::StartsWith(l, L"#include", true))
					{
						String fn = l.substr(8);
						StringUtils::Trim(fn);

						if (StringUtils::StartsWith(fn, L"\""))
						{
							fn = fn.substr(1);
						}
						if (StringUtils::EndsWith(fn, L"\""))
						{
							fn = fn.substr(0, fn.size() - 1);
						}

						String subfn = PathUtils::Combine(basePath, fn);

						FindIncludeFiles(fileList, subfn);
					}
				}
			}
		}
	}

	List<String> ProjectResEffect::GetAllInputFiles() 
	{
		List<String> initialFiles = MakeInputFileList({ VS, PS, GS, PListFile });
		HashSet<String> fileList;
		for (const String& ifn : initialFiles)
		{
			FindIncludeFiles(fileList, ifn);
		}

		List<String> result;
		fileList.FillItems(result);
		return result;
	}
	List<String> ProjectResEffect::GetAllOutputFiles() { return MakeOutputFileList(DestFile); }


	bool ProjectResEffect::SpecifySource(const String& srcName, const String& source)
	{
		if (srcName == L"VS")
		{
			VS = source;
			return true;
		}
		else if (srcName == L"PS")
		{
			PS = source;
			return true;
		}
		else if (srcName == L"GS")
		{
			GS = source;
			return true;
		}
		else if (srcName == L"ALL")
		{
			VS = PS = GS = source;
			return true;
		}
		return false;
	}

	/************************************************************************/
	/*   ProjectResCustomEffect                                             */
	/************************************************************************/
	void ProjectResCustomEffect::Parse(const ConfigurationSection* sect)
	{
		SrcVSFile = sect->getAttribute(L"VSSource");
		SrcPSFile = sect->getAttribute(L"PSSource");
		DestFile = sect->getAttribute(L"DestinationFile");
		EntryPointVS = sect->getAttribute(L"EntryPointVS");
		EntryPointPS = sect->getAttribute(L"EntryPointPS");
		Profile = sect->getAttribute(L"Profile");
	}
	void ProjectResCustomEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"VSSource", WrapSourcePath(SrcVSFile, savingBuild));
		sect->AddAttributeString(L"PSSource", WrapSourcePath(SrcPSFile, savingBuild));

		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
		sect->AddAttributeString(L"EntryPointVS", EntryPointVS);
		sect->AddAttributeString(L"EntryPointPS", EntryPointPS);
		sect->AddAttributeString(L"Profile", Profile);
	}
	
	/************************************************************************/
	/*  ProjectResEffectList                                                */
	/************************************************************************/

	// Finds all effects in the project
	static void WalkProjectForEffects(const List<ProjectItem*>& items, List<String>& effectsFound)
	{
		for (ProjectItem* pi : items)
		{
			if (pi->getType() == ProjectItemType::Effect)
			{
				effectsFound.Add(pi->getName());
			}
			else if (pi->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(pi->getData());

				WalkProjectForEffects(folder->SubItems, effectsFound);
			}
		}
	}

	void ProjectResEffectList::Parse(const ConfigurationSection* sect)
	{
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResEffectList::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, true));

		if (savingBuild)
		{
			List<String> effectList;
			WalkProjectForEffects(m_project->getItems(), effectList);

			for (const String& fxName : effectList)
			{
				ConfigurationSection* ss = new ConfigurationSection(fxName);
				sect->AddSection(ss);
			}
		}
	}

	/************************************************************************/
	/*  ProjectResShaderNetwork                                             */
	/************************************************************************/

	void ProjectResShaderNetwork::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResShaderNetwork::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SrcFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
	}

	/************************************************************************/
	/*  ProjectResModel                                                     */
	/************************************************************************/

	void ProjectResModel::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		sect->tryGetAttribute(L"PresetFile", PresetFile);

		Method = MeshBuildMethod::ASS;
		sect->TryGetAttributeEnum(L"Method", Method, ProjectUtils::MeshBuildMethodConv);
		
		UseVertexFormatConversion = false;
		ConfigurationSection* subs = sect->getSection(L"VertexFormatConversion");
		if (subs && subs->getSubSectionCount() > 0)
		{
			UseVertexFormatConversion = true;
			for (const ConfigurationSection* ent : subs->getSubSections())
			{
				VertexElementUsage usage = VertexElementUsageConverter[ent->getName()];
				int index = 0;
				if (usage == VEU_TextureCoordinate)
				{
					index = StringUtils::ParseInt32(ent->getValue());
				}

				// the vertex elements here only has usage and index. 
				// They only store info here, not for normal use in graphics
				ConversionVertexElements.Add(VertexElement(0, VEF_Count, usage, index));
			}
		}

		CompactBuild = false;
		sect->TryGetAttributeBool(L"CompactBuild", CompactBuild);

		CollapseMeshs = false;
		sect->TryGetAttributeBool(L"CollapseMeshs", CollapseMeshs);

		CollapseAll = false;
		sect->TryGetAttributeBool(L"CollapseAll", CollapseAll);

	}
	void ProjectResModel::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SrcFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DstFile, savingBuild));
		if (DstAnimationFile.size())
			sect->AddAttributeString(L"DestinationAnimFile", WrapDestinationPath(DstAnimationFile, savingBuild));

		if (PresetFile.size())
			sect->AddAttributeString(L"PresetFile", WrapSourcePath(PresetFile, savingBuild));

		sect->AddAttributeEnum(L"Method", Method, ProjectUtils::MeshBuildMethodConv);

		if (UseVertexFormatConversion && ConversionVertexElements.getCount() > 0)
		{
			ConfigurationSection* subs = new ConfigurationSection(L"VertexFormatConversion");
			for (const VertexElement& ve : ConversionVertexElements)
			{
				ConfigurationSection* vs;
				if (ve.getUsage() == VEU_TextureCoordinate)
				{
					String idxText = StringUtils::IntToString(ve.getIndex());

					vs = new ConfigurationSection(VertexElementUsageConverter[ve.getUsage()] + idxText);
					vs->SetValue(idxText);
				}
				else
				{
					vs = new ConfigurationSection(VertexElementUsageConverter[ve.getUsage()]);
				}

				subs->AddSection(vs);
			}
			sect->AddSection(subs);
		}
		
		if (CompactBuild)
			sect->AddAttributeBool(L"CompactBuild", CompactBuild);

		if (CollapseMeshs)
			sect->AddAttributeBool(L"CollapseMeshs", CollapseMeshs);

		if (CollapseAll)
			sect->AddAttributeBool(L"CollapseAll", CollapseAll);

	}

	/************************************************************************/
	/*   ProjectResMAnim                                                    */
	/************************************************************************/

	void ProjectResMAnim::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResMAnim::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, savingBuild));
	}
	
	/************************************************************************/
	/*    ProjectResTAnim                                                   */
	/************************************************************************/

	void ProjectResTAnim::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");
		Reverse = false;
		sect->TryGetAttributeBool(L"Reverse", Reverse);

		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			String name = ss->getName();
			int objIdx = StringUtils::ParseInt32(ss->getValue());

			ObjectIndexMapping.Add(name, objIdx);
		}
	}
	void ProjectResTAnim::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, true));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, true));
		sect->AddAttributeString(L"Reverse", StringUtils::BoolToString(Reverse));

		for (auto e : ObjectIndexMapping)
		{
			ConfigurationSection* ss = new ConfigurationSection(e.Key);
			
			ss->SetValue(StringUtils::IntToString(e.Value));

			sect->AddSection(ss);
		}
	}

	/************************************************************************/
	/*   ProjectResUILayout                                                 */
	/************************************************************************/

	void ProjectResUILayout::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResUILayout::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, savingBuild));
	}

	/************************************************************************/
	/*   ProjectResCopy                                                     */
	/************************************************************************/


	void ProjectResCopy::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResCopy::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestinationFile, savingBuild));
	}

	/************************************************************************/
	/*    ProjectCustomItem                                                 */
	/************************************************************************/

	void ProjectCustomItem::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"EditorExtension", EditorExtension);

		for (ConfigurationSection* ss : sect->getSubSections())
		{
			Properties.Add(ss->getName(), ss->getValue());
		}
	}
	void ProjectCustomItem::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SourceFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
		if (EditorExtension.size())
		{
			sect->AddAttributeString(L"EditorExtension", EditorExtension);
		}

		for (auto e : Properties)
		{
			ConfigurationSection* valSect = new ConfigurationSection(e.Key);
			valSect->SetValue(e.Value);

			sect->AddSection(valSect);
		}
	}

	/************************************************************************/
	/*  ProjectItemPreset                                                   */
	/************************************************************************/
	ProjectItemPreset::~ProjectItemPreset()
	{
		DELETE_AND_NULL(SectionCopy);
	}

	void ProjectItemPreset::Parse(const ConfigurationSection* sect)
	{
		DELETE_AND_NULL(SectionCopy);

		SectionCopy = new ConfigurationSection(*sect);
	}
	void ProjectItemPreset::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (SectionCopy)
			sect->Merge(SectionCopy, true);
	}


	/************************************************************************/
	/*  ProjectItemData                                                     */
	/************************************************************************/

	String ProjectItemData::GetAbsoluteSourcePathBase(bool ingoreRelativeBase) const
	{
		if (!ingoreRelativeBase)
		{
			ProjectFolder* pf = m_parentItem->getParentFolder();

			if (pf && pf->SubItemsSourceRelativeBase != L":")
			{
				String basePath;
				while (pf && pf->SubItemsSourceRelativeBase != L":")
				{
					if (pf->SubItemsSourceRelativeBase.size())
						basePath = PathUtils::Combine(pf->SubItemsSourceRelativeBase, basePath);

					pf = pf->getParentItem()->getParentFolder();
				}
				return PathUtils::Combine(m_project->getBasePath(), basePath);
			}
		}
		
		return m_project->getBasePath();
	}
	String ProjectItemData::GetAbsoluteDestinationPathBase(bool ingoreRelativeBase) const
	{
		if (!ingoreRelativeBase)
		{
			ProjectFolder* pf = m_parentItem->getParentFolder();

			while (pf && pf->SubItemsDestinationRelativeBase != L":")
			{
				String basePath;
				while (pf && pf->SubItemsDestinationRelativeBase != L":")
				{
					if (pf->SubItemsDestinationRelativeBase.size())
						basePath = PathUtils::Combine(pf->SubItemsDestinationRelativeBase, basePath);

					pf = pf->getParentItem()->getParentFolder();
				}
				return PathUtils::Combine(m_project->getOutputPath(), basePath);
			}
		}

		return m_project->getOutputPath();
	}

	String ProjectItemData::GetAbsoluteSourcePath(const String& path) const 
	{
		if (StringUtils::StartsWith(path, L":"))
			return PathUtils::Combine(GetAbsoluteSourcePathBase(true), path.substr(1));
		return PathUtils::Combine(GetAbsoluteSourcePathBase(false), path);
	}
	String ProjectItemData::GetAbsoluteDestinationPath(const String& path) const 
	{
		if (StringUtils::StartsWith(path, L":"))
			return PathUtils::Combine(GetAbsoluteDestinationPathBase(true), path.substr(1));
		return PathUtils::Combine(GetAbsoluteDestinationPathBase(false), path);
	}
	
	String ProjectItemData::WrapSourcePath(const String& path, bool isAbsolute) const
	{
		if (isAbsolute)
		{
			return GetAbsoluteSourcePath(path);
		}
		return path;
	}
	String ProjectItemData::WrapDestinationPath(const String& path, bool isAbsolute) const
	{
		if (isAbsolute)
		{
			return GetAbsoluteDestinationPath(path);
		}
		return path;
	}


	List<String> ProjectItemData::MakeOutputFileList(const String& destinationFile)
	{
		List<String> e;
		if (destinationFile.size()) e.Add(WrapDestinationPath(destinationFile, true));
		return e;
	}
	List<String> ProjectItemData::MakeOutputFileList(const String& f1, const String& f2)
	{
		List<String> e;
		if (f1.size()) e.Add(WrapDestinationPath(f1, true));
		if (f2.size()) e.Add(WrapDestinationPath(f2, true));
		return e;
	}
	List<String> ProjectItemData::MakeOutputFileList(const String& f1, const String& f2, const String& f3)
	{
		List<String> e;
		if (f1.size()) e.Add(WrapDestinationPath(f1, true));
		if (f2.size()) e.Add(WrapDestinationPath(f2, true));
		if (f3.size()) e.Add(WrapDestinationPath(f3, true));
		return e;
	}
	List<String> ProjectItemData::MakeOutputFileList(const List<String>& files)
	{
		List<String> e(files.getCount());
		for (const String& fn : files)
			if (fn.size()) e.Add(WrapDestinationPath(fn, true));
		return e;
	}


	List<String> ProjectItemData::MakeInputFileList(const String& srcFile)
	{
		List<String> e;
		if (srcFile.size()) e.Add(WrapSourcePath(srcFile, true));
		return e;
	}
	List<String> ProjectItemData::MakeInputFileList(const String& f1, const String& f2)
	{
		List<String> e;
		if (f1.size()) e.Add(WrapSourcePath(f1, true));
		if (f2.size()) e.Add(WrapSourcePath(f2, true));
		return e;
	}
	List<String> ProjectItemData::MakeInputFileList(const String& f1, const String& f2, const String& f3)
	{
		List<String> e;
		if (f1.size()) e.Add(WrapSourcePath(f1, true));
		if (f2.size()) e.Add(WrapSourcePath(f2, true));
		if (f3.size()) e.Add(WrapSourcePath(f3, true));
		return e;
	}
	List<String> ProjectItemData::MakeInputFileList(const List<String>& files)
	{
		List<String> e(files.getCount());
		for (const String& fn : files)
			if (fn.size()) e.Add(WrapSourcePath(fn, true));
		return e;
	}

	/************************************************************************/
	/* ProjectBuildStamp                                                    */
	/************************************************************************/

	const int32 ProjectBuildStampID = 'PBST';

	void ProjectBuildStamp::Load(Stream& strm)
	{
		if (strm.getLength() < 16)
			return;

		BinaryReader br(&strm, false);

		if (br.ReadInt32() == ProjectBuildStampID)
		{
			br.ReadInt32();

			int32 count = br.ReadInt32();
			m_sourceTimeStamps.Resize(count);
			for (int32 i = 0; i < count;i++)
			{
				String p = br.ReadString();
				int64 t = br.ReadInt64();

				m_sourceTimeStamps.Add(p, t);
			}

			count = br.ReadInt32();
			m_itemSettingStamps.Resize(count);
			for (int32 i = 0; i < count;i++)
			{
				String p = br.ReadString();
				uint32 t = br.ReadUInt32();

				m_itemSettingStamps.Add(p, t);
			}

			m_hasData = true;
		}
	}

	void ProjectBuildStamp::Save(Stream& strm)
	{
		BinaryWriter bw(&strm, false);

		bw.WriteInt32(ProjectBuildStampID);
		bw.WriteInt32(0);

		bw.WriteInt32(m_sourceTimeStamps.getCount());
		for (auto e : m_sourceTimeStamps)
		{
			bw.WriteString(e.Key);
			bw.WriteInt64(e.Value);
		}

		bw.WriteInt32(m_itemSettingStamps.getCount());
		for (auto e : m_itemSettingStamps)
		{
			bw.WriteString(e.Key);
			bw.WriteUInt32(e.Value);
		}
	}
	
	void ProjectBuildStamp::Clear()
	{
		m_sourceTimeStamps.Clear();
		m_itemSettingStamps.Clear();
		m_hasData = false;
	}

	int64 ProjectBuildStamp::LookupSourceTimestamp(const String& path) const
	{
		int64 r = 0;
		m_sourceTimeStamps.TryGetValue(path, r);
		return r;
	}

	void ProjectBuildStamp::SetSourceTimestamp(const String& path, time_t t) 
	{
		m_hasData = true;

		if (!m_sourceTimeStamps.Contains(path))
			m_sourceTimeStamps.Add(path, t);
		else
			m_sourceTimeStamps[path] = t;
	}

	uint32 ProjectBuildStamp::LookupItemSettingStamp(const String& path) const
	{
		uint32 r = 0;
		m_itemSettingStamps.TryGetValue(path, r);
		return r;
	}
	void ProjectBuildStamp::SetItemSettingStamp(const String& path, uint32 hash)
	{
		m_hasData = true;
		if (!m_itemSettingStamps.Contains(path))
			m_itemSettingStamps.Add(path, hash);
		else
			m_itemSettingStamps[path] = hash;
	}

	/************************************************************************/
	/*    ProjectItem                                                       */
	/************************************************************************/

	void ProjectItem::Rename(const String& newName)
	{
		m_name = newName;
	}

	void ProjectItem::SetParent(ProjectFolder* parent)
	{
		m_parentFolder = parent;
	}


	ConfigurationSection* ProjectItem::Save(bool savingBuild)
	{
		if (savingBuild && !IsOutDated())
		{
			return 0;
		}

		if (m_typeData)
		{
			ConfigurationSection* sect = new ConfigurationSection(m_name);

			if (m_inheritSouceItem.size())
				sect->AddAttributeString(L"Inherits", m_inheritSouceItem);

			sect->AddAttributeString(L"Type", ProjectUtils::ProjectItemTypeConv.ToString(m_typeData->getType()));
			
			m_typeData->Save(sect, savingBuild);
			
			if (!savingBuild && m_inheritSouceItem.size())
			{
				ProjectItem* inheritSrc = SearchItem(m_inheritSouceItem);
				if (inheritSrc && inheritSrc->getType() == ProjectItemType::ItemPreset)
				{
					ProjectItemPreset* preset = static_cast<ProjectItemPreset*>(inheritSrc->getData());
					sect->RemoveIntersection(preset->SectionCopy);
				}
				else
				{
					ApocLog(LOG_System, L"Can't find inherited preset: " + m_inheritSouceItem, LOGLVL_Warning);
				}
			}

			return sect;
		}
		return nullptr;
	}

	void ProjectItem::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getName();
		sect->tryGetAttribute(L"Inherits", m_inheritSouceItem);

		const ConfigurationSection* adjustedSect = sect;
		ConfigurationSection* newSect = nullptr;

		if (m_inheritSouceItem.size())
		{
			ProjectItem* inhSrc = SearchItem(m_inheritSouceItem);
			if (inhSrc && inhSrc->getType() == ProjectItemType::ItemPreset)
			{
				ProjectItemPreset* preset = static_cast<ProjectItemPreset*>(inhSrc->getData());

				newSect = new ConfigurationSection(*sect);
				newSect->Merge(preset->SectionCopy, true);
				adjustedSect = newSect;
			}
			else
			{
				ApocLog(LOG_System, L"Can't find inherited preset: " + m_inheritSouceItem, LOGLVL_Warning);
			}
		}

		ProjectItemType itemType = ProjectUtils::ProjectItemTypeConv.Parse(adjustedSect->getAttribute(L"Type"));
		
		switch (itemType)
		{
		case ProjectItemType::Custom:
			m_typeData = new ProjectCustomItem(m_project, this);
			break;
		case ProjectItemType::Folder:
			m_typeData = new ProjectFolder(m_project, this);
			break;
		case ProjectItemType::Material:
			m_typeData = new ProjectResMaterial(m_project, this);
			break;
		case ProjectItemType::MaterialSet:
			m_typeData = new ProjectResMaterialSet(m_project, this);
			break;
		case ProjectItemType::Texture:
			m_typeData = new ProjectResTexture(m_project, this);
			break;
		case ProjectItemType::Model:
			m_typeData = new ProjectResModel(m_project, this);
			break;
		case ProjectItemType::TransformAnimation:
			m_typeData = new ProjectResTAnim(m_project, this);
			break;
		case ProjectItemType::MaterialAnimation:
			m_typeData = new ProjectResMAnim(m_project, this);
			break;
		case ProjectItemType::Effect:
			m_typeData = new ProjectResEffect(m_project, this);
			break;
		case ProjectItemType::EffectList:
			m_typeData = new ProjectResEffectList(m_project, this);
			break;
		case ProjectItemType::CustomEffect:
			m_typeData = new ProjectResCustomEffect(m_project, this);
			break;
		case ProjectItemType::ShaderNetwork:
			m_typeData = new ProjectResShaderNetwork(m_project, this);
			break;
		case ProjectItemType::Font:
			m_typeData = new ProjectResFont(m_project, this);
			break;
		case ProjectItemType::FontGlyphDist:
			m_typeData = new ProjectResFontGlyphDist(m_project, this);
			break;
		case ProjectItemType::UILayout:
			m_typeData = new ProjectResUILayout(m_project, this);
			break;
		case ProjectItemType::Copy:
			m_typeData = new ProjectResCopy(m_project, this);
			break;
		case ProjectItemType::ItemPreset:
			m_typeData = new ProjectItemPreset(m_project, this);
			break;
		default:
			assert(0);
		}

		m_typeData->Parse(adjustedSect);

		DELETE_AND_NULL(newSect);
	}

	ProjectItem* ProjectItem::SearchItem(const String& path) const
	{
		String npath = PathUtils::NormalizePath(path);

		if (m_parentFolder &&
			npath.find(PathUtils::AltDirectorySeparator) == String::npos &&
			npath.find(PathUtils::DirectorySeparator) == String::npos)
		{
			for (ProjectItem* pi : m_parentFolder->SubItems)
			{
				if (pi->getName() == npath)
					return pi;
			}
		}

		if (m_parentFolder && StringUtils::StartsWith(npath, L".."))
		{
			return m_parentFolder->GetItem(npath);
		}

		return m_project->GetItem(npath);
	}

	bool ProjectItem::IsNotBuilt() const
	{
		if (m_typeData)
		{
			if (m_typeData->AlwaysBuild())
				return true;

			for (const String& fn : m_typeData->GetAllOutputFiles())
			{
				if (!File::FileExists(fn))
					return true;
			}
		}
		return false;
	}


	ProjectItemOutdateType ProjectItem::GetOutDatedType()
	{
		if (IsNotBuilt())
			return ProjectItemOutdateType::NewerSource;

		if (m_typeData)
		{
			if (m_typeData->AlwaysBuild())
				return ProjectItemOutdateType::NewerSource;

			{
				// check if source is newer
				int64 maxSourceTime = 0;
				int64 minDestTime = INT64_MAX;

				for (const String& fn : m_typeData->GetAllInputFiles())
				{
					if (File::FileExists(fn))
					{
						time_t mt = File::GetFileModifiyTime(fn);
						if (mt && mt > maxSourceTime)
							maxSourceTime = mt;
					}
				}
				for (const String& fn : m_typeData->GetAllOutputFiles())
				{
					if (File::FileExists(fn))
					{
						time_t mt = File::GetFileModifiyTime(fn);
						if (mt && mt < minDestTime)
							minDestTime = mt;
					}
				}
				if (maxSourceTime > minDestTime)
					return ProjectItemOutdateType::NewerSource;
			}

			if (m_project->getBuildStamp().hasData() && !m_typeData->RequiresPostEdit())
			{
				// check if the setting stamp matches current
				if (m_prevBuildSettingStamp != 0 && GetCurrentBuildSettingStamp() != m_prevBuildSettingStamp)
					return ProjectItemOutdateType::DifferentSetting;

				// check if any sources have different mod time than the one in the build stamp
				for (const String& fn : m_typeData->GetAllInputFiles())
				{
					if (File::FileExists(fn))
					{
						time_t mt = File::GetFileModifiyTime(fn);

						if (mt && m_project->getBuildStamp().LookupSourceTimestamp(fn) != mt)
							return ProjectItemOutdateType::DifferentSource;
					}
				}

			}

		}
		return ProjectItemOutdateType::Current;
	}
	
	bool ProjectItem::IsOutDated()
	{
		return GetOutDatedType() != ProjectItemOutdateType::Current;
	}


	uint32 ProjectItem::GetCurrentBuildSettingStamp()
	{
		ConfigurationSection* s = Save(false);

		int32 r = s->GetHashCode();

		delete s;

		if (m_inheritSouceItem.size())
		{
			ProjectItem* inheritSrc = SearchItem(m_inheritSouceItem);
			if (inheritSrc)
			{
				r ^= inheritSrc->GetCurrentBuildSettingStamp();
			}
		}

		return r;
	}
	
	void ProjectItem::LoadPrevBuildStamp(const String& path)
	{
		const String curPath = PathUtils::Combine(path, m_name);
		m_prevBuildSettingStamp = m_project->getBuildStamp().LookupItemSettingStamp(curPath);

		if (m_typeData == nullptr)
			return;

		if (getType() == ProjectItemType::Folder)
		{
			ProjectFolder* fld = static_cast<ProjectFolder*>(m_typeData);

			for (ProjectItem* pi : fld->SubItems)
			{
				pi->LoadPrevBuildStamp(curPath);
			}
		}
	}
	void ProjectItem::SaveCurrentBuildStamp(const String& path, ProjectBuildStamp& stmp)
	{
		const String curPath = PathUtils::Combine(path, m_name);
		uint32 hash = GetCurrentBuildSettingStamp();
		stmp.SetItemSettingStamp(curPath, hash);

		if (m_typeData == nullptr)
			return;

		for (const String& fn : m_typeData->GetAllInputFiles())
		{
			if (File::FileExists(fn))
			{
				time_t mt = File::GetFileModifiyTime(fn);

				if (mt)
					stmp.SetSourceTimestamp(fn, mt);
			}
		}
		if (getType() == ProjectItemType::Folder)
		{
			ProjectFolder* fld = static_cast<ProjectFolder*>(m_typeData);

			for (ProjectItem* pi : fld->SubItems)
			{
				pi->SaveCurrentBuildStamp(curPath, stmp);
			}
		}
	}

	// obsolete?
	void ProjectItem::MarkOutdatedOutputs()
	{
		ProjectItemOutdateType ot = GetOutDatedType();
		if (ot == ProjectItemOutdateType::DifferentSource || ot == ProjectItemOutdateType::DifferentSetting)
		{
			// change all output file's last write time to a second earlier than the source's
			// This is to prevent the item being treated as up to date if build fails but the stamps were written to current

			time_t minSourceTime = ULONG_MAX;

			for (const String& fn : m_typeData->GetAllInputFiles())
			{
				if (File::FileExists(fn))
				{
					time_t t = File::GetFileModifiyTime(fn);
					if (t && t < minSourceTime)
						minSourceTime = t;
				}
			}

			if (minSourceTime != ULONG_MAX)
			{
				for (const String& fn : m_typeData->GetAllOutputFiles())
				{
					if (File::FileExists(fn))
					{
						File::SetFileModifiyTime(fn, minSourceTime - 1); // one second earlier
					}
				}
			}
		}

		if (getType() == ProjectItemType::Folder)
		{
			ProjectFolder* fld = static_cast<ProjectFolder*>(m_typeData);

			for (ProjectItem* pi : fld->SubItems)
			{
				pi->MarkOutdatedOutputs();
			}
		}
	}
	/************************************************************************/
	/*    Project                                                           */
	/************************************************************************/

	void Project::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getAttribute(L"Name");
		sect->tryGetAttribute(L"TexturePath", m_texturePath);
		sect->tryGetAttribute(L"MaterialPath", m_materialPath);

		sect->tryGetAttribute(L"ExplicitBuildPath", m_originalOutputPath);
		m_outputPath = m_originalOutputPath;

		ProjectParseSubItems(this, m_items, nullptr, sect);
	}
	void Project::Save(const String& file)
	{
		ConfigurationSection* s = Save();

		Configuration xc(m_name);
		xc.Add(s);
		//xc->Save(file);

		XMLConfigurationFormat::Instance.Save(&xc, FileOutStream(file));
	}

	ConfigurationSection* Project::Save()
	{
		ConfigurationSection* sect = new ConfigurationSection(L"Project", m_items.getCount() * 2);

		sect->AddAttributeString(L"Name", m_name);
		sect->AddAttributeString(L"TexturePath", m_texturePath);
		sect->AddAttributeString(L"MaterialPath", m_materialPath);

		if (m_originalOutputPath.size())
			sect->AddAttributeString(L"ExplicitBuildPath", m_originalOutputPath);
		ProjectSaveSubItems(sect, m_items, false);

		return sect;
	}

	void Project::LoadBuildStamp(Stream& strm)
	{
		m_buildStamp.Clear();

		m_buildStamp.Load(strm);

		for (ProjectItem* pi : m_items)
		{
			pi->LoadPrevBuildStamp(L"");
		}
	}
	void Project::SaveBuildStamp(Stream& strm)
	{
		m_buildStamp.Clear();

		for (ProjectItem* pi : m_items)
		{
			pi->SaveCurrentBuildStamp(L"", m_buildStamp);
		}
		m_buildStamp.Save(strm);
	}
	void Project::MarkOutdatedOutputs()
	{
		for (ProjectItem* pi : m_items)
		{
			pi->MarkOutdatedOutputs();
		}
	}

	void Project::GenerateBuildScripts(List<ProjectBuildScript>& result)
	{
		List<ConfigurationSection*> subtrees;

		ConfigurationSection* sect = new ConfigurationSection(L"Build", m_items.getCount() * 2);

		sect->AddAttributeString(L"Name", m_name);

		ProjectSaveSubItems(sect, m_items, true);

		sect->AddAttributeString(L"Type", L"Project");
		subtrees.Add(sect);
		
		int32 startNo = Randomizer::NextInclusive(65535);
		GenerateFolderPackaging(startNo, subtrees, m_items);
		
		// pack up with settings
		{
			String baseOutputDir = m_outputPath;

			for (ConfigurationSection* part : subtrees)
			{
				result.Add({ part, baseOutputDir });
			}
		}
	}

	void GenerateFolderPackaging(int& startNo, List<ConfigurationSection*>& result, List<ProjectItem*>& items)
	{
		// post traversal on the project tree will make leaf folder to pack file builds comes first
		for (ProjectItem* pi : items)
		{
			if (pi->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(pi->getData());

				if (fld)
				{
					GenerateFolderPackaging(startNo, result, fld->SubItems);

					if (fld->PackType.size())
					{
						ConfigurationSection* s = new ConfigurationSection(L"Archive_" + StringUtils::IntToString(startNo++));
						fld->SavePackBuildConfig(s);

						// these packaging builds will be added to the end, as the prerequisite items should be built first
						result.Add(s);
					}
				}
			}
		}
	}


	// this function is for parsing all sub items in a section
	void ProjectParseSubItems(Project* prj, List<ProjectItem*>& parentContainer, ProjectFolder* parentFolder, const ConfigurationSection* sect)
	{
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ProjectItem* item = new ProjectItem(prj);
			item->SetParent(parentFolder);

			item->Parse(ss);

			parentContainer.Add(item);
		}
	}

	// this function is for saving all sub items into a section
	void ProjectSaveSubItems(ConfigurationSection* parentSect, List<ProjectItem*>& items, bool savingBuild)
	{
		for (ProjectItem* pi : items)
		{
			ConfigurationSection* sect = pi->Save(savingBuild);
			if (sect)
				parentSect->AddSection(sect);
		}
	}


	ProjectItem* GetItemGeneric(const String& path, const List<ProjectItem*>& subItems, ProjectFolder* parentFolder)
	{
		String itemName, subPath;
		size_t pos = path.find(PathUtils::DirectorySeparator);
		if (pos != String::npos)
		{
			itemName = path.substr(0, pos);
			subPath = path.substr(itemName.size() + 1);
		}
		else
		{
			itemName = path;
		}

		if (parentFolder && itemName == L".." && subPath.size())
			return parentFolder->GetItem(subPath);

		for (ProjectItem* pi : subItems)
		{
			if (pi->getName() == itemName)
			{
				if (subPath.empty())
					return pi;

				if (pi->getType() == ProjectItemType::Folder)
				{
					ProjectFolder* fld = static_cast<ProjectFolder*>(pi->getData());
					return fld->GetItem(subPath);
				}
				return nullptr;
			}
		}
		return nullptr;
	}


	void Project::SetPath(const String& basePath, const String* outputPath)
	{
		m_basePath = basePath;

		if (outputPath)
		{
			m_outputPath = *outputPath;
		}
		else
		{
			// existing m_outputPath is the explicit build path, optional

			if (m_outputPath.size() == 0)
				m_outputPath = m_basePath;
			else
			{
				// explicit build path specified
				
				if (!PathUtils::IsAbsolute(m_outputPath))
				{
					m_outputPath = PathUtils::Combine(m_basePath, m_outputPath);
				}
			}
			PathUtils::Append(m_outputPath, L"build");
		}
	}

	ProjectItem* Project::GetItem(const String& path) { return GetItemGeneric(path, m_items, nullptr); }
	

	//////////////////////////////////////////////////////////////////////////

	const TypeDualConverter<ProjectItemType> ProjectUtils::ProjectItemTypeConv =
	{
		{ L"Custom", ProjectItemType::Custom },
		{ L"Folder", ProjectItemType::Folder },
		{ L"Material", ProjectItemType::Material },
		{ L"MaterialSet", ProjectItemType::MaterialSet },
		{ L"Texture", ProjectItemType::Texture },
		{ L"Model", ProjectItemType::Model },
		{ L"tanim", ProjectItemType::TransformAnimation },
		{ L"manim", ProjectItemType::MaterialAnimation },
		{ L"Effect", ProjectItemType::Effect },
		{ L"EffectList", ProjectItemType::EffectList },
		{ L"CustomEffect", ProjectItemType::CustomEffect },
		{ L"ShaderNetwork", ProjectItemType::ShaderNetwork },
		{ L"Font", ProjectItemType::Font },
		{ L"FontGlyphDist", ProjectItemType::FontGlyphDist },
		{ L"UILayout", ProjectItemType::UILayout },
		{ L"Copy", ProjectItemType::Copy },
		{ L"preset", ProjectItemType::ItemPreset },
	};

	const TypeDualConverter<TextureFilterType> ProjectUtils::TextureFilterTypeConv =
	{
		{ L"Nearest", TextureFilterType::Nearest },
		{ L"BSpline", TextureFilterType::BSpline },
		{ L"Box", TextureFilterType::Box }
	};

	const TypeDualConverter<TextureBuildMethod> ProjectUtils::TextureBuildMethodConv =
	{
		{ L"Default", TextureBuildMethod::BuiltIn },
		{ L"D3D", TextureBuildMethod::D3D },
		{ L"Devil", TextureBuildMethod::Devil }
	};

	const TypeDualConverter<TextureCompressionType> ProjectUtils::TextureCompressionTypeConv =
	{
		{ L"None", TextureCompressionType::None },
		{ L"LZ4", TextureCompressionType::LZ4 },
		{ L"RLE", TextureCompressionType::RLE }
	};

	const TypeDualConverter<MeshBuildMethod> ProjectUtils::MeshBuildMethodConv =
	{
		{ L"Ass", MeshBuildMethod::ASS },
		{ L"FBX", MeshBuildMethod::FBX },
		{ L"D3D", MeshBuildMethod::D3D }
	};

	const String ProjectUtils::BuildAttachmentSectionGUID = L"Attachment38d237b1976f42cab52e58570959a3ff";

}