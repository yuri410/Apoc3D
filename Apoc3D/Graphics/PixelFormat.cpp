
#include "PixelFormat.h"
#include "LockData.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Utility/TypeConverter.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		const TypeDualConverter<PixelFormat> PixelFormatEnumConverter =
		{
			{ L"L8", FMT_Luminance8 },
			{ L"L16", FMT_Luminance16 },
			{ L"A8", FMT_Alpha8 },
			{ L"A4L4", FMT_A4L4 },
			{ L"A8L8", FMT_A8L8 },

			{ L"R5G6B5", FMT_R5G6B5 },
			{ L"R8G8B8", FMT_R8G8B8 },
			{ L"A8R8G8B8", FMT_A8R8G8B8 },
			{ L"A4R4G4B4", FMT_A4R4G4B4 },
			{ L"A1R5G5B5", FMT_A1R5G5B5 },

			{ L"B5G6R5", FMT_B5G6R5 },
			{ L"B8G8R8", FMT_B8G8R8 },
			{ L"B8G8R8A8", FMT_B8G8R8A8 },
			{ L"A8B8G8R8", FMT_A8B8G8R8 },

			{ L"A2R10G10B10", FMT_A2R10G10B10 },
			{ L"A2B10G10R10", FMT_A2B10G10R10 },

			{ L"DXT1", FMT_DXT1 },
			{ L"DXT2", FMT_DXT2 },
			{ L"DXT3", FMT_DXT3 },
			{ L"DXT4", FMT_DXT4 },
			{ L"DXT5", FMT_DXT5 },

			{ L"R16F", FMT_R16F },
			{ L"G16R16F", FMT_G16R16F },
			{ L"A16B16G16R16F", FMT_A16B16G16R16F },

			{ L"R32F", FMT_R32F },
			{ L"G32R32F", FMT_G32R32F },
			{ L"A32B32G32R32F", FMT_A32B32G32R32F },

			{ L"X8R8G8B8", FMT_X8R8G8B8 },
			{ L"X8B8G8R8", FMT_X8B8G8R8 },
			{ L"X1R5G5B5", FMT_X1R5G5B5 },

			{ L"R8G8B8A8", FMT_R8G8B8A8 },

			{ L"A16B16G16R16", FMT_A16B16G16R16 },

			{ L"R3G3B2", FMT_R3G3B2 },
			{ L"B4G4R4A4", FMT_B4G4R4A4 },

			{ L"G16R16", FMT_G16R16 },
			{ L"R16G16B16", FMT_R16G16B16 },

			{ L"P8", FMT_Palette8 },
			{ L"P8A8", FMT_Palette8Alpha8 },

			{ L"Unknown", FMT_Unknown }
		};

		const TypeDualConverter<DepthFormat> DepthFormatEnumConveter =
		{
			{ L"D15S1", DEPFMT_Depth15Stencil1 },
			{ L"D16", DEPFMT_Depth16 },
			{ L"D16Lockable", DEPFMT_Depth16Lockable },
			{ L"D24", DEPFMT_Depth24X8 },
			{ L"D24S4", DEPFMT_Depth24Stencil4 },
			{ L"D24S8", DEPFMT_Depth24Stencil8 },
			{ L"D24S8F", DEPFMT_Depth24Stencil8Single },
			{ L"D32", DEPFMT_Depth32 },
			{ L"D32Lockable", DEPFMT_Depth32Lockable },
			{ L"D32F", DEPFMT_Depth32Single },
		};

		struct SizeTable
		{
			int pfSizeTable[FMT_Count];
			int dfSizeTable[DEPFMT_Count];
			int pfChannelCountTable[FMT_Count];
			int pfChannelBits[FMT_Count][4];

			SizeTable()
			{
				memset(pfSizeTable, 0, sizeof(pfSizeTable));
				memset(dfSizeTable, 0, sizeof(dfSizeTable));
				memset(pfChannelCountTable, 0, sizeof(pfChannelCountTable));
				memset(pfChannelBits, 0, sizeof(pfChannelBits));

				createPixelFormatSizeTable(pfSizeTable);
				createDepthFormatSizeTable(dfSizeTable);
				createChannelCountTable(pfChannelCountTable);
				createChannelBit(pfChannelBits);
			}

			void createPixelFormatSizeTable(int st[]);
			void createDepthFormatSizeTable(int st[]);
			void createChannelCountTable(int st[]);
			void createChannelBit(int (&st)[FMT_Count][4]);

		} static FormatSizeTable;
		
		PixelFormat PixelFormatUtils::ConvertFormat(const String& fmt) { return PixelFormatEnumConverter.Parse(fmt); }
		String PixelFormatUtils::ToString(PixelFormat format) { return PixelFormatEnumConverter.ToString(format); }

		DepthFormat PixelFormatUtils::ConvertDepthFormat(const String& fmt) { return DepthFormatEnumConveter.Parse(fmt); }
		String PixelFormatUtils::ToString(DepthFormat format) { return DepthFormatEnumConveter.ToString(format); }

		void PixelFormatUtils::DumpPixelFormatName(Apoc3D::Collections::List<String>& names) { PixelFormatEnumConverter.DumpNames(names); }

		bool PixelFormatUtils::IsCompressed(PixelFormat format)
		{
			return (format == FMT_DXT1 ||
				format == FMT_DXT2 ||
				format == FMT_DXT3 ||
				format == FMT_DXT4 ||
				format == FMT_DXT5);
		}
		int PixelFormatUtils::GetMemorySize(int width, int height, DepthFormat format) 
		{
			int bytepp = FormatSizeTable.dfSizeTable[(int)format];
			if (bytepp == -1)
			{
				throw AP_EXCEPTION(ExceptID::Default, L"Invalid pixel format");
			}
			return width * height * bytepp;
		}

		int PixelFormatUtils::GetMemorySize(int width, int height, int depth, PixelFormat format)
		{
			if (format == FMT_DXT1)				
			{
				return ((width + 3) / 4) * ((height + 3) / 4) * 8;
			}

			if (format == FMT_DXT2 ||
				format == FMT_DXT3 ||
				format == FMT_DXT4 ||
				format == FMT_DXT5)
			{
				return ((width + 3) / 4) * ((height + 3) / 4) * 16;
			}
			int bytepp = FormatSizeTable.pfSizeTable[(int)format];
			if (bytepp == -1)
			{
				throw AP_EXCEPTION(ExceptID::Default, L"Invalid pixel format");
			}
			return width * height * depth * bytepp;
		}
		int PixelFormatUtils::GetBPP(PixelFormat fmt) { return FormatSizeTable.pfSizeTable[(int)fmt]; }
		int PixelFormatUtils::GetBPP(DepthFormat fmt) { return FormatSizeTable.dfSizeTable[(int)fmt]; }
		int PixelFormatUtils::GetStencilBitDepth(DepthFormat fmt)
		{
			switch (fmt)
			{
			case DEPFMT_Depth15Stencil1:
				return 1;
			case DEPFMT_Depth24Stencil4:
				return 4;
			case DEPFMT_Depth24Stencil8:
				return 8;
			case DEPFMT_Depth24Stencil8Single:
				return 8;

			}
			return 0;
		}
		int PixelFormatUtils::GetChannelCount(PixelFormat fmt) { return FormatSizeTable.pfChannelCountTable[(int)fmt]; }
		void PixelFormatUtils::GetChannelBitDepth(PixelFormat fmt, int32 chnBitDepths[4])
		{
			memcpy(chnBitDepths, FormatSizeTable.pfChannelBits[(int32)fmt], sizeof(chnBitDepths));
		}

	

		void SizeTable::createPixelFormatSizeTable(int st[])
		{
			st[(int)FMT_Unknown] = -1;
			st[(int)FMT_Luminance8] = 1;
			st[(int)FMT_Luminance16] = 2;
			st[(int)FMT_Alpha1] = -1;
			st[(int)FMT_Alpha8] = 1;
			st[(int)FMT_A4L4] = 1;
			st[(int)FMT_A8L8] = 2;
			st[(int)FMT_R5G6B5] = 2;
			st[(int)FMT_B5G6R5] = 2;
			st[(int)FMT_A4R4G4B4] = 2;
			st[(int)FMT_A1R5G5B5] = 2;
			st[(int)FMT_R8G8B8] = 3;
			st[(int)FMT_B8G8R8] = 3;
			st[(int)FMT_A8R8G8B8] = 4;
			st[(int)FMT_A8B8G8R8] = 4;
			st[(int)FMT_B8G8R8A8] = 4;
			st[(int)FMT_A2B10G10R10] = 4;
			st[(int)FMT_A2R10G10B10] = 4;
			st[(int)FMT_DXT1] = -1;
			st[(int)FMT_DXT2] = -1;
			st[(int)FMT_DXT3] = -1;
			st[(int)FMT_DXT4] = -1;
			st[(int)FMT_DXT5] = -1;
			st[(int)FMT_A16B16G16R16F] = 8;
			st[(int)FMT_A32B32G32R32F] = 16;
			st[(int)FMT_X8R8G8B8] = 4;
			st[(int)FMT_X8B8G8R8] = 4;
			st[(int)FMT_X1R5G5B5] = 4;
			st[(int)FMT_R8G8B8A8] = 4;
			st[(int)FMT_Depth] = -1;
			st[(int)FMT_A16B16G16R16] = 8;
			st[(int)FMT_R3G3B2] = 1;
			st[(int)FMT_R16F] = 2;
			st[(int)FMT_R32F] = 4;
			st[(int)FMT_G16R16] = 4;
			st[(int)FMT_G16R16F] = 4;
			st[(int)FMT_G32R32F] = 8;
			st[(int)FMT_R16G16B16] = 6;
			st[(int)FMT_B4G4R4A4] = 2;
			st[(int)FMT_Palette8] = 1;
			st[(int)FMT_Palette8Alpha8] = 2;
		}
		void SizeTable::createDepthFormatSizeTable(int st[])
		{			
			st[(int)DEPFMT_Depth15Stencil1] = 2;
			st[(int)DEPFMT_Depth16] = 2;
			st[(int)DEPFMT_Depth16Lockable] = 2;
			st[(int)DEPFMT_Depth24X8] = 4;
			st[(int)DEPFMT_Depth24Stencil4] = 4;
			st[(int)DEPFMT_Depth24Stencil8] = 4;
			st[(int)DEPFMT_Depth24Stencil8Single] = 4;
			st[(int)DEPFMT_Depth32] = 4;
			st[(int)DEPFMT_Depth32Lockable] = 4;
			st[(int)DEPFMT_Depth32Single] = 4;		
		}
		void SizeTable::createChannelCountTable(int st[])
		{
			st[(int)FMT_Unknown] = 0;
			st[(int)FMT_Luminance8] = 1;
			st[(int)FMT_Luminance16] = 1;
			st[(int)FMT_Alpha1] = 1;
			st[(int)FMT_Alpha8] = 1;
			st[(int)FMT_A4L4] = 2;
			st[(int)FMT_A8L8] = 2;
			st[(int)FMT_R5G6B5] = 3;
			st[(int)FMT_B5G6R5] = 3;
			st[(int)FMT_A4R4G4B4] = 4;
			st[(int)FMT_A1R5G5B5] = 4;
			st[(int)FMT_R8G8B8] = 3;
			st[(int)FMT_B8G8R8] = 3;
			st[(int)FMT_A8R8G8B8] = 4;
			st[(int)FMT_A8B8G8R8] = 4;
			st[(int)FMT_B8G8R8A8] = 4;
			st[(int)FMT_A2B10G10R10] = 4;
			st[(int)FMT_A2R10G10B10] = 4;
			st[(int)FMT_DXT1] = 0;
			st[(int)FMT_DXT2] = 0;
			st[(int)FMT_DXT3] = 0;
			st[(int)FMT_DXT4] = 0;
			st[(int)FMT_DXT5] = 0;
			st[(int)FMT_A16B16G16R16F] = 4;
			st[(int)FMT_A32B32G32R32F] = 4;
			st[(int)FMT_X8R8G8B8] = 4;
			st[(int)FMT_X8B8G8R8] = 4;
			st[(int)FMT_X1R5G5B5] = 4;
			st[(int)FMT_R8G8B8A8] = 4;
			st[(int)FMT_Depth] = 0;
			st[(int)FMT_A16B16G16R16] = 4;
			st[(int)FMT_R3G3B2] = 3;
			st[(int)FMT_R16F] = 1;
			st[(int)FMT_R32F] = 1;
			st[(int)FMT_G16R16] = 2;
			st[(int)FMT_G16R16F] = 2;
			st[(int)FMT_G32R32F] = 2;
			st[(int)FMT_R16G16B16] = 3;
			st[(int)FMT_B4G4R4A4] = 2;
			st[(int)FMT_Palette8] = 1;
			st[(int)FMT_Palette8Alpha8] = 2;
		}

		static void setArr(int (&arr)[4], int a, int b, int c, int d)
		{
			arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
		}
		static void setArr(int(&arr)[4], int a)
		{
			arr[0] = arr[1] = arr[2] = arr[3] = a;
		}
		void SizeTable::createChannelBit(int (&st)[FMT_Count][4])
		{
			setArr(st[(int)FMT_Unknown], 0);
			setArr(st[(int)FMT_Luminance8], 8,0,0,0);
			setArr(st[(int)FMT_Luminance16], 16,0,0,0);
			setArr(st[(int)FMT_Alpha1], 1,0,0,0);
			setArr(st[(int)FMT_Alpha8], 8,0,0,0);
			setArr(st[(int)FMT_A4L4], 4,4,0,0);
			setArr(st[(int)FMT_A8L8], 8,8,0,0);
			setArr(st[(int)FMT_R5G6B5], 5,6,5,0);
			setArr(st[(int)FMT_B5G6R5], 5,6,5,0);
			setArr(st[(int)FMT_A4R4G4B4], 4);
			setArr(st[(int)FMT_A1R5G5B5], 1,5,6,5);
			setArr(st[(int)FMT_R8G8B8], 8,8,8,0);
			setArr(st[(int)FMT_B8G8R8], 8,8,8,0);
			setArr(st[(int)FMT_A8R8G8B8], 8);
			setArr(st[(int)FMT_A8B8G8R8], 8);
			setArr(st[(int)FMT_B8G8R8A8], 8);
			setArr(st[(int)FMT_A2R10G10B10], 2,10,10,10);
			setArr(st[(int)FMT_A2B10G10R10], 2,10,10,10);
			setArr(st[(int)FMT_DXT1], 0);
			setArr(st[(int)FMT_DXT2], 0);
			setArr(st[(int)FMT_DXT3], 0);
			setArr(st[(int)FMT_DXT4], 0);
			setArr(st[(int)FMT_DXT5], 0);
			setArr(st[(int)FMT_A16B16G16R16F], 16);
			setArr(st[(int)FMT_A32B32G32R32F], 32);
			setArr(st[(int)FMT_X8R8G8B8], 8);
			setArr(st[(int)FMT_X8B8G8R8], 8);
			setArr(st[(int)FMT_X1R5G5B5], 1,5,5,5);
			setArr(st[(int)FMT_R8G8B8A8], 8);
			setArr(st[(int)FMT_Depth], 0);
			setArr(st[(int)FMT_A16B16G16R16], 16);
			setArr(st[(int)FMT_R3G3B2], 3,3,2,0);
			setArr(st[(int)FMT_R16F], 16,0,0,0);
			setArr(st[(int)FMT_R32F], 32,0,0,0);
			setArr(st[(int)FMT_G16R16], 16,16,0,0);
			setArr(st[(int)FMT_G16R16F], 16,16,0,0);
			setArr(st[(int)FMT_G32R32F], 32,32,0,0);
			setArr(st[(int)FMT_R16G16B16], 16,16,16,0);
			setArr(st[(int)FMT_B4G4R4A4], 4);
			setArr(st[(int)FMT_Palette8], 8,0,0,0);
			setArr(st[(int)FMT_Palette8Alpha8], 8,8,0,0);
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// BEGIN PixelConverters
		// 
		// -----------------------------------------------------------------------------
		// Copyright (c) 2000-2013 Torus Knot Software Ltd
		//
		// Permission is hereby granted, free of charge, to any person obtaining a copy
		// of this software and associated documentation files (the "Software"), to deal
		// in the Software without restriction, including without limitation the rights
		// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		// copies of the Software, and to permit persons to whom the Software is
		// furnished to do so, subject to the following conditions:
		//
		// The above copyright notice and this permission notice shall be included in
		// all copies or substantial portions of the Software.
		// 
		// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
		// THE SOFTWARE.
		// -----------------------------------------------------------------------------
		// NOTICE: This section of code has been modified. For the original source, find in http://www.ogre3d.org/
		// 

#define PACKCONVERTERID(srcFmtId, dstFmtId)  ((srcFmtId << 16) | dstFmtId)
		template <typename T, typename U, uint32 srcFmtId, uint32 dstFmtId> 
		struct PixelConverter 
		{
			static const uint32 ID = PACKCONVERTERID(srcFmtId, dstFmtId);
			typedef T SrcType;
			typedef U DstType;
		};

		struct Col3b 
		{
			Col3b(uint a, uint b, uint c)
				: x((byte)a), y((byte)b), z((byte)c) { }
			byte x,y,z;
		};

		struct ColArgb4f
		{
			ColArgb4f(float _a, float _r, float _g, float _b)
				: a(_a), r(_r), g(_g), b(_b) { }
			float a,r,g,b;
		};
		
		struct ColArgb4h
		{
			ColArgb4h(uint16 _a, uint16 _r, uint16 _g, uint16 _b)
				: a(_a), r(_r), g(_g), b(_b) { }
			uint16 a,r,g,b;
		};


		struct A8R8G8B8toA8B8G8R8 : public PixelConverter<uint, uint, FMT_A8R8G8B8, FMT_A8B8G8R8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
			}
		};

		struct A8R8G8B8toB8G8R8A8 : public PixelConverter<uint, uint, FMT_A8R8G8B8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};
		struct A8R8G8B8toR8G8B8A8 : public PixelConverter<uint, uint, FMT_A8R8G8B8, FMT_R8G8B8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
			}
		};

		struct A8B8G8R8toA8R8G8B8 : public PixelConverter<uint, uint, FMT_A8B8G8R8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
			}
		};

		struct A8B8G8R8toB8G8R8A8 : public PixelConverter<uint, uint, FMT_A8B8G8R8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
			}
		};

		struct A8B8G8R8toR8G8B8A8 : public PixelConverter<uint, uint, FMT_A8B8G8R8, FMT_R8G8B8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};


		struct B8G8R8A8toA8R8G8B8 : public PixelConverter<uint, uint, FMT_B8G8R8A8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};

		struct B8G8R8A8toA8B8G8R8
			: public PixelConverter<uint, uint, FMT_B8G8R8A8, FMT_A8B8G8R8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
			}
		};

		struct B8G8R8A8toR8G8B8A8 : public PixelConverter<uint, uint, FMT_B8G8R8A8, FMT_R8G8B8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
			}
		};

		struct R8G8B8A8toA8R8G8B8 : public PixelConverter<uint, uint, FMT_R8G8B8A8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
			}
		};


		struct R8G8B8A8toA8B8G8R8 : public PixelConverter<uint, uint, FMT_R8G8B8A8, FMT_A8B8G8R8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};

		struct R8G8B8A8toB8G8R8A8 : public PixelConverter<uint, uint, FMT_R8G8B8A8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
			}
		};


		struct A8B8G8R8toL8 : public PixelConverter<uint, byte, FMT_A8B8G8R8, FMT_Luminance8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return (byte)(inp&0x000000FF);
			}
		};

		struct L8toA8B8G8R8 : public PixelConverter<byte, uint, FMT_Luminance8, FMT_A8B8G8R8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
			}
		};

		struct A8R8G8B8toL8 : public PixelConverter<uint, byte, FMT_A8R8G8B8, FMT_Luminance8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return (byte)((inp&0x00FF0000)>>16);
			}
		};

		struct L8toA8R8G8B8 : public PixelConverter<byte, uint, FMT_Luminance8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
			}
		};


		struct B8G8R8A8toL8 : public PixelConverter<uint, byte, FMT_B8G8R8A8, FMT_Luminance8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return (byte)((inp&0x0000FF00)>>8);
			}
		};

		struct L8toB8G8R8A8 : public PixelConverter<byte, uint, FMT_Luminance8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return 0x000000FF|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16)|(((unsigned int)inp)<<24);
			}
		};


		struct L8toL16 : public PixelConverter<byte, uint16, FMT_Luminance8, FMT_Luminance16>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return (uint16)((((unsigned int)inp)<<8)|(((unsigned int)inp)));
			}
		};

		struct L16toL8
			: public PixelConverter<uint16, byte, FMT_Luminance16, FMT_Luminance8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return (byte)(inp>>8);
			}
		};


		struct R8G8B8toB8G8R8 : public PixelConverter<Col3b, Col3b, FMT_R8G8B8, FMT_B8G8R8>
		{
			static DstType ConvertPixel(const SrcType &inp)
			{
				return Col3b(inp.z, inp.y, inp.x);
			}  
		};

		struct B8G8R8toR8G8B8 : public PixelConverter<Col3b, Col3b, FMT_B8G8R8, FMT_R8G8B8>
		{
			static DstType ConvertPixel(const SrcType &inp)
			{
				return Col3b(inp.z, inp.y, inp.x);
			}  
		};

		// X8Y8Z8 ->  X8<<xshift Y8<<yshift Z8<<zshift A8<<ashift
		template <uint32 srcFmtId, uint32 dstFmtId, unsigned int xshift, unsigned int yshift, unsigned int zshift, unsigned int ashift>
		struct Col3btoUint32swizzler
			: public PixelConverter<Col3b, uint, srcFmtId, dstFmtId>
		{
			static uint ConvertPixel(const Col3b &inp)
			{
#ifndef BIG_ENDIAN
				return (0xFF<<ashift) | (((unsigned int)inp.x)<<zshift) | (((unsigned int)inp.y)<<yshift) | (((unsigned int)inp.z)<<xshift);
#else
				return (0xFF<<ashift) | (((unsigned int)inp.x)<<xshift) | (((unsigned int)inp.y)<<yshift) | (((unsigned int)inp.z)<<zshift);
#endif
			}
		};

		struct R8G8B8toX8R8G8B8: public Col3btoUint32swizzler<FMT_R8G8B8, FMT_X8R8G8B8, 16, 8, 0, 24> { };
		struct R8G8B8toA8R8G8B8: public Col3btoUint32swizzler<FMT_R8G8B8, FMT_A8R8G8B8, 16, 8, 0, 24> { };
		struct B8G8R8toA8R8G8B8: public Col3btoUint32swizzler<FMT_B8G8R8, FMT_A8R8G8B8, 0, 8, 16, 24> { };
		struct R8G8B8toA8B8G8R8: public Col3btoUint32swizzler<FMT_R8G8B8, FMT_A8B8G8R8, 0, 8, 16, 24> { };
		struct B8G8R8toA8B8G8R8: public Col3btoUint32swizzler<FMT_B8G8R8, FMT_A8B8G8R8, 16, 8, 0, 24> { };
		struct R8G8B8toB8G8R8A8: public Col3btoUint32swizzler<FMT_R8G8B8, FMT_B8G8R8A8, 8, 16, 24, 0> { };
		struct B8G8R8toB8G8R8A8: public Col3btoUint32swizzler<FMT_B8G8R8, FMT_B8G8R8A8, 24, 16, 8, 0> { };
		


		struct A8R8G8B8toR8G8B8 : public PixelConverter<uint, Col3b, FMT_A8R8G8B8, FMT_R8G8B8>
		{
			static DstType ConvertPixel(uint inp)
			{
				return Col3b((byte)((inp>>16)&0xFF), (byte)((inp>>8)&0xFF), (byte)((inp>>0)&0xFF));
			}
		};
		struct A8R8G8B8toB8G8R8 : public PixelConverter<uint, Col3b, FMT_A8R8G8B8, FMT_B8G8R8>
		{
			static DstType ConvertPixel(uint inp)
			{
				return Col3b((byte)((inp>>0)&0xFF), (byte)((inp>>8)&0xFF), (byte)((inp>>16)&0xFF));
			}
		};
		struct X8R8G8B8toA8R8G8B8 : public PixelConverter<uint, uint, FMT_X8R8G8B8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return inp | 0xFF000000;
			}
		};

		struct X8R8G8B8toA8B8G8R8 : public PixelConverter<uint, uint, FMT_X8R8G8B8, FMT_A8B8G8R8>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
			}
		};
		struct X8R8G8B8toB8G8R8A8 : public PixelConverter <uint, uint, FMT_X8R8G8B8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
			}
		};
		struct X8R8G8B8toR8G8B8A8 : public PixelConverter<uint, uint, FMT_X8R8G8B8, FMT_R8G8B8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0xFFFFFF)<<8)|0x000000FF;
			}
		};


		// X8B8G8R8
		struct X8B8G8R8toA8R8G8B8 : public PixelConverter<uint, uint, FMT_X8B8G8R8, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
			}
		};
		struct X8B8G8R8toA8B8G8R8 : public PixelConverter<uint, uint, FMT_X8B8G8R8, FMT_A8B8G8R8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return inp | 0xFF000000;
			}
		};
		struct X8B8G8R8toB8G8R8A8 : public PixelConverter<uint, uint, FMT_X8B8G8R8, FMT_B8G8R8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0xFFFFFF)<<8)|0x000000FF;
			}
		};
		struct X8B8G8R8toR8G8B8A8 : public PixelConverter<uint, uint, FMT_X8B8G8R8, FMT_R8G8B8A8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
			}
		};
		
		template <class U>
		static void BoxConversion(const DataBox& src, const DataBox& dst)
		{
			typename U::SrcType *srcptr = static_cast<typename U::SrcType*>(src.getDataPointer());
			typename U::DstType *dstptr = static_cast<typename U::DstType*>(dst.getDataPointer());
			const int srcSliceSkip = src.getSlicePitch();
			const int dstSliceSkip = dst.getSlicePitch();

			for (int z=0;z<src.getDepth();z++)
			{
				for (int y=0;y<src.getHeight();y++)
				{
					for (int x=0;x<src.getWidth();x++)
					{
						dstptr[x] = U::ConvertPixel(srcptr[x]);
					}
					srcptr = reinterpret_cast<typename U::SrcType*>((byte*)srcptr + src.getRowPitch());
					dstptr = reinterpret_cast<typename U::DstType*>((byte*)dstptr + dst.getRowPitch());
				}
				srcptr = reinterpret_cast<typename U::SrcType*>((byte*)srcptr + srcSliceSkip);
				dstptr = reinterpret_cast<typename U::DstType*>((byte*)dstptr + dstSliceSkip);
			} 
		}

		// END Pixel Converters
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		/*inline uint16 ch_r32u16(float v)
		{
			v = Math::Saturate(v);
			return static_cast<uint16>(v*65535);
		}*/
		inline uint16 ch_r32r16(float v)
		{
			return Math::R32ToR16(v);
		}
		inline float ch_r16r32(uint16 v)
		{
			return Math::R16ToR32(v);
		}
		inline byte ch_r32u8(float v)
		{
			v = Math::Saturate(v);
			return static_cast<byte>(v*255);
		}

		/*inline float ch_u16r32(uint16 v)
		{
			return v / 65535.0f;
		}*/
		inline byte ch_r16u8(uint16 v)
		{
			return ch_r32u8(Math::R16ToR32(v));
		}
		inline float ch_u8r32(byte v)
		{
			return v / 255.0f;
		}
		inline uint16 ch_u8r16(byte v)
		{
			return Math::R32ToR16(v / 255.0f);
		}

		struct A8R8G8B8toA32B32G32R32F : public PixelConverter<uint, ColArgb4f, FMT_A8R8G8B8, FMT_A32B32G32R32F>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				float a = ch_u8r32(CV_GetColorA(inp));
				float r = ch_u8r32(CV_GetColorR(inp));
				float g = ch_u8r32(CV_GetColorG(inp));
				float b = ch_u8r32(CV_GetColorB(inp));

				return ColArgb4f(a,r,g,b);
			}
		};

		struct A32B32G32R32FtoA8R8G8B8 : public PixelConverter<ColArgb4f, uint, FMT_A32B32G32R32F, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				byte a = ch_r32u8(inp.a);
				byte r = ch_r32u8(inp.r);
				byte g = ch_r32u8(inp.g);
				byte b = ch_r32u8(inp.b);

				return CV_PackColor(r,g,b,a);
			}
		};


		struct A32B32G32R32FtoA16B16G16R16F : public PixelConverter<ColArgb4f, ColArgb4h, FMT_A32B32G32R32F, FMT_A16B16G16R16F>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				uint16 a = ch_r32r16(inp.a);
				uint16 r = ch_r32r16(inp.r);
				uint16 g = ch_r32r16(inp.g);
				uint16 b = ch_r32r16(inp.b);

				return ColArgb4h(a,r,g,b);
			}
		};

		struct A16B16G16R16FtoA32B32G32R32F : public PixelConverter<ColArgb4h, ColArgb4f, FMT_A16B16G16R16F, FMT_A32B32G32R32F>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				float a = ch_r16r32(inp.a);
				float r = ch_r16r32(inp.r);
				float g = ch_r16r32(inp.g);
				float b = ch_r16r32(inp.b);

				return ColArgb4f(a,r,g,b);
			}
		};


		struct A8R8G8B8toA16B16G16R16F : public PixelConverter<uint, ColArgb4h, FMT_A8R8G8B8, FMT_A16B16G16R16F>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				uint16 a = ch_u8r16(CV_GetColorA(inp));
				uint16 r = ch_u8r16(CV_GetColorR(inp));
				uint16 g = ch_u8r16(CV_GetColorG(inp));
				uint16 b = ch_u8r16(CV_GetColorB(inp));

				return ColArgb4h(a,r,g,b);
			}
		};
		struct A16B16G16R16FtoA8R8G8B8 : public PixelConverter<ColArgb4h, uint, FMT_A16B16G16R16F, FMT_A8R8G8B8>
		{
			static DstType ConvertPixel(SrcType inp)
			{
				byte a = ch_r16u8(inp.a);
				byte r = ch_r16u8(inp.r);
				byte g = ch_r16u8(inp.g);
				byte b = ch_r16u8(inp.b);

				return CV_PackColor(r,g,b,a);
			}
		};


		struct ConveterHelper
		{
			typedef void (*ConversionRoutine)(const DataBox& src, const DataBox& dst);
			Apoc3D::Collections::HashMap<uint, ConversionRoutine> Converters;

			ConveterHelper()
				: Converters(25)
			{
				Register<A8R8G8B8toA8B8G8R8>();
				Register<A8R8G8B8toB8G8R8A8>();
				Register<A8R8G8B8toR8G8B8A8>();
				Register<A8B8G8R8toA8R8G8B8>();
				Register<A8B8G8R8toB8G8R8A8>();
				Register<A8B8G8R8toR8G8B8A8>();
				Register<B8G8R8A8toA8R8G8B8>();
				Register<B8G8R8A8toA8B8G8R8>();
				Register<B8G8R8A8toR8G8B8A8>();
				Register<R8G8B8A8toA8R8G8B8>();
				Register<R8G8B8A8toA8B8G8R8>();
				Register<R8G8B8A8toB8G8R8A8>();
				Register<A8B8G8R8toL8>();
				Register<L8toA8B8G8R8>();
				Register<A8R8G8B8toL8>();
				Register<L8toA8R8G8B8>();
				Register<B8G8R8A8toL8>();
				Register<L8toB8G8R8A8>();
				Register<L8toL16>();
				Register<L16toL8>();
				Register<B8G8R8toR8G8B8>();
				Register<R8G8B8toB8G8R8>();
				Register<R8G8B8toX8R8G8B8>();
				Register<R8G8B8toA8R8G8B8>();
				Register<B8G8R8toA8R8G8B8>();
				Register<R8G8B8toA8B8G8R8>();
				Register<B8G8R8toA8B8G8R8>();
				Register<R8G8B8toB8G8R8A8>();
				Register<B8G8R8toB8G8R8A8>();
				Register<A8R8G8B8toR8G8B8>();
				Register<A8R8G8B8toB8G8R8>();
				Register<X8R8G8B8toA8R8G8B8>();
				Register<X8R8G8B8toA8B8G8R8>();
				Register<X8R8G8B8toB8G8R8A8>();
				Register<X8R8G8B8toR8G8B8A8>();
				Register<X8B8G8R8toA8R8G8B8>();
				Register<X8B8G8R8toA8B8G8R8>();
				Register<X8B8G8R8toB8G8R8A8>();
				Register<X8B8G8R8toR8G8B8A8>();

				Register<A8R8G8B8toA32B32G32R32F>();
				Register<A32B32G32R32FtoA8R8G8B8>();

				Register<A16B16G16R16FtoA32B32G32R32F>();
				Register<A32B32G32R32FtoA16B16G16R16F>();

				Register<A8R8G8B8toA16B16G16R16F>();
				Register<A16B16G16R16FtoA8R8G8B8>();	
			}

			template <typename type>
			void Register()
			{
				Converters.Add(type::ID, &BoxConversion<type>);
			}

			bool Convert(const DataBox& src, const DataBox& dst)
			{
				ConversionRoutine func;
				if (Converters.TryGetValue(PACKCONVERTERID(src.getFormat(), dst.getFormat()), func))
				{
					func(src, dst);
					return true;
				}
				return false;
			}

		} static converterHelper;


		bool PixelFormatUtils::ConvertPixels(const DataBox& src, const DataBox& dst)
		{
			return converterHelper.Convert(src, dst);
		}

		void PixelFormatUtils::Resize(const void* src, int srcWidth, int srcHeight,
			void* dst, int dstWidth, int dstHeight, PixelFormat format)
		{
			int lvlSize = PixelFormatUtils::GetMemorySize(
				dstWidth, dstHeight, 1, format);

			memset(dst, 0, lvlSize);
		}


	}
}