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

#include "TextureBuild.h"

#include "BuildSystem.h"

#include "D3DTextureBuild.h"

#include <IL/il.h>
#include <IL/ilu.h>

namespace APBuild
{
	void BuildByD3D(const TextureBuildConfig& config)
	{
		using namespace Apoc3D::Graphics::D3D9RenderSystem;
		
		if (config.AssembleCubemap || config.AssembleVolumeMap)
		{
			int32 width = config.Resizing.getNewWidth();
			int32 height = config.Resizing.getNewHeight();
			int32 depth = config.Resizing.getNewDepth();

			DXTex tex(config.AssembleCubemap ? TextureType::CubeTexture : TextureType::Texture3D, config.DestinationFile,
				width, height, depth, config.NewFormat);

			if (tex.isError())
				return;

			if (config.AssembleCubemap)
			{
				tex.AssembleCubeMap(config.SubMapTable, &config.SubAlphaMapTable);
				if (tex.isError())
					return;

			}
			else
			{
				tex.AssembleVolumeMap(config.SubMapTable, &config.SubAlphaMapTable);
				if (tex.isError())
					return;
			}

			tex.Save(config.DestinationFile, config.CompressionType);
		}
		else
		{
			DXTex tex(config.SourceFile);
			if (tex.isError())
				return;
			
			if (config.SourceAlphaFile.size())
				tex.LoadAlphaMap(config.SourceAlphaFile);

			if (config.Resizing.IsResizing())
			{
				int32 newWidth = config.Resizing.GetResizedWidth(tex.getWidth());
				int32 newHeight = config.Resizing.GetResizedHeight(tex.getHeight());

				tex.Resize(newWidth, newHeight);
				if (tex.isError())
					return;
			}
			if (config.GenerateMipmaps)
			{
				tex.GenerateMipMaps();
				if (tex.isError())
					return;
			}
			if (config.NewFormat != FMT_Unknown)
			{
				tex.Compress(D3D9Utils::ConvertPixelFormat(config.NewFormat));
				if (tex.isError())
					return;
				
			}

			tex.Save(config.DestinationFile, config.CompressionType);
		}
	}

	PixelFormat ConvertFormat(int format, int elementType, int bpp)
	{
		switch (format)
		{
		case IL_BGR:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				if (bpp == 4)
					return FMT_X8R8G8B8;
				return FMT_R8G8B8;
			}
			break;
		case IL_BGRA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8R8G8B8;
			}
			break;
		case IL_COLOUR_INDEX:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_Palette8;
			}
			break;
		case IL_LUMINANCE:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_Luminance8;
			case IL_SHORT:
			case IL_UNSIGNED_SHORT:
				return FMT_Luminance16;
			}

			break;
		case IL_LUMINANCE_ALPHA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8L8;
			}
			break;
		case IL_RGB:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				if (bpp == 4)
					return FMT_X8B8G8R8;
				return FMT_B8G8R8;
			}
			break;
		case IL_RGBA:
			switch (elementType)
			{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				return FMT_A8B8G8R8;
			case IL_SHORT:
			case IL_UNSIGNED_SHORT:
				return FMT_A16B16G16R16;
			case IL_FLOAT:
				if (bpp == 2)
					return FMT_A16B16G16R16F;
				return FMT_A32B32G32R32F;
			}
			break;

		case IL_DXT1:
			return FMT_DXT1;
		case IL_DXT2:
			return FMT_DXT2;
		case IL_DXT3:
			return FMT_DXT3;
		case IL_DXT4:
			return FMT_DXT4;
		case IL_DXT5:
			return FMT_DXT5;
		}
		throw AP_EXCEPTION(ExceptID::NotSupported, L"ConvertFormat");
		//return FMT_Unknown;
	}
	int ConvertFilter(TextureFilterType flt)
	{
		switch (flt)
		{
			case TextureFilterType::Nearest: return ILU_NEAREST;
			case TextureFilterType::Box: return ILU_SCALE_BOX;
			case TextureFilterType::BSpline: return ILU_SCALE_BSPLINE;
		}
		throw AP_EXCEPTION(ExceptID::NotSupported, L"Not supported filter type");
	}
	void BuildByDevIL(const TextureBuildConfig& config)
	{
		if (config.AssembleCubemap || config.AssembleVolumeMap)
		{
			BuildSystem::LogError(L"DevIL Build Method currently does not support assembling textures.", config.SourceFile);
			return;
		}
		int image = ilGenImage();

		ilBindImage(image);
		ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);
		
		ILboolean ret = ilLoadImage(config.SourceFile.c_str());
		assert(ret);

		int ilFormat = ilGetInteger(IL_IMAGE_FORMAT);


		if (config.GenerateMipmaps)
		{
			iluBuildMipmaps();
		}

		ilActiveMipmap(0);

		if (config.Resizing.IsResizing())
		{
			int32 curWidth = ilGetInteger(IL_IMAGE_WIDTH);
			int32 curHeight = ilGetInteger(IL_IMAGE_HEIGHT);
			int32 curDepth = ilGetInteger(IL_IMAGE_DEPTH);

			int32 newWidth = config.Resizing.GetResizedWidth(curWidth);
			int32 newHeight = config.Resizing.GetResizedHeight(curHeight);
			int32 newDepth = config.Resizing.GetResizedHeight(curDepth);

			iluImageParameter(ILU_FILTER, ConvertFilter(config.ResizeFilterType));
			iluScale(newWidth, newHeight, newDepth);
		}

		int mipCount = ilGetInteger(IL_NUM_MIPMAPS) + 1;

		int dataType = ilGetInteger(IL_IMAGE_TYPE);
		int bytePP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

		bool cubeFlags = ilGetInteger(IL_IMAGE_CUBEFLAGS) > 0;
		int depth0 = ilGetInteger(IL_IMAGE_DEPTH);

		TextureData texData;
		texData.LevelCount = mipCount;
		texData.ContentSize = 0;
		texData.Format = ConvertFormat(ilFormat, dataType, bytePP);
		texData.Flags = TextureData::TDF_None;

		if (cubeFlags)
		{
			texData.Type = TextureType::CubeTexture;
		}
		else
		{
			texData.Type = depth0 > 1 ? TextureType::Texture3D : TextureType::Texture2D;
		}

		int dxtFormat = ilGetInteger(IL_DXTC_DATA_FORMAT);
		if (dxtFormat != IL_DXT_NO_COMP)
		{
			texData.Format = ConvertFormat(dxtFormat, 0, 0);
		}

		texData.Levels.Reserve(mipCount);
		for (int i = 0; i < mipCount; i++)
		{
			ilBindImage(image);
			ilActiveMipmap(i);

			texData.Levels[i].Width = ilGetInteger(IL_IMAGE_WIDTH);
			texData.Levels[i].Height = ilGetInteger(IL_IMAGE_HEIGHT);
			texData.Levels[i].Depth = ilGetInteger(IL_IMAGE_DEPTH);

			if (dxtFormat != IL_DXT_NO_COMP)
			{
				int numImagePasses = cubeFlags ? 6 : 1;
				int dxtSize = ilGetDXTCData(0, 0, dxtFormat);

				char* buffer = new char[numImagePasses * dxtSize];

				for (int j = 0, offset = 0; j < numImagePasses; j++, offset += dxtSize)
				{
					if (cubeFlags)
					{
						ilBindImage(image);
						ilActiveImage(j);
						ilActiveMipmap(i);
					}
					ilGetDXTCData(&buffer[offset], dxtSize, dxtFormat);
				}

				texData.Levels[i].ContentData = buffer;
				texData.Levels[i].LevelSize = numImagePasses * dxtSize;

				texData.ContentSize += texData.Levels[i].LevelSize;
			}
			else
			{
				int numImagePasses = cubeFlags ? 6 : 1;
				int imageSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

				texData.Levels[i].LevelSize = imageSize;
				char* buffer = new char[numImagePasses * imageSize];
				texData.Levels[i].ContentData = buffer;

				for (int j = 0, offset = 0; j < numImagePasses; j++, offset += imageSize)
				{
					if (cubeFlags)
					{
						ilBindImage(image);
						ilActiveImage(j);
						ilActiveMipmap(i);
					}

					//if (texData.Format == ImagePixelFormat.A8B8G8R8)
					//{
					//	fixed (byte* dst = &buffer[offset])
					//	{
					//		Il.ilCopyPixels(0, 0, 0, texData.Levels[i].Width, texData.Levels[i].Height,
					//			texData.Levels[i].Depth, Il.IL_BGRA, Il.IL_UNSIGNED_BYTE, new IntPtr(dst));
					//	}
					//}
					//else
					//{
					//IntPtr ptr = Il.ilGetData();
					//fixed (byte* dst = &buffer[offset])
					//{
					memcpy(&buffer[offset], ilGetData(), imageSize);
					//}

					//}
				}

				texData.ContentSize += imageSize;
			}

		}

		ilDeleteImage(image);


		if (config.NewFormat != FMT_Unknown &&
			texData.Format != config.NewFormat)
		{
			texData.ConvertInPlace(config.NewFormat);
		}

		if (config.CompressionType == TextureCompressionType::RLE)
			texData.Flags = TextureData::TDF_RLECompressed;
		else if (config.CompressionType == TextureCompressionType::LZ4)
			texData.Flags = TextureData::TDF_LZ4Compressed;

		//else if (config.CompressionType == TDCT_Auto)
		//{
		//	int32 compressedSize = 0;
		//	for (int32 i=0;i<texData.Levels.getCount();i++)
		//	{
		//		compressedSize += rleEvalCompressedSize(texData.Levels[i].ContentData, texData.Levels[i].LevelSize);
		//	}
		//	float ratio = (float)compressedSize / texData.ContentSize;
		//	if (ratio < RLECompressRatioThreshold)
		//	{
		//		texData.Flags = TextureData::TDF_RLECompressed;
		//	}
		//}

		texData.Save(FileOutStream(config.DestinationFile));
	}
	
	void TextureBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		TextureBuildConfig config;
		config.Parse(sect);

		if (!config.AssembleCubemap && !config.AssembleVolumeMap && !File::FileExists(config.SourceFile))
		{
			BuildSystem::LogError(config.SourceFile, L"Could not find source file.");
			return;
		}
		if (!config.AssembleCubemap && !config.AssembleVolumeMap && config.SourceAlphaFile.size() && !File::FileExists(config.SourceAlphaFile))
		{
			BuildSystem::LogError(config.SourceAlphaFile, L"Could not find source alpha file.");
			return;
		}

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestinationFile));

		switch (config.Method)
		{
			case TextureBuildMethod::D3D: BuildByD3D(config); break;
			case TextureBuildMethod::Devil: BuildByDevIL(config); break;
		}

		BuildSystem::LogEntryProcessed(config.DestinationFile, hierarchyPath);
	}
}