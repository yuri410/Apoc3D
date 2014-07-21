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
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/ABCConfigurationFormat.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Collections/EnumConverterHelper.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/VertexFormats.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Math/RandomUtils.h"


#include <ctime>

using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void RecursivePassFolderPacks(int& startNo, List<ConfigurationSection*>& result, List<ProjectItem*>& items);
	void ProjectParse(Project* prj, List<ProjectItem*>& parentContainer, const ConfigurationSection* sect);
	void ProjectSave(ConfigurationSection* parentSect, List<ProjectItem*>& items, bool savingBuild);

	/************************************************************************/
	/*  ProjectFolder                                                       */
	/************************************************************************/

	void ProjectFolder::Parse(const ConfigurationSection* sect)
	{
		sect->tryGetAttribute(L"Pack", PackType);
		sect->TryGetAttributeBool(L"IncludeUnpackedSubFolderItems", IncludeUnpackedSubFolderItems);

		if (PackType.size())
		{
			DestinationPack = sect->getAttribute(L"DestinationPack");
		}

		ProjectParse(m_project, SubItems,sect);
	}
	void ProjectFolder::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (PackType.size())
		{
			sect->AddAttributeString(L"Pack", PackType);
			sect->AddAttributeString(L"DestinationPack", DestinationPack);
			sect->AddAttributeBool(L"IncludeUnpackedSubFolderItems", IncludeUnpackedSubFolderItems);
		}
		ProjectSave(sect, SubItems, savingBuild);
	}
	void ProjectFolder::SavePackBuildConfig(ConfigurationSection* sect)
	{
		sect->AddAttributeString(L"Type", PackType);
		sect->AddAttributeString(L"DestinationFile", PathUtils::Combine(m_project->getOutputPath(),DestinationPack));

		// a package file/archive build config section needs every item's path
		int pakEntryIndex = 0;
		AddPackageEntries(sect, pakEntryIndex);
		/*for (int i=0;i<SubItems.getCount();i++)
		{
			if (SubItems[i]->getData())
			{
				ProjectItemData* item = SubItems[i]->getData();

				List<String> itemOutputs = item->GetAllOutputFiles();
				for (int32 j=0;j<itemOutputs.getCount();j++)
				{
					ConfigurationSection* e = new ConfigurationSection(L"Entry" + StringUtils::ToString(pakEntryIndex++));
					e->AddAttributeString(L"FilePath", itemOutputs[j]);
					sect->AddSection(e);
				}
			}
		}*/
	}
	void ProjectFolder::AddPackageEntries(ConfigurationSection* sect, int32& idx)
	{
		// this will also search sub folder which is not packed
		for (int i=0;i<SubItems.getCount();i++)
		{
			if (SubItems[i]->getData())
			{
				ProjectItemData* item = SubItems[i]->getData();

				if (item->getType() == ProjectItemType::Folder)
				{
					ProjectFolder* fol = static_cast<ProjectFolder*>(item);
					if (fol->PackType.empty() && IncludeUnpackedSubFolderItems)
					{
						fol->AddPackageEntries(sect, idx);
					}
				}

				List<String> itemOutputs = item->GetAllOutputFiles();
				for (int32 j=0;j<itemOutputs.getCount();j++)
				{
					ConfigurationSection* e = new ConfigurationSection(L"Entry" + StringUtils::IntToString(idx++));
					e->AddAttributeString(L"FilePath", itemOutputs[j]);
					sect->AddSection(e);
				}
			}
		}
	}

	List<String> ProjectFolder::GetAllOutputFiles()  { return simpleGetAllOutputFiles(DestinationPack); }

	/************************************************************************/
	/*  ProjectResTexture                                                   */
	/************************************************************************/

	void ProjectResTexture::Parse(const ConfigurationSection* sect)
	{
		String method = L"d3d";
		sect->tryGetAttribute(L"Method", method);

		Method = ProjectTypeUtils::ParseTextureBuildMethod(method);
		AssembleCubemap = false;
		AssembleVolumeMap = false;

		String assembleType;
		if (!sect->tryGetAttribute(L"Assemble", assembleType))
		{
			SourceFile = sect->getAttribute(L"SourceFile");
		}
		else
		{
			StringUtils::ToLowerCase(assembleType);
			if (assembleType == L"cubemap")
			{
				AssembleCubemap = true;
				AssembleVolumeMap = false;

				String file = sect->getAttribute(L"NegX");
				SubMapTable.Add((uint)CUBE_NegativeX, file);
				file = sect->getAttribute(L"NegY");
				SubMapTable.Add((uint)CUBE_NegativeY, file);
				file = sect->getAttribute(L"NegZ");
				SubMapTable.Add((uint)CUBE_NegativeZ, file);

				file = sect->getAttribute(L"PosX");
				SubMapTable.Add((uint)CUBE_PositiveX, file);
				file = sect->getAttribute(L"PosY");
				SubMapTable.Add((uint)CUBE_PositiveY, file);
				file = sect->getAttribute(L"PosZ");
				SubMapTable.Add((uint)CUBE_PositiveZ, file);

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

				uint i =0;
				ConfigurationSection* srcsect = sect->getSection(L"Source");
				for (ConfigurationSection::SubSectionEnumerator iter = srcsect->GetSubSectionEnumrator();
					iter.MoveNext();)
				{
					const ConfigurationSection* ss = *iter.getCurrentValue();

					SubMapTable.Add(i, ss->getAttribute(L"FilePath"));
					i++;
				}
				srcsect = sect->getSection(L"AlphaSource");
				i=0;
				for (ConfigurationSection::SubSectionEnumerator iter = srcsect->GetSubSectionEnumrator();
					iter.MoveNext();)
				{
					const ConfigurationSection* ss = *iter.getCurrentValue();

					SubAlphaMapTable.Add(i, ss->getAttribute(L"FilePath"));
					i++;
				}

			}

		}

		DestinationFile = sect->getAttribute(L"DestinationFile");

		GenerateMipmaps = false;
		sect->TryGetAttributeBool(L"GenerateMipmaps", GenerateMipmaps);

		bool passed = false;
		passed |= sect->TryGetAttributeInt(L"Width", NewWidth);
		passed |= sect->TryGetAttributeInt(L"Height", NewHeight);
		passed |= sect->TryGetAttributeInt(L"Depth", NewDepth);

		ResizeFilterType = TextureFilterType::BSpline;
		String flt;
		if (sect->tryGetAttribute(L"ResizeFilter", flt))
		{
			ResizeFilterType = ProjectTypeUtils::ParseTextureFilterType(flt);
		}

		Resize = passed;

		NewFormat = FMT_Unknown;
		String fmt;
		if (sect->tryGetAttribute(L"PixelFormat", fmt))
		{
			NewFormat = PixelFormatUtils::ConvertFormat(fmt);
		}

		CompressionType = TextureCompressionType::None;
		String cmp;
		if (sect->tryGetAttribute(L"Compression", cmp))
		{
			StringUtils::ToLowerCase(cmp);
			if (cmp == L"rle")
			{
				CompressionType = TextureCompressionType::RLE;
			}
			else if (cmp == L"lz4")
			{
				CompressionType = TextureCompressionType::LZ4;
			}
		}

	}
	void ProjectResTexture::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"Method", ProjectTypeUtils::ToString(Method));

		if (AssembleCubemap || AssembleVolumeMap)
		{
			if (AssembleCubemap)
			{
				sect->AddAttributeString(L"Assemble", L"cubemap");

				sect->AddAttributeString(L"NegX", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeX]) : SubMapTable[CUBE_NegativeX]);
				sect->AddAttributeString(L"NegY", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeY]) : SubMapTable[CUBE_NegativeY]);
				sect->AddAttributeString(L"NegZ", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeZ]) :SubMapTable[CUBE_NegativeZ]);
				sect->AddAttributeString(L"PosX", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveX]) :SubMapTable[CUBE_PositiveX]);
				sect->AddAttributeString(L"PosY", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveY]) :SubMapTable[CUBE_PositiveY]);
				sect->AddAttributeString(L"PosZ", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveZ]) :SubMapTable[CUBE_PositiveZ]);

				if (SubAlphaMapTable.Contains(CUBE_NegativeX))
				{
					sect->AddAttributeString(L"NegXAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeX]) : SubAlphaMapTable[CUBE_NegativeX]);
				}
				if (SubAlphaMapTable.Contains(CUBE_NegativeY))
				{
					sect->AddAttributeString(L"NegYAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeY]) : SubAlphaMapTable[CUBE_NegativeY]);
				}
				if (SubAlphaMapTable.Contains(CUBE_NegativeZ))
				{
					sect->AddAttributeString(L"NegZAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeZ]) : SubAlphaMapTable[CUBE_NegativeZ]);
				}

				if (SubAlphaMapTable.Contains(CUBE_PositiveX))
				{
					sect->AddAttributeString(L"PosXAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveX]) : SubAlphaMapTable[CUBE_PositiveX]);
				}
				if (SubAlphaMapTable.Contains(CUBE_PositiveY))
				{
					sect->AddAttributeString(L"PosYAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveY]) : SubAlphaMapTable[CUBE_PositiveY]);
				}
				if (SubAlphaMapTable.Contains(CUBE_PositiveZ))
				{
					sect->AddAttributeString(L"PosZAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveZ]) : SubAlphaMapTable[CUBE_PositiveZ]);
				}
			}
			else
			{
				sect->AddAttributeString(L"Assemble", L"volume");
				
				ConfigurationSection* srcsect = new ConfigurationSection(L"Source");
				for (int32 i=0;i<SubMapTable.getCount();i++)
				{
					ConfigurationSection* es = new ConfigurationSection(String(L"Slice") + StringUtils::IntToString(i));
					es->AddAttributeString(L"FilePath", SubMapTable[i]);

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);

				srcsect = new ConfigurationSection(L"AlphaSource");
				for (int32 i=0;i<SubAlphaMapTable.getCount();i++)
				{
					ConfigurationSection* es = new ConfigurationSection(String(L"Slice") + StringUtils::IntToString(i));
					es->AddAttributeString(L"FilePath", SubAlphaMapTable[i]);

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);
			}
			
		}
		else
		{
			sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(),SourceFile) : SourceFile);
		}
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(),DestinationFile) : DestinationFile);
		if (GenerateMipmaps)
			sect->AddAttributeString(L"GenerateMipmaps", StringUtils::BoolToString(GenerateMipmaps));

		if (Resize)
		{
			sect->AddAttributeString(L"Width", StringUtils::IntToString(NewWidth));
			sect->AddAttributeString(L"Height", StringUtils::IntToString(NewHeight));
			sect->AddAttributeString(L"Depth", StringUtils::IntToString(NewDepth));

			sect->AddAttributeString(L"ResizeFilter", ProjectTypeUtils::ToString(ResizeFilterType));
		}

		if (NewFormat != FMT_Unknown)
		{
			sect->AddAttributeString(L"PixelFormat", PixelFormatUtils::ToString(NewFormat));
		}

		if (CompressionType != TextureCompressionType::None)
		{
			if (CompressionType == TextureCompressionType::RLE)
			{
				sect->AddAttributeString(L"Compression", L"RLE");
			}
			else if (CompressionType == TextureCompressionType::LZ4)
			{
				sect->AddAttributeString(L"Compression", L"LZ4");
			}
		}
	}
	List<String> ProjectResTexture::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResTexture::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestinationFile));

		if (destFileTime < t)
			return true;
		
		if (AssembleCubemap || AssembleVolumeMap)
		{
			for (int i=0;i<SubMapTable.getCount();i++)
			{
				String path = PathUtils::Combine(m_project->getBasePath(),SubMapTable[i]);
				if (File::FileExists(path))
				{
					if (File::GetFileModifiyTime(path) > destFileTime)
					{
						return true;
					}
				}
			}
			//File::GetFileModifiyTime(SourceFile);
		}
		else
		{
			String path = PathUtils::Combine(m_project->getBasePath(), SourceFile);
			if (File::FileExists(path))
			{
				if (File::GetFileModifiyTime(path) > destFileTime)
				{
					return true;
				}
			}
		}
		return false;
	}
	bool ProjectResTexture::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }
	
	/************************************************************************/
	/*  ProjectResMaterial                                                  */
	/************************************************************************/

	void ProjectResMaterial::Parse(const ConfigurationSection* sect)
	{
		DestinationFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResMaterial::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(),DestinationFile) : DestinationFile);
	}
	List<String> ProjectResMaterial::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResMaterial::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestinationFile); }
	bool ProjectResMaterial::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }

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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationLocation", savingBuild ? PathUtils::Combine(m_project->getOutputPath(),DestinationLocation) : DestinationLocation);
		sect->AddAttributeString(L"DestinationToken", savingBuild ? PathUtils::Combine(m_project->getOutputPath(),DestinationToken) : DestinationToken);
	}

	void ParseMaterialTree(List<String>& mtrlList, const String& baseMtrlName, const ConfigurationSection* sect)
	{
		String name = baseMtrlName;
		if (name.size())
		{
			name.append(L"_");
		}
		name.append(sect->getName());

		// go into sub sections
		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(mtrlList, name, *e.getCurrentValue());
		}

		mtrlList.Add(name);
	}

	List<String> ProjectResMaterialSet::GetAllOutputFiles()
	{
		List<String> res;

		String path = PathUtils::Combine(m_project->getBasePath(), SourceFile);
		if (File::FileExists(path))
		{
			FileLocation floc(path);
			Configuration* config = XMLConfigurationFormat::Instance.Load(floc);
			ConfigurationSection* mSect = config->get(L"Materials");

			List<String> names;
			for (ConfigurationSection::SubSectionEnumerator e = mSect->GetSubSectionEnumrator(); e.MoveNext();)
			{
				ParseMaterialTree(names, L"", *e.getCurrentValue());
			} 

			delete config;

			String basePath = PathUtils::Combine(m_project->getOutputPath(), DestinationLocation);

			for (int i=0;i<names.getCount();i++)
			{
				res.Add(PathUtils::Combine(basePath, names[i] + L".mtrl"));
			}
		}

		if (DestinationToken.size())
			res.Add(PathUtils::Combine(m_project->getOutputPath(),DestinationToken));
		return res;
	}
	bool ProjectResMaterialSet::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestinationToken));

		if (destFileTime < t)
			return true;

		String path = PathUtils::Combine(m_project->getBasePath(), SourceFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		return false;
	}
	bool ProjectResMaterialSet::IsNotBuilt() { return simpleIsNotBuilt(DestinationToken); }

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

		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}
	}
	void ProjectResFont::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"Size", StringUtils::SingleToString(Size));

		sect->AddAttributeBool(L"AntiAlias", AntiAlias);

		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);

		for (int32 i=0;i<Ranges.getCount();i++)
		{
			ConfigurationSection* ss = new ConfigurationSection(String(L"Range") + StringUtils::IntToString(i));
			ss->AddAttributeString(L"Start", StringUtils::IntToString( Ranges[i].MinChar));
			ss->AddAttributeString(L"End", StringUtils::IntToString( Ranges[i].MaxChar));

			sect->AddSection(ss);
		}
	}
	List<String> ProjectResFont::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResFont::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestFile); }
	bool ProjectResFont::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
	}
	List<String> ProjectResFontGlyphDist::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResFontGlyphDist::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile));

		if (destFileTime < t)
			return true;

		String path = PathUtils::Combine(m_project->getBasePath(), SourceFile);
		if (File::FileExists(path))
			if (File::GetFileModifiyTime(path) > destFileTime)
				return true;
		return false;
	}
	bool ProjectResFontGlyphDist::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }
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
		for (int i=0;i<srcSets.getCount();i++)
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
		for (int i=0;i<Targets.getCount();i++)
		{
			StringUtils::Trim(Targets[i]);
			StringUtils::ToLowerCase(Targets[i]);
		}
	}
	void ProjectResEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		if (VS == PS && PS == GS)
		{
			sect->AddAttributeString(L"Source", L"ALL:" + (savingBuild ? PathUtils::Combine(m_project->getBasePath(), VS) : VS));
		}
		else
		{
			String srcText = L"VS:";
			srcText.append(savingBuild ? PathUtils::Combine(m_project->getBasePath(), VS) : VS);

			srcText.append(L" | PS:");
			srcText.append(savingBuild ? PathUtils::Combine(m_project->getBasePath(), PS) : PS);

			if (GS.size())
			{
				srcText.append(L" | GS:");
				srcText.append(savingBuild ? PathUtils::Combine(m_project->getBasePath(), GS) : GS);
			}
			sect->AddAttributeString(L"Source", srcText);
		}

		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
		sect->AddAttributeString(L"ParamList", savingBuild ? PathUtils::Combine(m_project->getBasePath(), PListFile) : PListFile);

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
		for (int i=0;i<Targets.getCount();i++)
		{
			targetsStr.append(Targets[i]);
			if (i!=Targets.getCount()-1)
				targetsStr.append(L" | ");
		}
		sect->AddAttributeString(L"Targets", targetsStr);
	}
	List<String> ProjectResEffect::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResEffect::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile));

		if (destFileTime < t)
			return true;

		if (VS.size())
		{
			String path = PathUtils::Combine(m_project->getBasePath(), VS);
			if (File::FileExists(path))
				if (File::GetFileModifiyTime(path) > destFileTime)
					return true;
		}
		if (PS.size())
		{
			String path = PathUtils::Combine(m_project->getBasePath(), PS);
			if (File::FileExists(path))
				if (File::GetFileModifiyTime(path) > destFileTime)
					return true;
		}
		if (GS.size())
		{
			String path = PathUtils::Combine(m_project->getBasePath(), GS);
			if (File::FileExists(path))
				if (File::GetFileModifiyTime(path) > destFileTime)
					return true;
		}
		if (PListFile.size())
		{
			String path = PathUtils::Combine(m_project->getBasePath(), PListFile);
			if (File::FileExists(path))
			{
				if (File::GetFileModifiyTime(path) > destFileTime)
					return true;
			}
		}
		return false;
	}
	bool ProjectResEffect::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

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
		sect->AddAttributeString(L"VSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcVSFile) : SrcVSFile);
		sect->AddAttributeString(L"PSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcPSFile) : SrcPSFile);

		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
		sect->AddAttributeString(L"EntryPointVS", EntryPointVS);
		sect->AddAttributeString(L"EntryPointPS", EntryPointPS);
		sect->AddAttributeString(L"Profile", Profile);
	}
	List<String> ProjectResCustomEffect::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResCustomEffect::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile));

		if (destFileTime < t)
			return true;

		String path = PathUtils::Combine(m_project->getBasePath(), SrcVSFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		path = PathUtils::Combine(m_project->getBasePath(), SrcPSFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		return false;
	}
	bool ProjectResCustomEffect::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

	/************************************************************************/
	/*  ProjectResEffectList                                                */
	/************************************************************************/

	// Finds all effects in the project
	void WalkProject(const List<ProjectItem*>& items, List<String>& effectsFound)
	{
		for (int i=0;i<items.getCount();i++)
		{
			if (items[i]->getType() == ProjectItemType::Effect)
			{
				effectsFound.Add(items[i]->getName());
			}
			else if (items[i]->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(items[i]->getData());

				WalkProject(folder->SubItems, effectsFound);
			}
		}
	}

	void ProjectResEffectList::Parse(const ConfigurationSection* sect)
	{
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResEffectList::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);

		if (savingBuild)
		{
			List<String> effectList;
			WalkProject(m_project->getItems(), effectList);

			for (int i=0;i<effectList.getCount();i++)
			{
				ConfigurationSection* ss = new ConfigurationSection(effectList[i]);
				sect->AddSection(ss);
			}
		}
		
	}
	List<String> ProjectResEffectList::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResEffectList::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
	}
	List<String> ProjectResShaderNetwork::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectResShaderNetwork::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile));

		if (destFileTime < t)
			return true;

		String path = PathUtils::Combine(m_project->getBasePath(), SrcFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		return false;
	}
	bool ProjectResShaderNetwork::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

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
		StringUtils::ToLowerCase(method);

		Method = ProjectTypeUtils::ParseModelBuildMethod(method);

		UseVertexFormatConversion = false;
		ConfigurationSection* subs = sect->getSection(L"VertexFormatConversion");
		if (subs && subs->getSubSectionCount()>0)
		{
			UseVertexFormatConversion = true;
			for (ConfigurationSection::SubSectionEnumerator iter = subs->GetSubSectionEnumrator();
				iter.MoveNext();)
			{
				const ConfigurationSection* ent = *iter.getCurrentValue();

				VertexElementUsage usage = GraphicsCommonUtils::ParseVertexElementUsage(ent->getName());
				int index = 0;
				if (usage == VEU_TextureCoordinate)
				{
					index = StringUtils::ParseInt32( ent->getValue() );
				}
				// the vertex elements here only has usage and index. 
				// They only store info here, not for normal use in graphics
				VertexElements.Add(VertexElement(0,VEF_Count,usage,index));
			}
		}
		CollapseMeshs = false;
		sect->TryGetAttributeBool(L"CollapseMeshs", CollapseMeshs);
	}
	void ProjectResModel::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DstFile) : DstFile);
		if (DstAnimationFile.size())
		{
			sect->AddAttributeString(L"DestinationAnimFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DstAnimationFile) :DstAnimationFile);
		}
		sect->AddAttributeString(L"Method", ProjectTypeUtils::ToString(Method));

		if (UseVertexFormatConversion && VertexElements.getCount() > 0)
		{
			ConfigurationSection* subs = new ConfigurationSection(L"VertexFormatConversion");
			for (int i=0;i<VertexElements.getCount();i++)
			{
				const VertexElement& ve = VertexElements[i];

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
			e.Add(PathUtils::Combine(m_project->getOutputPath(),DstAnimationFile));
		if (DstFile.size())
			e.Add(PathUtils::Combine(m_project->getOutputPath(),DstFile));
		return e;
	}
	bool ProjectResModel::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DstFile));

		if (destFileTime < t)
			return true;

		// check if the source file is newer than the built ones.
		String path = PathUtils::Combine(m_project->getBasePath(), SrcFile);
		if (File::FileExists(path))
		{
			time_t srcTime = File::GetFileModifiyTime(path);
			if (srcTime > destFileTime)
			{
				return true;
			}
			if (DstAnimationFile.size())
			{
				destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DstAnimationFile));

				if (srcTime > destFileTime)
					return true;
			}
		}

		return false;
	}
	bool ProjectResModel::IsNotBuilt()
	{
		if (DstAnimationFile.size())
			if (!File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DstAnimationFile)))
				return true;

		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DstFile)) ;
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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestinationFile) : DestinationFile);
	}
	List<String> ProjectResMAnim::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResMAnim::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestinationFile); }
	bool ProjectResMAnim::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }


	/************************************************************************/
	/*    ProjectResTAnim                                                   */
	/************************************************************************/

	void ProjectResTAnim::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");
		Reverse = false;
		sect->TryGetAttributeBool(L"Reverse", Reverse);

		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			String name = ss->getName();
			int objIdx = StringUtils::ParseInt32(ss->getValue());

			ObjectIndexMapping.Add(name, objIdx);
		}
	}
	void ProjectResTAnim::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestinationFile) : DestinationFile);
		sect->AddAttributeString(L"Reverse", StringUtils::BoolToString(Reverse));

		for (HashMap<String, int>::Enumerator e = ObjectIndexMapping.GetEnumerator(); e.MoveNext(); )
		{
			ConfigurationSection* ss = new ConfigurationSection(*e.getCurrentKey());
			
			ss->SetValue(StringUtils::IntToString(*e.getCurrentValue()));

			sect->AddSection(ss);
		}
	}
	List<String> ProjectResTAnim::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResTAnim::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(), DestinationFile));

		if (destFileTime < t)
			return true;

		String path = PathUtils::Combine(m_project->getBasePath(), SourceFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		return false;
	}
	bool ProjectResTAnim::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }

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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestinationFile) : DestinationFile);
	}
	List<String> ProjectResUILayout::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResUILayout::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestinationFile); }
	bool ProjectResUILayout::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }

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
		sect->AddAttributeString(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SourceFile) : SourceFile);
		sect->AddAttributeString(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestinationFile) : DestinationFile);
	}
	List<String> ProjectResCopy::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestinationFile); }
	bool ProjectResCopy::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestinationFile); }
	bool ProjectResCopy::IsNotBuilt() { return simpleIsNotBuilt(DestinationFile); }


	/************************************************************************/
	/*    ProjectCustomItem                                                 */
	/************************************************************************/

	void ProjectCustomItem::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"EditorExtension", EditorExtension);

		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			Properties.Add(*e.getCurrentKey(), (*e.getCurrentValue())->getValue());
		}
	}
	void ProjectCustomItem::Save(ConfigurationSection* sect, bool savingBuild)
	{
		//SourceFile = sect->getAttribute(L"SourceFile");
		sect->AddAttributeString(L"SourceFile", SourceFile);
		sect->AddAttributeString(L"DestinationFile", DestFile);
		if (EditorExtension.size())
		{
			sect->AddAttributeString(L"EditorExtension", EditorExtension);
		}

		for (HashMap<String, String>::Enumerator e = Properties.GetEnumerator(); e.MoveNext();)
		{
			ConfigurationSection* valSect = new ConfigurationSection(*e.getCurrentKey());
			valSect->SetValue(*e.getCurrentValue());

			sect->AddSection(valSect);
		}
	}

	List<String> ProjectCustomItem::GetAllOutputFiles() { return simpleGetAllOutputFiles(DestFile); }
	bool ProjectCustomItem::IsEarlierThan(time_t t) { return simpleIsEarlierThan(t, DestFile); }
	bool ProjectCustomItem::IsNotBuilt() { return simpleIsNotBuilt(DestFile); }

	List<String> ProjectItemData::simpleGetAllOutputFiles(const String& destinationFile)
	{
		List<String> e;
		if (destinationFile.size())
			e.Add(PathUtils::Combine(m_project->getOutputPath(),destinationFile));
		return e;
	}
	bool ProjectItemData::simpleIsEarlierThan(time_t t, const String& destinationFile)
	{
		return File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),destinationFile)) < t;
	}
	bool ProjectItemData::simpleIsNotBuilt(const String& destinationFile)
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),destinationFile));
	}

	/************************************************************************/
	/*    ProjectItem                                                       */
	/************************************************************************/

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
			sect->AddAttributeString(L"Type", ProjectTypeUtils::ToString(m_typeData->getType()));
			
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
			m_timeStamp = StringUtils::ParseInt64( sect->getAttribute(L"LastModTime"));
		}
		else
		{
			m_timeStamp = time(0);
		}
		

		ProjectItemType itemType = ProjectTypeUtils::ParseProjectItemType(sect->getAttribute(L"Type"));
		//StringUtils::ToLowerCase(buildType);

		switch (itemType)
		{
		case ProjectItemType::Custom:
			m_typeData = new ProjectCustomItem(m_project);
			break;
		case ProjectItemType::Folder:
			m_typeData = new ProjectFolder(m_project);
			break;
		case ProjectItemType::Material:
			m_typeData = new ProjectResMaterial(m_project);
			break;
		case ProjectItemType::MaterialSet:
			m_typeData = new ProjectResMaterialSet(m_project);
			break;
		case ProjectItemType::Texture:
			m_typeData = new ProjectResTexture(m_project);
			break;
		case ProjectItemType::Model:
			m_typeData = new ProjectResModel(m_project);
			break;
		case ProjectItemType::TransformAnimation:
			m_typeData = new ProjectResTAnim(m_project);
			break;
		case ProjectItemType::MaterialAnimation:
			m_typeData = new ProjectResMAnim(m_project);
			break;
		case ProjectItemType::Effect:
			m_typeData = new ProjectResEffect(m_project);
			break;
		case ProjectItemType::EffectList:
			m_typeData = new ProjectResEffectList(m_project);
			break;
		case ProjectItemType::CustomEffect:
			m_typeData = new ProjectResCustomEffect(m_project);
			break;
		case ProjectItemType::ShaderNetwork:
			m_typeData = new ProjectResShaderNetwork(m_project);
			break;
		case ProjectItemType::Font:
			m_typeData = new ProjectResFont(m_project);
			break;
		case ProjectItemType::FontGlyphDist:
			m_typeData = new ProjectResFontGlyphDist(m_project);
			break;
		case ProjectItemType::UILayout:
			m_typeData = new ProjectResUILayout(m_project);
			break;
		case ProjectItemType::Copy:
			m_typeData = new ProjectResCopy(m_project);
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

		ProjectParse(this, m_items, sect);
	}
	void Project::Save(const String& file)
	{
		ConfigurationSection* s = Save();

		Configuration* xc = new Configuration(m_name);
		xc->Add(s);
		//xc->Save(file);
		FileOutStream* fs = new FileOutStream(file);
		XMLConfigurationFormat::Instance.Save(xc, fs);
		
		delete xc;
	}

	void RecursivePassFolderPacks(int& startNo, ConfigurationSection* parentSect, List<ProjectItem*>& items);

	ConfigurationSection* Project::Save()
	{
		ConfigurationSection* sect = new ConfigurationSection(L"Project", m_items.getCount() * 2);

		sect->AddAttributeString(L"Name", m_name);
		sect->AddAttributeString(L"TexturePath", m_texturePath);
		sect->AddAttributeString(L"MaterialPath", m_materialPath);

		if (m_originalOutputPath.size())
			sect->AddAttributeString(L"ExplicitBuildPath", m_originalOutputPath);
		ProjectSave(sect, m_items, false);

		return sect;
	}
	void Project::GenerateBuildScripts(List<ConfigurationSection*>& result)
	{
		ConfigurationSection* sect = new ConfigurationSection(L"Build", m_items.getCount() * 2);

		sect->AddAttributeString(L"Name", m_name);

		ProjectSave(sect, m_items, true);

		sect->AddAttributeString(L"Type", L"Project");
		result.Add(sect);
		
		int32 startNo = Randomizer::NextInclusive(65535);
		RecursivePassFolderPacks(startNo, result, m_items);
		
	}
	void RecursivePassFolderPacks(int& startNo, List<ConfigurationSection*>& result, List<ProjectItem*>& items)
	{
		// post traversal on the project tree will make leaf folder to pack file builds comes first
		for (int32 i=0;i<items.getCount();i++)
		{
			if (items[i]->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(items[i]->getData());

				if (fld)
				{
					RecursivePassFolderPacks(startNo, result, fld->SubItems);

					if (fld->PackType.size())
					{
						ConfigurationSection* s = new ConfigurationSection(L"Archive_" + StringUtils::IntToString(startNo++));
						fld->SavePackBuildConfig(s);
						//parentSect->AddSection(s);

						// these packaging builds will be added to the end, as the prerequisite items should be built first
						result.Add(s);
					}
				}
			}
		}
	}

	// this function is for parsing all sub items in a section
	void ProjectParse(Project* prj, List<ProjectItem*>& parentContainer, const ConfigurationSection* sect)
	{
		for (ConfigurationSection::SubSectionEnumerator iter =  sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			ConfigurationSection* sect = *iter.getCurrentValue();

			ProjectItem* item = new ProjectItem(prj);

			item->Parse(sect);

			parentContainer.Add(item);

		}
	}

	// this function is for saving all sub items into a section
	void ProjectSave(ConfigurationSection* parentSect, List<ProjectItem*>& items, bool savingBuild)
	{
		for (int i=0;i<items.getCount();i++)
		{
			ConfigurationSection* sect = items[i]->Save(savingBuild);
			if (sect)
				parentSect->AddSection(sect);
		}
	}

	void Project::setBasePath(const String& path)
	{
		m_basePath = path; 
		if (m_outputPath.size() == 0)
			m_outputPath = m_basePath;
		else
		{
			if (StringUtils::StartsWith(m_outputPath, L".."))
			{
				m_outputPath = PathUtils::Combine(m_basePath, m_outputPath);
			}
		}
		PathUtils::Append(m_outputPath, L"build");
	}

	void ProjectItem::NotifyModified() { m_timeStamp = time(0); }

	//////////////////////////////////////////////////////////////////////////
	struct ProjectItemTypeConv : public EnumDualConversionHelper<ProjectItemType>
	{
		ProjectItemTypeConv() 
			: EnumDualConversionHelper<ProjectItemType>(10)
		{
			AddPair(L"Custom", ProjectItemType::Custom);
			AddPair(L"Folder", ProjectItemType::Folder);
			AddPair(L"Material", ProjectItemType::Material);
			AddPair(L"MaterialSet", ProjectItemType::MaterialSet);
			AddPair(L"Texture", ProjectItemType::Texture);
			AddPair(L"Model", ProjectItemType::Model);
			AddPair(L"tanim", ProjectItemType::TransformAnimation);
			AddPair(L"manim", ProjectItemType::MaterialAnimation);
			AddPair(L"Effect", ProjectItemType::Effect);
			AddPair(L"EffectList", ProjectItemType::EffectList);
			AddPair(L"CustomEffect", ProjectItemType::CustomEffect);
			AddPair(L"ShaderNetwork", ProjectItemType::ShaderNetwork);
			AddPair(L"Font", ProjectItemType::Font);
			AddPair(L"FontGlyphDist", ProjectItemType::FontGlyphDist);
			AddPair(L"UILayout", ProjectItemType::UILayout);
			AddPair(L"Copy", ProjectItemType::Copy);
		}
	} static ProjectItemTypeConvInst;

	struct TextureFilterTypeConv : public EnumDualConversionHelper<TextureFilterType>
	{
		TextureFilterTypeConv() 
			: EnumDualConversionHelper<TextureFilterType>(10)
		{
			AddPair(L"Nearest", TextureFilterType::Nearest);
			AddPair(L"BSpline", TextureFilterType::BSpline);
			AddPair(L"Box", TextureFilterType::Box);
		}
	} static TextureFilterTypeConvInst;

	struct TextureBuildMethodConv : public EnumDualConversionHelper<TextureBuildMethod>
	{
		TextureBuildMethodConv() 
			: EnumDualConversionHelper<TextureBuildMethod>(10)
		{
			AddPair(L"Default", TextureBuildMethod::BuiltIn);
			AddPair(L"D3D", TextureBuildMethod::D3D);
			AddPair(L"Devil", TextureBuildMethod::Devil);
		}
	} static TextureBuildMethodConvInst;

	struct TextureCompressionTypeConv : public EnumDualConversionHelper<TextureCompressionType>
	{
		TextureCompressionTypeConv() 
			: EnumDualConversionHelper<TextureCompressionType>(10)
		{
			AddPair(L"None", TextureCompressionType::None);
			AddPair(L"LZ4", TextureCompressionType::LZ4);
			AddPair(L"RLE", TextureCompressionType::RLE);
		}
	} static TextureCompressionTypeConvInst;

	struct MeshBuildMethodConv : public EnumDualConversionHelper<MeshBuildMethod>
	{
		MeshBuildMethodConv() 
			: EnumDualConversionHelper<MeshBuildMethod>(10)
		{
			AddPair(L"Ass", MeshBuildMethod::ASS);
			AddPair(L"FBX", MeshBuildMethod::FBX);
			AddPair(L"D3D", MeshBuildMethod::D3D);
		}
	} static MeshBuildMethodConvInst;

	/*struct FontStyleConv : public EnumDualConversionHelper<FontStyle>
	{
		FontStyleConv() 
			: EnumDualConversionHelper<FontStyle>(10)
		{
			AddPair(L"Regular", FontStyle::Regular);
			AddPair(L"Bold", FontStyle::Bold);
			AddPair(L"Italic", FontStyle::Italic);
			AddPair(L"BoldItalic", FontStyle::BoldItalic);
			AddPair(L"Strikeout", FontStyle::Strikeout);
		}
	} static FontStyleConvInst;*/


	ProjectItemType ProjectTypeUtils::ParseProjectItemType(const String& str) { return ProjectItemTypeConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(ProjectItemType type) { return ProjectItemTypeConvInst.ToString(type); }
	void ProjectTypeUtils::FillProjectItemTypeNames(List<String>& names) { return ProjectItemTypeConvInst.DumpNames(names); }
	bool ProjectTypeUtils::SupportsProjectItemType(const String& str) { return ProjectItemTypeConvInst.SupportsName(str); }

	TextureFilterType ProjectTypeUtils::ParseTextureFilterType(const String& str) { return TextureFilterTypeConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(TextureFilterType flt) { return TextureFilterTypeConvInst.ToString(flt); }
	void ProjectTypeUtils::FillTextureFilterTypeNames(List<String>& names) { TextureFilterTypeConvInst.DumpNames(names); }

	TextureBuildMethod ProjectTypeUtils::ParseTextureBuildMethod(const String& str) { return TextureBuildMethodConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(TextureBuildMethod method) { return TextureBuildMethodConvInst.ToString(method); }
	void ProjectTypeUtils::FillTextureBuildMethodNames(List<String>& names) { TextureBuildMethodConvInst.DumpNames(names); }

	TextureCompressionType ProjectTypeUtils::ParseTextureCompressionType(const String& str) { return TextureCompressionTypeConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(TextureCompressionType type) { return TextureCompressionTypeConvInst.ToString(type); }
	void ProjectTypeUtils::FillTextureCompressionTypeNames(List<String>& names) { TextureCompressionTypeConvInst.DumpNames(names); }

	MeshBuildMethod ProjectTypeUtils::ParseModelBuildMethod(const String& str) { return MeshBuildMethodConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(MeshBuildMethod method) { return MeshBuildMethodConvInst.ToString(method); }
	void ProjectTypeUtils::FillModelBuildMethodNames(List<String>& names) { MeshBuildMethodConvInst.DumpNames(names); }

	/*FontStyle ProjectTypeUtils::ParseFontStyle(const String& str) { return FontStyleConvInst.Parse(str); }
	String ProjectTypeUtils::ToString(FontStyle type) { return FontStyleConvInst.ToString(type); }
	void ProjectTypeUtils::FillFontStyleNames(List<String>& names) { return FontStyleConvInst.DumpNames(names); }*/
}