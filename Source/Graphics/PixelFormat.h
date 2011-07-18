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
#ifndef PIXELFORMAT_H
#define PIXELFORMAT_H


#include "Common.h"
#include "Apoc3DException.h"

using namespace Apoc3D::Graphics::RenderSystem;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		enum APAPI DepthFormat
		{
			DEPFMT_Depth15Stencil1 = 0,
			DEPFMT_Depth16 = 1,
			DEPFMT_Depth16Lockable = 2,
			DEPFMT_Depth24X8 = 3,
			DEPFMT_Depth24Stencil4 = 4,
			DEPFMT_Depth24Stencil8 = 5,
			DEPFMT_Depth24Stencil8Single = 6,
			DEPFMT_Depth32 = 7,
			DEPFMT_Depth32Lockable = 8,
			DEPFMT_Depth32Single = 9,
			DEPFMT_Count = 10
		};
		/* The pixel format used for images.
		*/		
		enum APAPI PixelFormat
		{
			/*  Unknown pixel format.
			*/			
			FMT_Unknown = 0,
			/// <summary>
			///    8-bit pixel format, all bits luminance.
			/// </summary>
			FMT_Luminance8 = 1,
			/// <summary>
			///    16-bit pixel format, all bits luminance.
			/// </summary>
			FMT_Luminance16 = 2,
			FMT_Alpha1 = 3,
			/// <summary>
			///    8-bit pixel format, all bits alpha.
			/// </summary>
			FMT_Alpha8 = 4,
			/// <summary>
			///    8-bit pixel format, 4 bits alpha, 4 bits luminance.
			/// </summary>
			FMT_A4L4 = 5,
			/// <summary>
			///    16-bit pixel format, 8 bits for alpha, 8 bits for luminance.
			/// </summary>
			FMT_A8L8 = 6,
			/// <summary>
			///    16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
			/// </summary>
			FMT_R5G6B5 = 7,
			/// <summary>
			///    16-bit pixel format, 5 bits blue, 6 bits green, 5 bits red.
			/// </summary>
			FMT_B5G6R5 = 8,
			/// <summary>
			///    16-bit pixel format, 4 bits for alpha, red, green and blue.
			/// </summary>
			FMT_A4R4G4B4 = 9,
			/// <summary>
			///    16-bit pixel format, 1 bit for alpha, 5 bits for blue, green and red.
			/// </summary>
			FMT_A1R5G5B5 = 10,

			/// <summary>
			///    24-bit pixel format, 8 bits for red, green and blue.
			/// </summary>
			FMT_R8G8B8 = 11,

			/// <summary>
			///    24-bit pixel format, 8 bits for blue, green and red.
			/// </summary>
			FMT_B8G8R8 = 12,

			/// <summary>
			///    32-bit pixel format, 8 bits for alpha, red, green and blue.
			/// </summary>
			FMT_A8R8G8B8 = 13,

			/// <summary>
			///    32-bit pixel format, 8 bits for alpha, blue, green and red`.
			/// </summary>
			FMT_A8B8G8R8 = 14,

			/// <summary>
			///    32-bit pixel format, 8 bits for blue, green, red and alpha.
			/// </summary>
			FMT_B8G8R8A8 = 15,
			/// <summary>
			///    32-bit pixel format, 2 bits for alpha, 10 bits for red, green and blue.
			/// </summary>
			FMT_A2R10G10B10 = 16,

			/// <summary>
			///    32-bit pixel format, 10 bits for blue, green and red, 2 bits for alpha.
			/// </summary>
			FMT_A2B10G10R10 = 17,
			/// <summary>
			///    DDS (DirectDraw Surface) DXT1 format.
			/// </summary>
			FMT_DXT1 = 18,
			/// <summary>
			///    DDS (DirectDraw Surface) DXT2 format.
			/// </summary>
			FMT_DXT2 = 19,
			/// <summary>
			///    DDS (DirectDraw Surface) DXT3 format.
			/// </summary>
			FMT_DXT3 = 20,
			/// <summary>
			///    DDS (DirectDraw Surface) DXT4 format.
			/// </summary>
			FMT_DXT4 = 21,
			/// <summary>
			///    DDS (DirectDraw Surface) DXT5 format.
			/// </summary>
			FMT_DXT5 = 22,

			/// <summary>
			/// 64-bit floating point format using 16 bits for each channel (alpha, blue, green, red).
			/// </summary>
			FMT_A16B16G16R16F = 23,

			/// <summary>
			///    128-bit floating point format using 32 bits for each channel (alpha, blue,  green, red).
			/// </summary>
			FMT_A32B32G32R32F = 24,
			/// <summary>
			///    32-bit pixel format, 8 bits for red, green and blue.
			///    like PF_A8R8G8B8, but alpha will get discarded
			/// </summary>
			FMT_X8R8G8B8 = 25,
			/// <summary>
			///    32-bit pixel format, 8 bits for blue, green and red.
			///		like PF_A8R8G8B8, but alpha will get discarded
			/// </summary>
			FMT_X8B8G8R8 = 26,
			FMT_X1R5G5B5 = 27,
			/// <summary>
			///    32-bit pixel format, 8 bits for red, green, blue and alpha.
			/// </summary>
			FMT_R8G8B8A8 = 28,
			/// <summary>
			///		Depth texture format
			/// </summary>
			FMT_Depth = 29,
			/// <summary>
			///    64-bit pixel ABGR format using 16 bits for each channel.
			/// </summary>
			FMT_A16B16G16R16 = 30,
			/// <summary>
			///   8-bit pixel format, 3 bits red, 3 bits green, 2 bits blue.
			/// </summary>
			FMT_R3G3B2 = 31,
			/// <summary>
			///    16 bit floating point with a single channel (red)
			/// </summary>
			FMT_R16F = 32,
			/// <summary>
			///		32 bit floating point with a single channel (red)
			/// </summary>
			FMT_R32F = 33,
			/// <summary>
			/// 32-bit pixel format, 16-bit green, 16-bit red
			/// </summary>
			FMT_G16R16 = 34,
			/// <summary>
			///		32-bit pixel format, 2-channel floating point pixel format, 16 bits (float) for green, 16 bits (float) for red
			/// </summary>
			FMT_G16R16F = 35,
			/// <summary>
			///		64-bit pixel format, 2-channel floating point pixel format, 32 bits (float) for green, 32 bits (float) for red
			/// </summary>
			FMT_G32R32F = 36,
			/// <summary>
			/// 48-bit pixel format, 16 bits for red, green and blue
			/// </summary>
			FMT_R16G16B16 = 37,
			/// <summary>
			///    16-bit pixel format, 4 bits for blue, green, red and alpha.
			/// </summary>
			FMT_B4G4R4A4=38,
			FMT_Palette8 = 39,
			FMT_Palette8Alpha8 = 40,
			FMT_Count = 41
		};
	
		class APAPI PixelFormatUtils
		{
		private:
			static int sizeTable[FMT_Count];
			static int depSizeTable[DEPFMT_Count];

#pragma warning(push)
#pragma warning(disable:4251)
			static auto_ptr<PixelFormatUtils> initializer;
#pragma warning(pop)

			PixelFormatUtils();
		public:
			static bool IsCompressed(PixelFormat format)
			{
				return (format == FMT_DXT1 ||
					format == FMT_DXT2 ||
					format == FMT_DXT3 ||
					format == FMT_DXT4 ||
					format == FMT_DXT5);
			}
			static int GetMemorySize(int width, int height, DepthFormat format) 
			{
				int bytepp = depSizeTable[(int)format];
				if (bytepp == -1)
				{
					throw Apoc3DException::createException(EX_Default, L"Invalid pixel format");
				}
				return width * height * bytepp;
			}
			static int GetMemorySize(int width, int height, int depth, PixelFormat format)
			{
				if (format == FMT_DXT1)				
				{
					return ((width * 3) / 4) * ((height * 3) / 4) * 8;
				}

				if (format == FMT_DXT2 ||
					format == FMT_DXT3 ||
					format == FMT_DXT4 ||
					format == FMT_DXT5)
				{
					return ((width * 3) / 4) * ((height * 3) / 4) * 16;
				}
				int bytepp = sizeTable[(int)format];
				if (bytepp == -1)
				{
					throw Apoc3DException::createException(EX_Default, L"Invalid pixel format");
				}
				return width * height * depth * bytepp;
			}

			static String ToString(PixelFormat format)
			{
				switch (format)
				{
				case FMT_Luminance8:
					return L"L8";
				case FMT_Luminance16:
					return L"L16";
				case FMT_Alpha8:
					return L"A8";
				case FMT_A4L4:
					return L"A4L4";
				case FMT_A8L8:
					return L"A8L8";
				case FMT_R5G6B5:
					return L"R5G6B5";
				case FMT_B5G6R5:
					return L"B5G6R5";	
				case FMT_A4R4G4B4:
					return L"A4R4G4B4";
				case FMT_A1R5G5B5:
					return L"A1R5G5B5";
				case FMT_R8G8B8:
					return L"R8G8B8";
				case FMT_B8G8R8:
					return L"B8G8R8";
				case FMT_A8R8G8B8:
					return L"A8R8G8B8";
				case FMT_A8B8G8R8:
					return L"A8B8G8R8";
				case FMT_B8G8R8A8:
					return L"B8G8R8A8";
				case FMT_A2R10G10B10:
					return L"A2R10G10B10";
				case FMT_A2B10G10R10:
					return L"A2B10G10R10";
				case FMT_DXT1:
					return L"DXT1";
				case FMT_DXT2:
					return L"DXT2";
				case FMT_DXT3:
					return L"DXT3";
				case FMT_DXT4:
					return L"DXT4";
				case FMT_DXT5:
					return L"DXT5";
				case FMT_A16B16G16R16F:
					return L"A16B16G16R16F";
				case FMT_A32B32G32R32F:
					return L"A32B32G32R32F";
				case FMT_X8R8G8B8:
					return L"X8R8G8B8";
				case FMT_X8B8G8R8:
					return L"X8B8G8R8";
				case FMT_X1R5G5B5:
					return L"X1R5G5B5";
				case FMT_R8G8B8A8:
					return L"R8G8B8A8";
				case FMT_A16B16G16R16:
					return L"A16B16G16R16";
				case FMT_R3G3B2:
					return L"R3G3B2";
				case FMT_R16F:
					return L"R16F";
				case FMT_R32F:
					return L"R32F";
				case FMT_G16R16:
					return L"G16R16";
				case FMT_G16R16F:
					return L"G16R16F";
				case FMT_G32R32F:
					return L"G32R32F";
				case FMT_R16G16B16:
					return L"R16G16B16";
				case FMT_B4G4R4A4:
					return L"B4G4R4A4";
				case FMT_Palette8:
					return L"P8";
				case FMT_Palette8Alpha8:
					return L"P8A8";
				}
				return L"Unknown";
			}
			static PixelFormat ConvertFormat(const String& fmt)
			{
				if (fmt == String(L"L8"))
				{
					return FMT_Luminance8;
				}
				else if (fmt == String(L"L16"))
				{
					return FMT_Luminance16;
				}
				else if (fmt == String(L"A8"))
				{
					return FMT_Alpha8;
				}
				else if (fmt == String(L"A4L4"))
				{
					return FMT_A4L4;
				}
				else if (fmt == String(L"A8L8"))
				{
					return FMT_A8L8;
				}
				else if (fmt == String(L"R5G6B5"))
				{
					return FMT_R5G6B5;
				}
				else if (fmt == String(L"B5G6R5"))
				{
					return FMT_B5G6R5;
				}
				else if (fmt == String(L"A4R4G4B4"))
				{
					return FMT_A4R4G4B4;
				}
				else if (fmt == String(L"A1R5G5B5"))
				{
					return FMT_A1R5G5B5;
				}
				else if (fmt == String(L"R8G8B8"))
				{
					return FMT_R8G8B8;
				}
				else if (fmt == String(L"B8G8R8"))
				{
					return FMT_B8G8R8;
				}
				else if (fmt == String(L"A8R8G8B8"))
				{
					return FMT_A8R8G8B8;
				}
				else if (fmt == String(L"A8B8G8R8"))
				{
					return FMT_A8B8G8R8;
				}
				else if (fmt == String(L"B8G8R8A8"))
				{
					return FMT_B8G8R8A8;
				}
				else if (fmt == String(L"A2R10G10B10"))
				{
					return FMT_A2R10G10B10;
				}
				else if (fmt == String(L"A2B10G10R10"))
				{
					return FMT_A2B10G10R10;
				}
				else if (fmt == String(L"DXT1"))
				{
					return FMT_DXT1;
				}
				else if (fmt == String(L"DXT2"))
				{
					return FMT_DXT2;
				}
				else if (fmt == String(L"DXT3"))
				{
					return FMT_DXT3;
				}
				else if (fmt == String(L"DXT4"))
				{
					return FMT_DXT4;
				}
				else if (fmt == String(L"DXT5"))
				{
					return FMT_DXT5;
				}
				else if (fmt == String(L"A16B16G16R16F"))
				{
					return FMT_A16B16G16R16F;
				}
				else if (fmt == String(L"A32B32G32R32F"))
				{
					return FMT_A32B32G32R32F;
				}
				else if (fmt == String(L"X8R8G8B8"))
				{
					return FMT_X8R8G8B8;
				}
				else if (fmt == String(L"X8B8G8R8"))
				{
					return FMT_X8B8G8R8;
				}
				else if (fmt == String(L"X1R5G5B5"))
				{
					return FMT_X1R5G5B5;
				}
				else if (fmt == String(L"R8G8B8A8"))
				{
					return FMT_R8G8B8A8;
				}
				else if (fmt == String(L"A16B16G16R16"))
				{
					return FMT_A16B16G16R16;
				}
				else if (fmt == String(L"R3G3B2"))
				{
					return FMT_R3G3B2;
				}
				else if (fmt == String(L"R16F"))
				{
					return FMT_R16F;
				}
				else if (fmt == String(L"R32F"))
				{
					return FMT_R32F;
				}
				else if (fmt == String(L"G16R16"))
				{
					return FMT_G16R16;
				}
				else if (fmt == String(L"G16R16F"))
				{
					return FMT_G16R16F;
				}
				else if (fmt == String(L"G32R32F"))
				{
					return FMT_G32R32F;
				}
				else if (fmt == String(L"R16G16B16"))
				{
					return FMT_R16G16B16;
				}
				else if (fmt == String(L"B4G4R4A4"))
				{
					return FMT_B4G4R4A4;
				}
				else if (fmt == String(L"P8"))
				{
					return FMT_Palette8;
				}
				else if (fmt == String(L"P8A8"))
				{
					return FMT_Palette8Alpha8;
				}
				return FMT_Unknown;
			}
			static DepthFormat ConvertDepthFormat(const String& fmt)
			{
				if (fmt == String(L"D15S1"))
				{
					return DEPFMT_Depth15Stencil1;
				}
				else if (fmt == String(L"D16"))
				{
					return DEPFMT_Depth16;
				}
				else if (fmt == String(L"D16Lockable"))
				{
					return DEPFMT_Depth16Lockable;
				}
				else if (fmt == String(L"D24"))
				{
					return DEPFMT_Depth24X8;
				}
				else if (fmt == String(L"D24S4"))
				{
					return DEPFMT_Depth24Stencil4;
				}
				else if (fmt == String(L"D24S8"))
				{
					return DEPFMT_Depth24Stencil8;
				}
				else if (fmt == String(L"D24S8F"))
				{
					return DEPFMT_Depth24Stencil8Single;
				}
				else if (fmt == String(L"D32"))
				{
					return DEPFMT_Depth32;
				}
				else if (fmt == String(L"D32Lockable"))
				{
					return DEPFMT_Depth32Lockable;
				}
				else if (fmt == String(L"D32F"))
				{
					return DEPFMT_Depth32Single;
				}

				return DEPFMT_Depth16;
			}
			static int ConvertPixels(const DataBox& src, const DataBox& dst);


			static void Resize(const float* src, int srcWidth, int srcHeight,
				float* dst, int dstWidth, int dstHeight, int numChannels);
		};

	}
}

#endif