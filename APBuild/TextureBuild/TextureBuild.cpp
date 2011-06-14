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
#include "TextureBuild.h"
#include "Config/ConfigurationSection.h"

#include "IOLib/TextureData.h"
#include "IOLib/Streams.h"
#include "Graphics/PixelFormat.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::IO;

namespace APBuild
{
	enum TextureFilterType
	{
		TFLT_Nearest,
		TFLT_Box,
		TFLT_BSpline
	};
	struct TexConfig 
	{
		String SourceFile;
		String DestinationFile;
		bool GenerateMipmaps;
		int NewWidth;
		int NewHeight;
		int NewDepth;
		TextureFilterType ResizeFilterType;
		PixelFormat NewFormat;

		void Parse(const ConfigurationSection* sect)
		{

		}
	};
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
		return FMT_Unknown;
	}

	void TextureBuild::Build(const ConfigurationSection* sect)
	{
		TexConfig config;
		config.Parse(sect);


		int image = ilGenImage();

		ilBindImage(image);
		ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);

		ilLoadImage(config.SourceFile);

		int ilFormat = ilGetInteger(IL_IMAGE_FORMAT);

		
		if (config.GenerateMipmaps)
		{
			iluBuildMipmaps();
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

		if (cubeFlags)
		{
			texData.Type = TT_CubeTexture;
		}
		else
		{
			texData.Type = depth0 > 1 ? TT_Texture3D : TT_Texture2D;
		}

		int dxtFormat = ilGetInteger(IL_DXTC_DATA_FORMAT);
		if (dxtFormat != IL_DXT_NO_COMP)
		{
			texData.Format = ConvertFormat(dxtFormat, 0, 0);
		}

		for (int i=0;i<mipCount;i++)
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

		FileOutStream* fs = new FileOutStream(config.DestinationFile);
		texData.Save(fs);
		delete fs;

		for (int i=0;i<mipCount;i++)
		{
			delete[] texData.Levels[i].ContentData;
		}

	}
}