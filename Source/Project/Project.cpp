/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Utility/StringUtils.h"
#include "Graphics/GraphicsCommon.h"
#include "Vfs/File.h"
#include "Vfs/PathUtils.h"
#include "Math/RandomUtils.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void RecursivePassFolderPacks(int& startNo, FastList<ConfigurationSection*>& result, FastList<ProjectItem*>& items);
	void ProjectParse(Project* prj, FastList<ProjectItem*>& parentContainer, const ConfigurationSection* sect);
	void ProjectSave(ConfigurationSection* parentSect, FastList<ProjectItem*>& items, bool savingBuild);


	void ProjectFolder::Parse(const ConfigurationSection* sect)
	{
		sect->tryGetAttribute(L"Pack", PackType);

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
			sect->AddAttribute(L"Pack", PackType);
			sect->AddAttribute(L"DestinationPack", DestinationPack);
		}
		ProjectSave(sect, SubItems, savingBuild);
	}
	void ProjectFolder::SavePackBuildConfig(ConfigurationSection* sect)
	{
		sect->AddAttribute(L"Type", PackType);
		sect->AddAttribute(L"DestinationFile", PathUtils::Combine(m_project->getOutputPath(),DestinationPack));

		// a package file/archive build config section needs every item's path
		int pakEntryIndex = 0;
		for (int i=0;i<SubItems.getCount();i++)
		{
			if (SubItems[i]->getData())
			{
				std::vector<String> itemOutputs = SubItems[i]->getData()->GetAllOutputFiles();
				for (size_t j=0;j<itemOutputs.size();j++)
				{
					ConfigurationSection* e = new ConfigurationSection(L"Entry" + StringUtils::ToString(pakEntryIndex++));
					e->AddAttribute(L"FilePath", itemOutputs[j]);
					sect->AddSection(e);
				}
			}
		}
	}
	std::vector<String> ProjectFolder::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (PackType.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestinationPack));
		return e;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ProjectResTexture::Parse(const ConfigurationSection* sect)
	{
		String method = L"d3d";
		sect->tryGetAttribute(L"Method", method);

		Method = ParseBuildMethod(method);
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

		ResizeFilterType = TFLT_BSpline;
		String flt;
		if (sect->tryGetAttribute(L"ResizeFilter", flt))
		{
			ResizeFilterType = ParseFilterType(flt);
		}

		Resize = passed;

		NewFormat = FMT_Unknown;
		String fmt;
		if (sect->tryGetAttribute(L"PixelFormat", fmt))
		{
			NewFormat = PixelFormatUtils::ConvertFormat(fmt);
		}

	}
	void ProjectResTexture::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"Method", ToString(Method));

		if (AssembleCubemap || AssembleVolumeMap)
		{
			if (AssembleCubemap)
			{
				sect->AddAttribute(L"Assemble", L"cubemap");

				sect->AddAttribute(L"NegX", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeX]) : SubMapTable[CUBE_NegativeX]);
				sect->AddAttribute(L"NegY", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeY]) : SubMapTable[CUBE_NegativeY]);
				sect->AddAttribute(L"NegZ", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_NegativeZ]) :SubMapTable[CUBE_NegativeZ]);
				sect->AddAttribute(L"PosX", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveX]) :SubMapTable[CUBE_PositiveX]);
				sect->AddAttribute(L"PosY", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveY]) :SubMapTable[CUBE_PositiveY]);
				sect->AddAttribute(L"PosZ", savingBuild ?
					PathUtils::Combine(m_project->getBasePath(), SubMapTable[CUBE_PositiveZ]) :SubMapTable[CUBE_PositiveZ]);

				if (SubAlphaMapTable.Contains(CUBE_NegativeX))
				{
					sect->AddAttribute(L"NegXAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeX]) : SubAlphaMapTable[CUBE_NegativeX]);
				}
				if (SubAlphaMapTable.Contains(CUBE_NegativeY))
				{
					sect->AddAttribute(L"NegYAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeY]) : SubAlphaMapTable[CUBE_NegativeY]);
				}
				if (SubAlphaMapTable.Contains(CUBE_NegativeZ))
				{
					sect->AddAttribute(L"NegZAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_NegativeZ]) : SubAlphaMapTable[CUBE_NegativeZ]);
				}

				if (SubAlphaMapTable.Contains(CUBE_PositiveX))
				{
					sect->AddAttribute(L"PosXAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveX]) : SubAlphaMapTable[CUBE_PositiveX]);
				}
				if (SubAlphaMapTable.Contains(CUBE_PositiveY))
				{
					sect->AddAttribute(L"PosYAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveY]) : SubAlphaMapTable[CUBE_PositiveY]);
				}
				if (SubAlphaMapTable.Contains(CUBE_PositiveZ))
				{
					sect->AddAttribute(L"PosZAlpha", savingBuild ?
						PathUtils::Combine(m_project->getBasePath(), SubAlphaMapTable[CUBE_PositiveZ]) : SubAlphaMapTable[CUBE_PositiveZ]);
				}
			}
			else
			{
				sect->AddAttribute(L"Assemble", L"volume");
				uint i =0;
				ConfigurationSection* srcsect = new ConfigurationSection(L"Source");
				for (int i=0;i<SubMapTable.getCount();i++)
				{
					ConfigurationSection* es = new ConfigurationSection(String(L"Slice") + StringUtils::ToString(i));
					es->AddAttribute(L"FilePath", SubMapTable[i]);

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);

				srcsect = new ConfigurationSection(L"AlphaSource");
				for (int i=0;i<SubAlphaMapTable.getCount();i++)
				{
					ConfigurationSection* es = new ConfigurationSection(String(L"Slice") + StringUtils::ToString(i));
					es->AddAttribute(L"FilePath", SubAlphaMapTable[i]);

					srcsect->AddSection(es);
				}
				sect->AddSection(srcsect);
			}
			
		}
		else
		{
			sect->AddAttribute(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(),SourceFile) : SourceFile);
		}
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(),DestinationFile) : DestinationFile);
		if (GenerateMipmaps)
			sect->AddAttribute(L"GenerateMipmaps", StringUtils::ToString(GenerateMipmaps));

		if (Resize)
		{
			sect->AddAttribute(L"Width", StringUtils::ToString(NewWidth));
			sect->AddAttribute(L"Height", StringUtils::ToString(NewHeight));
			sect->AddAttribute(L"Depth", StringUtils::ToString(NewDepth));

			sect->AddAttribute(L"ResizeFilter", ToString(ResizeFilterType));
		}

		if (NewFormat != FMT_Unknown)
		{
			sect->AddAttribute(L"PixelFormat", PixelFormatUtils::ToString(NewFormat));
		}
	}
	std::vector<String> ProjectResTexture::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestinationFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestinationFile));
		return e;
	}
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
	bool ProjectResTexture::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestinationFile));
	}
	String ProjectResTexture::ToString(TextureFilterType flt)
	{
		switch (flt)
		{
		case TFLT_Nearest:
			return L"nearest";
		case TFLT_BSpline:
			return L"bspline";
		case TFLT_Box:
			return L"box";
		}	
		return L"bspline";
	}
	String ProjectResTexture::ToString(TextureBuildMethod method)
	{
		switch (method)
		{
		case TEXBUILD_BuiltIn:
			return L"default";
		case TEXBUILD_D3D:
			return L"d3d";
		case TEXBUILD_Devil:
			return L"devil";
		}
		return L"default";
	}

	ProjectResTexture::TextureFilterType ProjectResTexture::ParseFilterType(const String& str)
	{
		String flt = str;
		StringUtils::ToLowerCase(flt);

		if (flt == L"nearest")
		{
			return ProjectResTexture::TFLT_Nearest;
		}
		else if (flt == L"box")
		{
			return ProjectResTexture::TFLT_Box;
		}
		return ProjectResTexture::TFLT_BSpline;
	}
	ProjectResTexture::TextureBuildMethod ProjectResTexture::ParseBuildMethod(const String& str)
	{
		String method = str;
		StringUtils::ToLowerCase(method);

		if (method == L"d3d")
		{
			return ProjectResTexture::TEXBUILD_D3D;
		}
		else if (method == L"devil")
		{
			return ProjectResTexture::TEXBUILD_Devil;
		}
		else if (method == L"default")
		{
			return ProjectResTexture::TEXBUILD_BuiltIn;
		}
		return ProjectResTexture::TEXBUILD_BuiltIn;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	
	void ProjectResFont::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");
		Size = sect->GetAttributeSingle(L"Size");

		Style = FONTSTYLE_Regular;
		String strStyle;
		if (sect->tryGetAttribute(L"Style", strStyle))
		{
			StringUtils::ToLowerCase(strStyle);

			if (strStyle == L"regular")
			{
				Style = FONTSTYLE_Regular;
			}
			else if (strStyle == L"bold")
			{
				Style = FONTSTYLE_Bold;
			}
			else if (strStyle == L"italic")
			{
				Style = FONTSTYLE_Italic;
			}
			else if (strStyle == L"bolditalic")
			{
				Style = FONTSTYLE_BoldItalic;
			}
			else if (strStyle == L"underline")
			{
				Style = FONTSTYLE_Strikeout;
			}
		}

		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResFont::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"Name", Name);
		sect->AddAttribute(L"Size", StringUtils::ToString(Size));

		if (Style != FONTSTYLE_Regular)
		{
			switch (Style)
			{
			case FONTSTYLE_Bold:
				sect->AddAttribute(L"Style", L"Bold");
				break;
			case FONTSTYLE_Italic:
				sect->AddAttribute(L"Style", L"Italic");
				break;
			case FONTSTYLE_BoldItalic:
				sect->AddAttribute(L"Style", L"BoldItalic");
				break;
			case FONTSTYLE_Strikeout:
				sect->AddAttribute(L"Style", L"Underline");
				break;
				
			}
		}

		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);

		for (int i=0;i<Ranges.getCount();i++)
		{
			ConfigurationSection* ss = new ConfigurationSection(String(L"Range") + StringUtils::ToString(i));
			ss->AddAttribute(L"Start", StringUtils::ToString( Ranges[i].MinChar));
			ss->AddAttribute(L"End", StringUtils::ToString( Ranges[i].MaxChar));

			sect->AddSection(ss);
		}
	}
	std::vector<String> ProjectResFont::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
	bool ProjectResFont::IsEarlierThan(time_t t)
	{
		return File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile)) < t;
	}
	bool ProjectResFont::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ProjectResEffect::Parse(const ConfigurationSection* sect)
	{
		SrcVSFile = sect->getAttribute(L"VSSource");
		SrcPSFile = sect->getAttribute(L"PSSource");
		DestFile = sect->getAttribute(L"DestinationFile");
		PListFile = sect->getAttribute(L"ParamList");
		EntryPointVS = sect->getAttribute(L"EntryPointVS");
		EntryPointPS = sect->getAttribute(L"EntryPointPS");
		Profile = sect->getAttribute(L"Profile");
	}
	void ProjectResEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"VSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcVSFile) : SrcVSFile);
		sect->AddAttribute(L"PSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcPSFile) : SrcPSFile);

		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
		sect->AddAttribute(L"ParamList", savingBuild ? PathUtils::Combine(m_project->getBasePath(), PListFile) : PListFile);
		sect->AddAttribute(L"EntryPointVS", EntryPointVS);
		sect->AddAttribute(L"EntryPointPS", EntryPointPS);
		sect->AddAttribute(L"Profile", Profile);
	}
	std::vector<String> ProjectResEffect::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
	bool ProjectResEffect::IsEarlierThan(time_t t)
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
		path = PathUtils::Combine(m_project->getBasePath(), PListFile);
		if (File::FileExists(path))
		{
			if (File::GetFileModifiyTime(path) > destFileTime)
			{
				return true;
			}
		}
		return false;
	}
	bool ProjectResEffect::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}
	/************************************************************************/
	/*                                                                      */
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
		sect->AddAttribute(L"VSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcVSFile) : SrcVSFile);
		sect->AddAttribute(L"PSSource", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcPSFile) : SrcPSFile);

		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
		sect->AddAttribute(L"EntryPointVS", EntryPointVS);
		sect->AddAttribute(L"EntryPointPS", EntryPointPS);
		sect->AddAttribute(L"Profile", Profile);
	}
	std::vector<String> ProjectResCustomEffect::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
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
	bool ProjectResCustomEffect::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	// Finds all effects in the project
	void WalkProject(const FastList<ProjectItem*>& items, List<String>& effectsFound)
	{
		for (int i=0;i<items.getCount();i++)
		{
			if (items[i]->getType() == PRJITEM_Effect)
			{
				effectsFound.Add(items[i]->getName());
			}
			else if (items[i]->getType() == PRJITEM_Folder)
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
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);

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
	std::vector<String> ProjectResEffectList::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
	bool ProjectResEffectList::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ProjectResShaderNetwork::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectResShaderNetwork::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
	}
	std::vector<String> ProjectResShaderNetwork::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
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
	bool ProjectResShaderNetwork::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ProjectResModel::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		String method = L"ass";
		sect->tryGetAttribute(L"Method", method);
		StringUtils::ToLowerCase(method);

		Method = ParseBuildMethod(method);

	}
	void ProjectResModel::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DstFile) : DstFile);
		if (DstAnimationFile.size())
		{
			sect->AddAttribute(L"DestinationAnimFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DstAnimationFile) :DstAnimationFile);
		}
		sect->AddAttribute(L"Method", ToString(Method));
	}
	std::vector<String> ProjectResModel::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DstAnimationFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DstAnimationFile));
		if (DstFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DstFile));
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

	ProjectResModel::MeshBuildMethod ProjectResModel::ParseBuildMethod(const String& str)
	{
		if (str == L"ass")
		{
			return MESHBUILD_ASS;
		}
		else if (str == L"fbx")
		{
			return MESHBUILD_FBX;
		}
		else if (str == L"d3d")
		{
			return MESHBUILD_D3D;
		}
		return MESHBUILD_ASS;
	}
	String ProjectResModel::ToString(MeshBuildMethod method)
	{
		switch (method)
		{
		case MESHBUILD_ASS:
			return L"ass";
		case MESHBUILD_FBX:
			return L"fbx";
			break;
		}
		return L"ass";
	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ProjectCustomItem::Parse(const ConfigurationSection* sect)
	{
		DestFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"EditorExtension", EditorExtension);
	}
	void ProjectCustomItem::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"DestinationFile", DestFile);
		if (EditorExtension.size())
		{
			sect->AddAttribute(L"EditorExtension", EditorExtension);
		}
	}
	std::vector<String> ProjectCustomItem::GetAllOutputFiles()
	{
		std::vector<String> e;
		if (DestFile.size())
			e.push_back(PathUtils::Combine(m_project->getOutputPath(),DestFile));
		return e;
	}
	bool ProjectCustomItem::IsEarlierThan(time_t t)
	{
		return File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile)) < t;
	}
	bool ProjectCustomItem::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	ConfigurationSection* ProjectItem::Save(bool savingBuild)
	{
		if (savingBuild && 
			(m_typeData->getType() == PRJITEM_Model || m_typeData->getType() == PRJITEM_Texture) &&
			!IsOutDated())
		{
			return 0;
		}
		ConfigurationSection* sect = new ConfigurationSection(m_name);

		if (!savingBuild)
		{
			sect->AddAttribute(L"LastModTime", StringUtils::ToString(m_timeStamp));
		}
		if (m_typeData)
		{
			switch (m_typeData->getType())
			{
			case PRJITEM_Texture:
				sect->AddAttribute(L"Type", L"texture");
				break;
			case PRJITEM_Model:
				sect->AddAttribute(L"Type", L"mesh");
				break;
			case PRJITEM_Effect:
				sect->AddAttribute(L"Type", L"effect");
				break;
			case PRJITEM_EffectList:
				sect->AddAttribute(L"Type", L"ProjectFXList");
				break;
			case PRJITEM_CustomEffect:
				sect->AddAttribute(L"Type", L"CustomEffect");
				break;
			case PRJITEM_Font:
				sect->AddAttribute(L"Type", L"font");
				break;
			case PRJITEM_Folder:
				sect->AddAttribute(L"Type", L"folder");
				break;
			case PRJITEM_ShaderNetwork:
				sect->AddAttribute(L"Type", L"ShaderNet");
				break;
			}
			
			m_typeData->Save(sect, savingBuild);
		}
		return sect;
	}
	void ProjectItem::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getName();//L"Name");

		if (sect->hasAttribute(L"LastModTime"))
		{
			m_timeStamp = StringUtils::ParseInt64( sect->getAttribute(L"LastModTime"));
		}
		else
		{
			m_timeStamp = time(0);
		}
		

		String buildType = sect->getAttribute(L"Type");
		StringUtils::ToLowerCase(buildType);

		if (buildType == L"texture")
		{
			ProjectResTexture* tex = new ProjectResTexture(m_project);
			tex->Parse(sect);
			m_typeData = tex;
		}
		else if (buildType == L"mesh")
		{
			ProjectResModel* mdl = new ProjectResModel(m_project);
			mdl->Parse(sect);
			m_typeData = mdl;
			//MeshBuild::Build(sect);
		}
		else if (buildType == L"effect")
		{
			ProjectResEffect* eff = new ProjectResEffect(m_project);
			eff->Parse(sect);
			m_typeData = eff;
		}
		else if (buildType == L"font")
		{
			ProjectResFont* font = new ProjectResFont(m_project);
			font->Parse(sect);
			m_typeData = font;
			//FontBuild::Build(sect);
		}
		else if (buildType == L"folder")
		{
			ProjectFolder* folder = new ProjectFolder(m_project);
			folder->Parse(sect);
			m_typeData = folder;
		}
		else if (buildType == L"shadernet")
		{
			ProjectResShaderNetwork* snet = new ProjectResShaderNetwork(m_project);
			snet->Parse(sect);
			m_typeData = snet;
		}
		//else if (buildType == L"pak")
		//{
		//	PakBuild::Build(sect);
		//}
		else if (buildType == L"uilayout")
		{

		}
		else if (buildType == L"projectfxlist")
		{
			ProjectResEffectList* list = new ProjectResEffectList(m_project);
			list->Parse(sect);
			m_typeData = list;
		}
	}


	void Project::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getAttribute(L"Name");
		sect->tryGetAttribute(L"TexturePath", m_texturePath);
		ProjectParse(this, m_items, sect);
	}
	void Project::Save(const String& file)
	{
		ConfigurationSection* s = Save();

		XMLConfiguration* xc = new XMLConfiguration(m_name);
		xc->Add(s);
		xc->Save(file);
		delete xc;
	}

	void RecursivePassFolderPacks(int& startNo, ConfigurationSection* parentSect, FastList<ProjectItem*>& items);

	ConfigurationSection* Project::Save()
	{
		ConfigurationSection* sect = new ConfigurationSection(L"Project", m_items.getCount() * 2);

		sect->AddAttribute(L"Name", m_name);
		sect->AddAttribute(L"TexturePath", m_texturePath);
		ProjectSave(sect, m_items, false);

		return sect;
	}
	void Project::GenerateBuildScripts(FastList<ConfigurationSection*>& result)
	{
		ConfigurationSection* sect = new ConfigurationSection(L"Build", m_items.getCount() * 2);

		sect->AddAttribute(L"Name", m_name);

		ProjectSave(sect, m_items, true);

		sect->AddAttribute(L"Type", L"Project");
		result.Add(sect);
		
		int startNo = Randomizer::Next(65535);
		RecursivePassFolderPacks(startNo, result, m_items);
		
	}
	void RecursivePassFolderPacks(int& startNo, FastList<ConfigurationSection*>& result, FastList<ProjectItem*>& items)
	{
		// post traversal on the project tree will make leaf folder to pack file builds comes first
		for (int i=0;i<items.getCount();i++)
		{
			if (items[i]->getType() == PRJITEM_Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(items[i]->getData());

				if (fld)
				{
					RecursivePassFolderPacks(startNo, result, fld->SubItems);

					if (fld->PackType.size())
					{
						ConfigurationSection* s = new ConfigurationSection(L"Archive_" + StringUtils::ToString(startNo++));
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
	void ProjectParse(Project* prj, FastList<ProjectItem*>& parentContainer, const ConfigurationSection* sect)
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
	void ProjectSave(ConfigurationSection* parentSect, FastList<ProjectItem*>& items, bool savingBuild)
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
		m_outputPath = m_basePath;
		PathUtils::Append(m_outputPath, L"build");
	}

	
}