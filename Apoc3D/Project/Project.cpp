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
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/ABCConfigurationFormat.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/VertexFormats.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Utility/TypeConverter.h"
#include "apoc3d/Utility/StringUtils.h"

#include <ctime>

using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void ProjectParseSubItems(Project* prj, List<ProjectItem*>& parentContainer, const ConfigurationSection* sect);
	void ProjectSaveSubItems(ConfigurationSection* parentSect, List<ProjectItem*>& items, bool savingBuild);
	void GenerateFolderPackaging(int& startNo, List<ConfigurationSection*>& result, List<ProjectItem*>& items);

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

		ProjectParseSubItems(m_project, SubItems, sect);

		for (ProjectItem* pi : SubItems)
			pi->SetParent(this);
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

	List<String> ProjectFolder::GetAllOutputFiles()  { return GetDestFileOutputSimple(DestinationPack); }

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
	bool ProjectResTexture::IsOutdated()
	{
		time_t destFileTime;
		
		if (IsSettingsNewerThan(DestinationFile, destFileTime))
			return true;
		
		if (AssembleCubemap || AssembleVolumeMap)
		{
			for (const String& srcFile : SubMapTable.getValueAccessor())
			{
				if (IsSourceFileNewer(srcFile, destFileTime))
					return true;
			}
		}
		else
		{
			if (IsSourceFileNewer(SourceFile, destFileTime))
				return true;
		}
		return false;
	}
	bool ProjectResTexture::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }

	List<String> ProjectResTexture::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }

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
	List<String> ProjectResMaterial::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }
	bool ProjectResMaterial::IsOutdated() { return IsSettingsNewerThan(DestinationFile); }
	bool ProjectResMaterial::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }

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

			List<String> names;
			for (ConfigurationSection* sub : mSect->getSubSections())
			{
				ParseMaterialTree(names, L"", sub);
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
	bool ProjectResMaterialSet::IsOutdated() { return IsOutdatedSimple(SourceFile, DestinationToken); }
	bool ProjectResMaterialSet::IsNotBuilt() { return IsDestFileNotBuilt(DestinationToken); }

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
	List<String> ProjectResFont::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResFont::IsOutdated() { return IsOutdatedSimple(SourceFile, DestFile); }
	bool ProjectResFont::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

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
	List<String> ProjectResFontGlyphDist::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResFontGlyphDist::IsOutdated() { return IsOutdatedSimple(SourceFile, DestFile); }
	bool ProjectResFontGlyphDist::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

	/************************************************************************/
	/*   ProjectResEffect                                                   */
	/************************************************************************/

	void ProjectResEffect::Parse(const ConfigurationSection* sect)
	{
		String srcDesc = sect->getAttribute(L"Source");
		List<String> srcSets;
		StringUtils::Split(srcDesc, srcSets, L"|");

		for (int i=0;i<srcSets.getCount();i++)
		{
			String e = srcSets[i];
			StringUtils::Trim(e);

			if (e.size() <= 3)
				continue;

			if (StringUtils::StartsWith(e, L"VS:"))
			{
				VS = e.substr(3);
				StringUtils::Trim(VS);
			}
			else if (StringUtils::StartsWith(e, L"PS:"))
			{
				PS = e.substr(3);
				StringUtils::Trim(PS);
			}
			else if (StringUtils::StartsWith(e, L"GS:"))
			{
				GS = e.substr(3);
				StringUtils::Trim(GS);
			}
			else if (StringUtils::StartsWith(e, L"ALL:"))
			{
				String fileName = e.substr(4);
				StringUtils::Trim(fileName);
				VS = PS = GS = fileName;
				break;
			}
		}

		String entryPointsDesc = sect->getAttribute(L"EntryPoints");
		srcSets.Clear();
		StringUtils::Split(entryPointsDesc, srcSets, L"|");
		for (int i = 0; i < srcSets.getCount(); i++)
		{
			String e = srcSets[i];
			StringUtils::Trim(e);

			if (e.size() <= 3)
				continue;

			if (StringUtils::StartsWith(e, L"VS:"))
			{
				EntryPointVS = e.substr(3);
				StringUtils::Trim(EntryPointVS);
			}
			else if (StringUtils::StartsWith(e, L"PS:"))
			{
				EntryPointPS = e.substr(3);
				StringUtils::Trim(EntryPointPS);
			}
			else if (StringUtils::StartsWith(e, L"GS:"))
			{
				EntryPointGS = e.substr(3);
				StringUtils::Trim(EntryPointGS);
			}
			else if (StringUtils::StartsWith(e, L"ALL:"))
			{
				String funcName = e.substr(4);
				StringUtils::Trim(funcName);
				EntryPointVS = EntryPointPS = EntryPointGS = funcName;
				break;
			}
		}

		DestFile = sect->getAttribute(L"DestinationFile");
		PListFile = sect->getAttribute(L"ParamList");

		StringUtils::Split(sect->getAttribute(L"Targets"), Targets, L"|");
		for (int i = 0; i < Targets.getCount(); i++)
		{
			StringUtils::Trim(Targets[i]);
			StringUtils::ToLowerCase(Targets[i]);
		}

		sect->TryGetAttributeBool(L"IsDebug", IsDebug);
		sect->TryGetAttributeBool(L"NoOptimization", NoOptimization);
	}
	void ProjectResEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (VS == PS && PS == GS)
		{
			sect->AddAttributeString(L"Source", L"ALL:" + WrapSourcePath(VS, savingBuild));
		}
		else
		{
			String srcText = L"VS:";
			srcText.append(WrapSourcePath(VS, savingBuild));

			srcText.append(L" | PS:");
			srcText.append(WrapSourcePath(PS, savingBuild));

			if (GS.size())
			{
				srcText.append(L" | GS:");
				srcText.append(WrapSourcePath(GS, savingBuild));
			}
			sect->AddAttributeString(L"Source", srcText);
		}

		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DestFile, savingBuild));
		sect->AddAttributeString(L"ParamList", WrapSourcePath(PListFile, savingBuild));

		if (EntryPointVS == EntryPointPS && EntryPointGS == EntryPointPS)
		{
			sect->AddAttributeString(L"EntryPoints", L"ALL:" + EntryPointVS);
		}
		else
		{
			String srcText = L"VS:";
			srcText.append(EntryPointVS);

			srcText.append(L" | PS:");
			srcText.append(EntryPointPS);

			if (GS.size())
			{
				srcText.append(L" | GS:");
				srcText.append(EntryPointGS);
			}
			sect->AddAttributeString(L"EntryPoints", srcText);
		}

		String targetsStr;
		for (int i = 0; i < Targets.getCount(); i++)
		{
			targetsStr.append(Targets[i]);
			if (i != Targets.getCount() - 1)
				targetsStr.append(L" | ");
		}
		sect->AddAttributeString(L"Targets", targetsStr);


		sect->AddAttributeBool(L"IsDebug", IsDebug);
		sect->AddAttributeBool(L"NoOptimization", NoOptimization);
	}

	List<String> ProjectResEffect::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResEffect::IsOutdated()
	{
		time_t destFileTime;
		if (IsSettingsNewerThan(DestFile, destFileTime))
			return true;

		if (VS.size())
		{
			if (IsSourceFileNewer(VS, destFileTime))
				return true;
		}
		if (PS.size())
		{
			if (IsSourceFileNewer(PS, destFileTime))
				return true;
		}
		if (GS.size())
		{
			if (IsSourceFileNewer(GS, destFileTime))
				return true;
		}
		if (PListFile.size())
		{
			if (IsSourceFileNewer(PListFile, destFileTime))
				return true;
		}
		return false;
	}
	bool ProjectResEffect::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

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
	List<String> ProjectResCustomEffect::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResCustomEffect::IsOutdated()
	{
		time_t destFileTime;
		if (IsSettingsNewerThan(DestFile, destFileTime))
			return true;

		if (IsSourceFileNewer(SrcVSFile, destFileTime))
			return true;

		if (IsSourceFileNewer(SrcPSFile, destFileTime))
			return true;
		
		return false;
	}
	bool ProjectResCustomEffect::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

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
	List<String> ProjectResEffectList::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResEffectList::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

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
	List<String> ProjectResShaderNetwork::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectResShaderNetwork::IsOutdated() { return IsOutdatedSimple(SrcFile, DestFile); }
	bool ProjectResShaderNetwork::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

	/************************************************************************/
	/*  ProjectResModel                                                     */
	/************************************************************************/

	void ProjectResModel::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		String method = L"ass";
		sect->tryGetAttribute(L"Method", method);
		
		Method = ProjectUtils::MeshBuildMethodConv.Parse(method);

		UseVertexFormatConversion = false;
		ConfigurationSection* subs = sect->getSection(L"VertexFormatConversion");
		if (subs && subs->getSubSectionCount()>0)
		{
			UseVertexFormatConversion = true;
			for (const ConfigurationSection* ent : subs->getSubSections())
			{
				VertexElementUsage usage = GraphicsCommonUtils::ParseVertexElementUsage(ent->getName());
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
		CollapseMeshs = false;
		sect->TryGetAttributeBool(L"CollapseMeshs", CollapseMeshs);
	}
	void ProjectResModel::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", WrapSourcePath(SrcFile, savingBuild));
		sect->AddAttributeString(L"DestinationFile", WrapDestinationPath(DstFile, savingBuild));
		if (DstAnimationFile.size())
		{
			sect->AddAttributeString(L"DestinationAnimFile", WrapDestinationPath(DstAnimationFile, savingBuild));
		}

		sect->AddAttributeString(L"Method", ProjectUtils::MeshBuildMethodConv.ToString(Method));

		if (UseVertexFormatConversion && ConversionVertexElements.getCount() > 0)
		{
			ConfigurationSection* subs = new ConfigurationSection(L"VertexFormatConversion");
			for (const VertexElement& ve : ConversionVertexElements)
			{
				ConfigurationSection* vs;
				if (ve.getUsage() == VEU_TextureCoordinate)
				{
					String idxText = StringUtils::IntToString(ve.getIndex());

					vs = new ConfigurationSection(GraphicsCommonUtils::ToString(ve.getUsage()) + idxText);
					vs->SetValue(idxText);
				}
				else
				{
					vs = new ConfigurationSection(GraphicsCommonUtils::ToString(ve.getUsage()));
				}

				subs->AddSection(vs);
			}
			sect->AddSection(subs);
		}
		

		if (CollapseMeshs)
		{
			sect->AddAttributeBool(L"CollapseMeshs", CollapseMeshs);
		}
	}
	List<String> ProjectResModel::GetAllOutputFiles()
	{
		List<String> e;
		if (DstAnimationFile.size())
			e.Add(WrapDestinationPath(DstAnimationFile, true));
		if (DstFile.size())
			e.Add(WrapDestinationPath(DstFile, true));
		return e;
	}
	bool ProjectResModel::IsOutdated()
	{
		time_t destFileTime;
		if (IsSettingsNewerThan(DstFile, destFileTime))
			return true;

		// check if the source file is newer than the built ones.
		String path = WrapSourcePath(SrcFile, true);
		if (File::FileExists(path))
		{
			time_t srcTime = File::GetFileModifiyTime(path);
			if (srcTime > destFileTime)
				return true;

			if (DstAnimationFile.size())
			{
				destFileTime = File::GetFileModifiyTime(WrapDestinationPath(DstAnimationFile, true));

				if (srcTime > destFileTime)
					return true;
			}
		}

		return false;
	}
	bool ProjectResModel::IsNotBuilt()
	{
		if (DstAnimationFile.size())
			if (!File::FileExists(WrapDestinationPath(DstAnimationFile, true)))
				return true;

		return !File::FileExists(WrapDestinationPath(DstFile, true));
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
	List<String> ProjectResMAnim::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }
	bool ProjectResMAnim::IsOutdated() { return IsOutdatedSimple(SourceFile, DestinationFile); }
	bool ProjectResMAnim::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }


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
	List<String> ProjectResTAnim::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }
	bool ProjectResTAnim::IsOutdated() { return IsOutdatedSimple(SourceFile, DestinationFile); }
	bool ProjectResTAnim::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }

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
	List<String> ProjectResUILayout::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }
	bool ProjectResUILayout::IsOutdated() { return IsSettingsNewerThan(DestinationFile); }
	bool ProjectResUILayout::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }

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
	List<String> ProjectResCopy::GetAllOutputFiles() { return GetDestFileOutputSimple(DestinationFile); }
	bool ProjectResCopy::IsOutdated() { return IsSettingsNewerThan(DestinationFile); }
	bool ProjectResCopy::IsNotBuilt() { return IsDestFileNotBuilt(DestinationFile); }


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
		//SourceFile = sect->getAttribute(L"SourceFile");
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

	List<String> ProjectCustomItem::GetAllOutputFiles() { return GetDestFileOutputSimple(DestFile); }
	bool ProjectCustomItem::IsOutdated() { return IsSettingsNewerThan( DestFile); }
	bool ProjectCustomItem::IsNotBuilt() { return IsDestFileNotBuilt(DestFile); }

	String ProjectItemData::GetAbsoluteSourcePathBase() const
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
		
		return m_project->getBasePath();
	}
	String ProjectItemData::GetAbsoluteDestinationPathBase() const
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

		return m_project->getOutputPath();
	}

	String ProjectItemData::GetAbsoluteSourcePath(const String& path) const { return PathUtils::Combine(GetAbsoluteSourcePathBase(), path); }
	String ProjectItemData::GetAbsoluteDestinationPath(const String& path) const { return PathUtils::Combine(GetAbsoluteDestinationPathBase(), path); }

	bool ProjectItemData::IsSourceFileNewer(const String& srcFile, time_t t) const
	{
		String p = WrapSourcePath(srcFile, true);
		return File::FileExists(p) && File::GetFileModifiyTime(p) > t;
	}

	bool ProjectItemData::IsSettingsNewerThan(const String& destinationFile) const
	{
		time_t t;
		return IsSettingsNewerThan(destinationFile, t);
	}
	bool ProjectItemData::IsSettingsNewerThan(const String& destinationFile, time_t& dstFileTime) const
	{
		String p = WrapDestinationPath(destinationFile, true);

		if (!File::FileExists(p))
			return true;

		dstFileTime = File::GetFileModifiyTime(p);
		return m_parentItem->isSettingsNewerThan(dstFileTime);
	}

	bool ProjectItemData::IsOutdatedSimple(const String& srcFile, const String& destinationFile) const
	{
		time_t destFileTime;
		if (IsSettingsNewerThan(destinationFile, destFileTime))
			return true;

		if (IsSourceFileNewer(srcFile, destFileTime))
			return true;

		return false;
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

	List<String> ProjectItemData::GetDestFileOutputSimple(const String& destinationFile)
	{
		List<String> e;
		if (destinationFile.size())
			e.Add(WrapDestinationPath(destinationFile, true));
		return e;
	}

	bool ProjectItemData::IsDestFileNotBuilt(const String& destinationFile)
	{
		return !File::FileExists(WrapDestinationPath(destinationFile, true));
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
		if (savingBuild && 
			(m_typeData->getType() == ProjectItemType::Model || 
			 m_typeData->getType() == ProjectItemType::Texture || 
			 m_typeData->getType() == ProjectItemType::MaterialSet || 
			 m_typeData->getType() == ProjectItemType::TransformAnimation) &&
			!IsOutDated())
		{
			return 0;
		}
		ConfigurationSection* sect = new ConfigurationSection(m_name);

		if (m_typeData)
		{
			sect->AddAttributeString(L"Type", ProjectUtils::ProjectItemTypeConv.ToString(m_typeData->getType()));
			
			m_typeData->Save(sect, savingBuild);
		}

		if (!savingBuild)
		{
			sect->AddAttributeString(L"LastModTime", StringUtils::IntToString(m_timeStamp));
		}
		return sect;
	}

	void ProjectItem::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getName();

		if (sect->hasAttribute(L"LastModTime"))
		{
			m_timeStamp = StringUtils::ParseInt64(sect->getAttribute(L"LastModTime"));
		}
		else
		{
			m_timeStamp = time(0);
		}
		

		ProjectItemType itemType = ProjectUtils::ProjectItemTypeConv.Parse(sect->getAttribute(L"Type"));
		
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
		default:
			assert(0);
		}

		m_typeData->Parse(sect);
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

		ProjectParseSubItems(this, m_items, sect);
	}
	void Project::Save(const String& file)
	{
		ConfigurationSection* s = Save();

		Configuration* xc = new Configuration(m_name);
		xc->Add(s);
		//xc->Save(file);

		XMLConfigurationFormat::Instance.Save(xc, FileOutStream(file));
		
		delete xc;
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
	void ProjectParseSubItems(Project* prj, List<ProjectItem*>& parentContainer, const ConfigurationSection* sect)
	{
		for (ConfigurationSection* ss : sect->getSubSections())
		{
			ProjectItem* item = new ProjectItem(prj);

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

	void ProjectItem::NotifyModified() { m_timeStamp = time(0); }

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