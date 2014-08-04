#pragma once
#ifndef APOC3D_PIXELFORMAT_H
#define APOC3D_PIXELFORMAT_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Exception.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		enum DepthFormat
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
		/**
		 *  The pixel format used for images.
		 */		
		enum PixelFormat
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
	

		/**
		 *  Some functions for PixelFormat
		 */
		namespace PixelFormatUtils
		{
			/**
			 *  Check if a PixelFormat is a compressed format
			 */
			APAPI bool IsCompressed(PixelFormat format);
			APAPI int GetMemorySize(int width, int height, DepthFormat format);
			APAPI int GetMemorySize(int width, int height, int depth, PixelFormat format);

			/** 
			 *  Gets the num of bytes per pixel of a given format
			 */
			APAPI int GetBPP(PixelFormat fmt);
			APAPI int GetBPP(DepthFormat fmt);
			APAPI int GetStencilBitDepth(DepthFormat fmt);

			APAPI int GetChannelCount(PixelFormat fmt);
			APAPI void GetChannelBitDepth(PixelFormat fmt, int32 chnBitDepths[4]);

			/** 
			 *  Converts a PixelFormat's string representation to the PixelFormat enum
			 */
			APAPI PixelFormat ConvertFormat(const String& fmt);
			/** 
			 *  Converts a PixelFormat value to a string representation
			 */
			APAPI String ToString(PixelFormat format);

			/**
			 *  Converts a DepthFormat's string representation to the DepthFormat enum
			 */
			APAPI DepthFormat ConvertDepthFormat(const String& fmt);
			/** 
			 *  Converts a DepthFormat value to a string representation
			 */
			APAPI String ToString(DepthFormat format);

			APAPI void DumpPixelFormatName(Apoc3D::Collections::List<String>& names);

			/** 
			 *  Converts some pixels from a source format to a destination format
			 */
			APAPI bool ConvertPixels(const DataBox& src, const DataBox& dst);


			APAPI void Resize(const void* src, int srcWidth, int srcHeight, 
				void* dst, int dstWidth, int dstHeight, PixelFormat format);
		};

	}
}

#endif