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

#ifndef BUILDCONFIG_H
#define BUILDCONFIG_H

#include "APBCommon.h"

namespace APBuild
{
	/** More descriptions on these build config structs can be found in Project.h
	*/

	struct TextureBuildConfig 
	{
		String SourceFile;
		String SourceAlphaFile;
		String DestinationFile;

		ProjectTextureResizingOptions Resizing;

		TextureBuildMethod Method = TextureBuildMethod::D3D;

		TextureFilterType ResizeFilterType = TextureFilterType::BSpline;
		Apoc3D::Graphics::PixelFormat NewFormat = FMT_Unknown;
		
		TextureCompressionType CompressionType = TextureCompressionType::None;

		bool GenerateMipmaps = false;
		bool AssembleCubemap = false;
		bool AssembleVolumeMap = false;

		HashMap<uint, String> SubMapTable;
		HashMap<uint, String> SubAlphaMapTable;


		void Parse(const ConfigurationSection* sect);
	};

	struct CharRange
	{
		uint32 MinChar;
		uint32 MaxChar;
	};
	struct FontBuildConfig
	{
		List<CharRange> Ranges;
		List<CharRange> InvRanges;

		String SourceFile;
		float Size;
		bool AntiAlias;
		bool Compress;

		String DestFile;
		
		void Parse(const ConfigurationSection* sect);
	};
	struct FontMapBuildConfig
	{
		List<CharRange> Ranges;
		String SourceFile;
		float Size;
		bool AntiAlias;

		String DestFile;
		String DestIndexFile;

		int32 GlyphMargin;
		bool ShowGrid;

		void Parse(const ConfigurationSection* sect);
	};

	struct PakBuildConfig
	{
		struct PakDirEntryConfig
		{
			bool Flatten;
			String Path;
		};
		List<String> Files;
		List<PakDirEntryConfig> Dirs;
		String DestFile;

		void Parse(const ConfigurationSection* sect);
	};

	struct FXListBuildConfig
	{
		String Name;
		String DestFile;

		void Parse(const ConfigurationSection* sect);
	};

	struct MaterialBuildConfig
	{
		String DstFile;

		void Parse(const ConfigurationSection* sect);
	};
	struct TransformAnimBuildConfig
	{
		String SrcFile;
		String DstFile;
		bool Reverse;

		HashMap<String, int> ObjectIndexMapping;

		void Parse(const ConfigurationSection* sect);
	};

	struct MaterialAnimBuildConfig
	{
		String SrcFile;
		String DstFile;

		void Parse(const ConfigurationSection* sect);
	};
}
#endif