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
#include "Utility/StringUtils.h"
#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
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

					SubMapTable.Add(i, ss->getAttribute(L"FilePath"));
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
	void ProjectResEffect::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
		PListFile = sect->getAttribute(L"ParamList");
		EntryPoint = sect->getAttribute(L"EntryPoint");
		Profile = sect->getAttribute(L"Profile");
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

	void ProjectCustomItem::Parse(const ConfigurationSection* sect)
	{
		DestFile = sect->getAttribute(L"DestinationFile");
	}

	void ProjectItem::Parse(const ConfigurationSection* sect)
	{
		m_name = sect->getValue();

		String buildType = sect->getAttribute(L"Type");
		StringUtils::ToLowerCase(buildType);

		if (buildType == L"texture")
		{
			ProjectResTexture* tex = new ProjectResTexture();
			tex->Parse(sect);
			m_typeData = tex;
		}
		else if (buildType == L"mesh")
		{
			ProjectResModel* mdl = new ProjectResModel();
			mdl->Parse(sect);
			m_typeData = mdl;
			//MeshBuild::Build(sect);
		}
		else if (buildType == L"effect")
		{
			ProjectResEffect* eff = new ProjectResEffect();
			eff->Parse(sect);
			m_typeData = eff;
		}
		else if (buildType == L"font")
		{
			ProjectResFont* font = new ProjectResFont();
			font->Parse(sect);
			m_typeData = font;
			//FontBuild::Build(sect);
		}
		else if (buildType == L"folder")
		{
			ProjectFolder* folder = new ProjectFolder();
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

	void ProjectParse(FastList<ProjectItem*>& parentContainer, const ConfigurationSection* sect)
	{
		for (ConfigurationSection::SubSectionIterator iter =  sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			ConfigurationSection* sect = iter->second;

			ProjectItem* item = new ProjectItem();

			item->Parse(sect);

			parentContainer.Add(item);

			if (item->getType() == PRJITEM_Folder)
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(item->getData());
				ProjectParse(folder->SubItems, sect);
			}
		}
	}

	void Project::Parse(const ConfigurationSection* sect)
	{
		ProjectParse(m_items, sect);
	}
}