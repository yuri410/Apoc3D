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

#include "BuildConfig.h"

#include <objidl.h>
#include <gdiplus.h>

namespace APBuild
{
	void TextureBuildConfig::Parse(const ConfigurationSection* sect)
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

	void FontBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		Size = sect->GetAttributeSingle(L"Size");

		AntiAlias = true;
		sect->TryGetAttributeBool(L"AntiAlias", AntiAlias);


		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void FontMapBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		Size = sect->GetAttributeSingle(L"Size");

		GlyphMargin = sect->GetAttributeInt(L"GlyphMargin");

		AntiAlias = true;
		sect->TryGetAttributeBool(L"AntiAlias", AntiAlias);

		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}

		ShowGrid = false;
		sect->TryGetAttributeBool(L"ShowGrid", ShowGrid);

		DestFile = sect->getAttribute(L"DestinationFile");
		DestIndexFile = sect->getAttribute(L"DestinationIndexFile");
	}



	void PakBuildConfig::Parse(const ConfigurationSection* sect)
	{
		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			String path;
			if (ss->tryGetAttribute(L"FilePath", path))
			{
				Files.Add(path);
			}
			else
			{
				path = ss->getAttribute(L"DirPath");
				bool flatten = false;
				ss->TryGetAttributeBool(L"Flatten", flatten);

				PakDirEntryConfig ent;
				ent.Flatten = flatten;
				ent.Path = path;
				Dirs.Add(ent);
			}
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void AFXBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();
		IsDebug = false;
		sect->TryGetAttributeBool(L"Debug", IsDebug);

		String srcDesc = sect->getAttribute(L"Source");
		List<String> srcSets = StringUtils::Split(srcDesc, L"|");

		for (String e : srcSets)
		{
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
		srcSets = StringUtils::Split(entryPointsDesc, L"|");

		for (String e : srcSets)
		{
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

		Targets = StringUtils::Split(sect->getAttribute(L"Targets"), L"|");
		for (String& tgt : Targets)
		{
			StringUtils::Trim(tgt);
			StringUtils::ToLowerCase(tgt);
		}

		CompactBuild = false;
		sect->TryGetAttributeBool(L"CompactBuild", CompactBuild);
	}
	void CFXBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();

		SrcVSFile = sect->getAttribute(L"VSSource");
		SrcPSFile = sect->getAttribute(L"PSSource");
		DestFile = sect->getAttribute(L"DestinationFile");
		EntryPointVS = sect->getAttribute(L"EntryPointVS");
		EntryPointPS = sect->getAttribute(L"EntryPointPS");
		Profile = sect->getAttribute(L"Profile");

		CompactBuild = false;
		sect->TryGetAttributeBool(L"CompactBuild", CompactBuild);
	}
	void FXListBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();
		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void MeshBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		String strMethod = L"ass";
		sect->tryGetAttribute(L"Method", strMethod);
		Method = ProjectUtils::MeshBuildMethodConv.Parse(strMethod);


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
					index = StringUtils::ParseInt32( ent->getValue() );
				}
				// the vertex elements here only has usage and index. 
				// They only store info here, not for normal use in graphics
				VertexElements.Add(VertexElement(0,VEF_Count,usage,index));
			}
		}

		CompactBuild = false;
		sect->TryGetAttributeBool(L"CompactBuild", CompactBuild);

		if (CompactBuild)
		{
			CollapseMeshs = true;
		}
		else
		{
			CollapseMeshs = false;
			sect->TryGetAttributeBool(L"CollapseMeshs", CollapseMeshs);
		}
	}

	void MaterialBuildConfig::Parse(const ConfigurationSection* sect)
	{
		DstFile = sect->getAttribute(L"DestinationFile");
	}

	void TransformAnimBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		Reverse = sect->GetAttributeBool(L"Reverse");

		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			String name = ss->getName();
			int objIdx = StringUtils::ParseInt32(ss->getValue());

			ObjectIndexMapping.Add(name, objIdx);
		}
	}
	void MaterialAnimBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
	}
	
}