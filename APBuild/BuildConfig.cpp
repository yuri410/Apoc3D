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

			sect->tryGetAttribute(L"SourceAlphaFile", SourceAlphaFile);
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

		DestFile = sect->getAttribute(L"DestinationFile");

		for (const ConfigurationSection* ss : sect->getSubSections())
		{
			CharRange range = { ss->GetAttributeUInt(L"Start"), ss->GetAttributeUInt(L"End") };
			Ranges.Add(range);
		}
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
			CharRange range = { ss->GetAttributeUInt(L"Start"), ss->GetAttributeUInt(L"End") };
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

	void FXListBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();
		DestFile = sect->getAttribute(L"DestinationFile");
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