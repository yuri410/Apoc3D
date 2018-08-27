/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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