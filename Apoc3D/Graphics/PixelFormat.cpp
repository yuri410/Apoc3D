
#include "PixelFormat.h"
#include "LockData.h"
#include "apoc3d/Math/Half.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/EnumConverterHelper.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		struct PixelFormatEnumHelper : public Apoc3D::Collections::EnumDualConversionHelper<PixelFormat>
		{
			PixelFormatEnumHelper();
		} static PixelFormatEnumConverter;
		struct DepthFormatEnumHelper : public Apoc3D::Collections::EnumDualConversionHelper<DepthFormat>
		{
			DepthFormatEnumHelper();
		} static DepthFormatEnumConveter;

		void createSizeTable(int st[])
		{
			st[(int)FMT_Unknown] = -1;
            st[(int)FMT_A16B16G16R16] = 8;
            st[(int)FMT_A16B16G16R16F] = 8;
            st[(int)FMT_A1R5G5B5] = 2;
            st[(int)FMT_A2B10G10R10] = 4;
            st[(int)FMT_A2R10G10B10] = 4;
            st[(int)FMT_A32B32G32R32F] = 32;
            st[(int)FMT_A4L4] = 1;
            st[(int)FMT_A4R4G4B4] = 2;
            st[(int)FMT_A8B8G8R8] = 4;
            st[(int)FMT_A8L8] = 2;
            st[(int)FMT_A8R8G8B8] = 4;
            st[(int)FMT_Alpha8] = 1;
            st[(int)FMT_B4G4R4A4] = 2;
            st[(int)FMT_B5G6R5] = 2;
            st[(int)FMT_B8G8R8] = 3;
            st[(int)FMT_B8G8R8A8] = 4;
            st[(int)FMT_Depth] = -1;
            st[(int)FMT_DXT1] = -1;
            st[(int)FMT_DXT2] = -1;
            st[(int)FMT_DXT3] = -1;
            st[(int)FMT_DXT4] = -1;
            st[(int)FMT_DXT5] = -1;
            st[(int)FMT_G16R16] = 4;
            st[(int)FMT_G16R16F] = 4;
            st[(int)FMT_G32R32F] = 8;
            st[(int)FMT_Luminance16] = 2;
            st[(int)FMT_Luminance8] = 1;
            st[(int)FMT_R16F] = 1;
            st[(int)FMT_R16G16B16] = 6;
            st[(int)FMT_R32F] = 4;
            st[(int)FMT_R3G3B2] = 1;
            st[(int)FMT_R5G6B5] = 2;
            st[(int)FMT_R8G8B8] = 3;
            st[(int)FMT_R8G8B8A8] = 4;
            st[(int)FMT_X8B8G8R8] = 4;
            st[(int)FMT_X8R8G8B8] = 4;
            st[(int)FMT_Palette8] = 1;
            st[(int)FMT_Palette8Alpha8] = 2;
		}
		void createDepsizeTable(int st[])
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

		struct SizeTable
		{
			int sizeTable[FMT_Count];
			int depSizeTable[DEPFMT_Count];

			SizeTable()
			{
				createSizeTable(sizeTable);
				createDepsizeTable(depSizeTable);
			}

		} static FormatSizeTable;
		
		PixelFormat PixelFormatUtils::ConvertFormat(const String& fmt) { return PixelFormatEnumConverter.Parse(fmt); }
		String PixelFormatUtils::ToString(PixelFormat format) { return PixelFormatEnumConverter.ToString(format); }

		DepthFormat PixelFormatUtils::ConvertDepthFormat(const String& fmt) { return DepthFormatEnumConveter.Parse(fmt); }
		String PixelFormatUtils::ToString(DepthFormat format) { return DepthFormatEnumConveter.ToString(format); }

		void PixelFormatUtils::DumpPixelFormatName(Apoc3D::Collections::List<String>& names)
		{
			PixelFormatEnumConverter.DumpNames(names);
		}

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
			int bytepp = FormatSizeTable.depSizeTable[(int)format];
			if (bytepp == -1)
			{
				throw AP_EXCEPTION(EX_Default, L"Invalid pixel format");
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
			int bytepp = FormatSizeTable.sizeTable[(int)format];
			if (bytepp == -1)
			{
				throw AP_EXCEPTION(EX_Default, L"Invalid pixel format");
			}
			return width * height * depth * bytepp;
		}
		int PixelFormatUtils::GetBPP(PixelFormat fmt) { return FormatSizeTable.sizeTable[(int)fmt]; }
		int PixelFormatUtils::GetBPP(DepthFormat fmt) { return FormatSizeTable.depSizeTable[(int)fmt]; }
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


		PixelFormatEnumHelper::PixelFormatEnumHelper()
			: Apoc3D::Collections::EnumDualConversionHelper<PixelFormat>(FMT_Count)
		{
			AddPair(L"L8",			FMT_Luminance8);
			AddPair(L"L16",			FMT_Luminance16);
			AddPair(L"A8",			FMT_Alpha8);
			AddPair(L"A4L4",		FMT_A4L4);
			AddPair(L"A8L8",		FMT_A8L8);

			AddPair(L"R5G6B5",		FMT_R5G6B5);
			AddPair(L"R8G8B8",		FMT_R8G8B8);
			AddPair(L"A8R8G8B8",	FMT_A8R8G8B8);
			AddPair(L"A4R4G4B4",	FMT_A4R4G4B4);
			AddPair(L"A1R5G5B5",	FMT_A1R5G5B5);

			AddPair(L"B5G6R5",		FMT_B5G6R5);
			AddPair(L"B8G8R8",		FMT_B8G8R8);
			AddPair(L"B8G8R8A8",	FMT_B8G8R8A8);
			AddPair(L"A8B8G8R8",	FMT_A8B8G8R8);

			AddPair(L"A2R10G10B10",	FMT_A2R10G10B10);
			AddPair(L"A2B10G10R10", FMT_A2B10G10R10);

			AddPair(L"DXT1", FMT_DXT1);
			AddPair(L"DXT2", FMT_DXT2);
			AddPair(L"DXT3", FMT_DXT3);
			AddPair(L"DXT4", FMT_DXT4);
			AddPair(L"DXT5", FMT_DXT5);

			AddPair(L"R16F",	FMT_R16F);
			AddPair(L"G16R16F",	FMT_G16R16F);
			AddPair(L"A16B16G16R16F",	FMT_A16B16G16R16F);

			AddPair(L"R32F",	FMT_R32F);
			AddPair(L"G32R32F",	FMT_G32R32F);
			AddPair(L"A32B32G32R32F",	FMT_A32B32G32R32F);

			AddPair(L"X8R8G8B8", FMT_X8R8G8B8);
			AddPair(L"X8B8G8R8", FMT_X8B8G8R8);
			AddPair(L"X1R5G5B5", FMT_X1R5G5B5);

			AddPair(L"R8G8B8A8", FMT_R8G8B8A8);

			AddPair(L"A16B16G16R16",	FMT_A16B16G16R16);

			AddPair(L"R3G3B2",	FMT_R3G3B2);
			AddPair(L"B4G4R4A4",	FMT_B4G4R4A4);

			AddPair(L"G16R16",	FMT_G16R16);
			AddPair(L"R16G16B16",	FMT_R16G16B16);

			AddPair(L"P8",	FMT_Palette8);
			AddPair(L"P8A8",	FMT_Palette8Alpha8);

			AddPair(L"Unknown", FMT_Unknown);
		}

		DepthFormatEnumHelper::DepthFormatEnumHelper()
			: Apoc3D::Collections::EnumDualConversionHelper<DepthFormat>(DEPFMT_Count)
		{
			AddPair(L"D15S1",		DEPFMT_Depth15Stencil1);
			AddPair(L"D16",			DEPFMT_Depth16);
			AddPair(L"D16Lockable",	DEPFMT_Depth16Lockable);
			AddPair(L"D24",			DEPFMT_Depth24X8);
			AddPair(L"D24S4",		DEPFMT_Depth24Stencil4);
			AddPair(L"D24S8",		DEPFMT_Depth24Stencil8);
			AddPair(L"D24S8F",		DEPFMT_Depth24Stencil8Single);
			AddPair(L"D32",			DEPFMT_Depth32);
			AddPair(L"D32Lockable", DEPFMT_Depth32Lockable);
			AddPair(L"D32F",		DEPFMT_Depth32Single);
		}


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
		

		/*inline uint16 ch_r32u16(float v)
		{
			v = Math::Saturate(v);
			return static_cast<uint16>(v*65535);
		}*/
		inline uint16 ch_r32r16(float v)
		{
			return FloatToHalf(v);
		}
		inline float ch_r16r32(uint16 v)
		{
			return HalfToFloat(v);
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
			return ch_r32u8(HalfToFloat(v));
		}
		inline float ch_u8r32(byte v)
		{
			return v / 255.0f;
		}
		inline uint16 ch_u8r16(byte v)
		{
			return FloatToHalf(v / 255.0f);
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
			Apoc3D::Collections::FastMap<uint, ConversionRoutine> Converters;

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

		private:

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

		//////////////////////////////////////////////////////////////////////////


	}
}