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

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void ProjectParse(Project* prj, FastList<ProjectItem*>& parentContainer, const ConfigurationSection* sect)
	{
		for (ConfigurationSection::SubSectionIterator iter =  sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			ConfigurationSection* sect = iter->second;

			ProjectItem* item = new ProjectItem(prj);

			item->Parse(sect);

			parentContainer.Add(item);

			//if (item->getType() == PRJITEM_Folder)
			//{
			//	ProjectFolder* folder = static_cast<ProjectFolder*>(item->getData());
			//	ProjectParse(prj, folder->SubItems, sect);
			//}
		}
	}

	void ProjectSave(ConfigurationSection* parentSect, FastList<ProjectItem*>& items, bool savingBuild)
	{
		for (int i=0;i<items.getCount();i++)
		{
			ConfigurationSection* sect = items[i]->Save(savingBuild);
			parentSect->AddSection(sect);

			//if (items[i]->getType() == PRJITEM_Folder)
			//{
			//	ProjectFolder* folder = static_cast<ProjectFolder*>(items[i]->getData());
			//	ProjectSave(sect, folder->SubItems);
			//}
		}
	}


	void ProjectFolder::Parse(const ConfigurationSection* sect)
	{
		//for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();
		//	iter != sect->SubSectionEnd(); iter++)
		//{
		//	const ConfigurationSection* ss = iter->second;

		//	
		//}
		ProjectParse(m_project, SubItems,sect);
	}
	void ProjectFolder::Save(ConfigurationSection* sect, bool savingBuild)
	{
		ProjectSave(sect, SubItems, savingBuild);
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
	void ProjectResTexture::Parse(const ConfigurationSection* sect)
	{
		String method = L"d3d";
		sect->tryGetAttribute(L"Method", method);
		StringUtils::ToLowerCase(method);

		if (method == L"d3d")
		{
			Method = TEXBUILD_D3D;
		}
		else if (method == L"devil")
		{
			Method = TEXBUILD_Devil;
		}
		else if (method == L"default")
		{
			Method = TEXBUILD_BuiltIn;
		}
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
				for (ConfigurationSection::SubSectionIterator iter = srcsect->SubSectionBegin();
					iter != sect->SubSectionEnd(); iter++)
				{
					const ConfigurationSection* ss = iter->second;

					SubMapTable.Add(i, ss->getAttribute(L"FilePath"));
					i++;
				}
				srcsect = sect->getSection(L"AlphaSource");
				i=0;
				for (ConfigurationSection::SubSectionIterator iter = srcsect->SubSectionBegin();
					iter != sect->SubSectionEnd(); iter++)
				{
					const ConfigurationSection* ss = iter->second;

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
			StringUtils::ToLowerCase(flt);

			if (flt == L"nearest")
			{
				ResizeFilterType = TFLT_Nearest;
			}
			else if (flt == L"box")
			{
				ResizeFilterType = TFLT_Box;
			}
			else 
			{
				ResizeFilterType = TFLT_BSpline;
			}
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
		switch (Method)
		{
		case TEXBUILD_BuiltIn:
			sect->AddAttribute(L"Method", L"default");
			break;
		case TEXBUILD_D3D:
			sect->AddAttribute(L"Method", L"d3d");
			break;
		case TEXBUILD_Devil:
			sect->AddAttribute(L"Method", L"devil");
			break;
		}

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

			switch (ResizeFilterType)
			{
			case TFLT_Nearest:
				sect->AddAttribute(L"ResizeFilter", L"nearest");
				break;
			case TFLT_BSpline:
				sect->AddAttribute(L"ResizeFilter", L"bspline");
				break;
			case TFLT_Box:
				sect->AddAttribute(L"ResizeFilter", L"box");
				break;
			}		
		}

		if (NewFormat != FMT_Unknown)
		{
			sect->AddAttribute(L"PixelFormat", PixelFormatUtils::ToString(NewFormat));
		}
	}

	bool ProjectResFont::IsEarlierThan(time_t t)
	{
		return File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile)) < t;
	}

	bool ProjectResFont::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}
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

		for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			const ConfigurationSection* ss = iter->second;

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
	//void PakBuildConfig::Parse(const ConfigurationSection* sect)
	//{
	//	for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();
	//		iter != sect->SubSectionEnd(); iter++)
	//	{
	//		const ConfigurationSection* ss = iter->second;

	//		String path;
	//		if (ss->tryGetAttribute(L"FilePath", path))
	//		{
	//			Files.Add(path);
	//		}
	//		else
	//		{
	//			path = ss->getAttribute(L"DirPath");
	//			bool flatten = false;
	//			ss->TryGetAttributeBool(L"Flatten", flatten);

	//			PakDirEntryConfig ent;
	//			ent.Flatten = flatten;
	//			ent.Path = path;
	//			Dirs.Add(ent);
	//		}


	//	}

	//	DestFile = sect->getAttribute(L"DestinationFile");
	//}


	bool ProjectResEffect::IsEarlierThan(time_t t)
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
	void ProjectResEffect::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
		PListFile = sect->getAttribute(L"ParamList");
		EntryPoint = sect->getAttribute(L"EntryPoint");
		Profile = sect->getAttribute(L"Profile");
	}
	void ProjectResEffect::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DestFile) : DestFile);
		sect->AddAttribute(L"ParamList", savingBuild ? PathUtils::Combine(m_project->getBasePath(), PListFile) : PListFile);
		sect->AddAttribute(L"EntryPoint", EntryPoint);
		sect->AddAttribute(L"Profile", Profile);
	}

	bool ProjectResModel::IsEarlierThan(time_t t)
	{
		time_t destFileTime = File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DstFile));

		if (destFileTime < t)
			return true;

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
	void ProjectResModel::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		String method = L"ass";
		sect->tryGetAttribute(L"Method", method);
		StringUtils::ToLowerCase(method);

		if (method == L"ass")
		{
			Method = MESHBUILD_ASS;
		}
		else if (method == L"fbx")
		{
			Method = MESHBUILD_FBX;
		}
	}
	void ProjectResModel::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"SourceFile", savingBuild ? PathUtils::Combine(m_project->getBasePath(), SrcFile) : SrcFile);
		sect->AddAttribute(L"DestinationFile", savingBuild ? PathUtils::Combine(m_project->getOutputPath(), DstFile) : DstFile);
		if (DstAnimationFile.size())
		{
			sect->AddAttribute(L"DestinationAnimFile", DstAnimationFile);
		}

		switch (Method)
		{
		case MESHBUILD_ASS:
			sect->AddAttribute(L"Method", L"ass");
			break;
		case MESHBUILD_FBX:
			sect->AddAttribute(L"Method", L"fbx");
			break;
		}
	}

	bool ProjectCustomItem::IsEarlierThan(time_t t)
	{
		return File::GetFileModifiyTime(PathUtils::Combine(m_project->getOutputPath(),DestFile)) < t;
	}

	bool ProjectCustomItem::IsNotBuilt()
	{
		return !File::FileExists(PathUtils::Combine(m_project->getOutputPath(),DestFile));
	}
	void ProjectCustomItem::Parse(const ConfigurationSection* sect)
	{
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void ProjectCustomItem::Save(ConfigurationSection* sect, bool savingBuild)
	{
		sect->AddAttribute(L"DestinationFile", DestFile);
	}

	ConfigurationSection* ProjectItem::Save(bool savingBuild)
	{
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
			case PRJITEM_Font:
				sect->AddAttribute(L"Type", L"font");
				break;
			case PRJITEM_Folder:
				sect->AddAttribute(L"Type", L"folder");
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
		//else if (buildType == L"pak")
		//{
		//	PakBuild::Build(sect);
		//}
		else if (buildType == L"uilayout")
		{

		}

	}


	void Project::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getAttribute(L"Name");

		ProjectParse(this, m_items, sect);
	}
	void Project::Save(const String& file, bool savingBuild)
	{
		ConfigurationSection* s = Save(savingBuild);

		XMLConfiguration* xc = new XMLConfiguration(m_name);
		xc->Add(s);
		xc->Save(file);
		delete xc;
	}
	ConfigurationSection* Project::Save(bool savingBuild)
	{
		ConfigurationSection* sect = new ConfigurationSection(savingBuild ? L"Build" : L"Project");

		sect->AddAttribute(L"Name", m_name);

		ProjectSave(sect, m_items, savingBuild);
		return sect;
	}
	
	void Project::setBasePath(const String& path)
	{
		m_basePath = path; 
		m_outputPath = m_basePath;
		PathUtils::Append(m_outputPath, L"build");
	}

	
}