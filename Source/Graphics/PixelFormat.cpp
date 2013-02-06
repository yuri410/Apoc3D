
#include "PixelFormat.h"
#include "LockData.h"
#include "Math/Half.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
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


		PixelFormatUtils PixelFormatUtils::initializer;

		PixelFormatUtils::PixelFormatUtils()
		{
			createSizeTable(sizeTable);
			createDepsizeTable(depSizeTable);
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




#define FMTCONVERTERID(from,to) (((from)<<16)|(to))

		template <class U> struct DataBoxConverter 
		{
			static const int ID = U::ID;
			static void conversion(const DataBox& src, const DataBox& dst)
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
		};


		template <typename T, typename U, int id> struct PixelConverter {
			static const uint ID = id;
			typedef T SrcType;
			typedef U DstType;    

		};


		/** Type for PF_R8G8B8/PF_B8G8R8 */
		struct Col3b {
			Col3b(uint a, uint b, uint c)
				: x((byte)a), y((byte)b), z((byte)c) { }
			byte x,y,z;
		};
		/** Type for PF_FLOAT32_RGB */
		struct Col3f {
			Col3f(float r, float g, float b)
				: r(r), g(g), b(b) { }
			float r,g,b;
		};
		/** Type for PF_FLOAT32_RGBA */
		struct Col4f {
			Col4f(float r, float g, float b, float a)
				: r(r), g(g), b(b), a(a) { }
			float r,g,b,a;
		};

		struct A8R8G8B8toA8B8G8R8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8R8G8B8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
			}
		};

		struct A8R8G8B8toB8G8R8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8R8G8B8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};
		struct A8R8G8B8toR8G8B8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8R8G8B8, FMT_R8G8B8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
			}
		};

		struct A8B8G8R8toA8R8G8B8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8B8G8R8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
			}
		};

		struct A8B8G8R8toB8G8R8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8B8G8R8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
			}
		};

		struct A8B8G8R8toR8G8B8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_A8B8G8R8, FMT_R8G8B8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};


		struct B8G8R8A8toA8R8G8B8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_B8G8R8A8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};

		struct B8G8R8A8toA8B8G8R8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_B8G8R8A8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
			}
		};

		struct B8G8R8A8toR8G8B8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_B8G8R8A8, FMT_R8G8B8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
			}
		};

		struct R8G8B8A8toA8R8G8B8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_R8G8B8A8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
			}
		};


		struct R8G8B8A8toA8B8G8R8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_R8G8B8A8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
			}
		};

		struct R8G8B8A8toB8G8R8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_R8G8B8A8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
			}
		};


		struct A8B8G8R8toL8
			: public PixelConverter<uint, byte, FMTCONVERTERID(FMT_A8B8G8R8, FMT_Luminance8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return (byte)(inp&0x000000FF);
			}
		};

		struct L8toA8B8G8R8
			: public PixelConverter<byte, uint, FMTCONVERTERID(FMT_Luminance8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
			}
		};

		struct A8R8G8B8toL8
			: public PixelConverter<uint, byte, FMTCONVERTERID(FMT_A8R8G8B8, FMT_Luminance8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return (byte)((inp&0x00FF0000)>>16);
			}
		};

		struct L8toA8R8G8B8
			: public PixelConverter<byte, uint, FMTCONVERTERID(FMT_Luminance8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
			}
		};


		struct B8G8R8A8toL8
			: public PixelConverter<uint, byte, FMTCONVERTERID(FMT_B8G8R8A8, FMT_Luminance8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return (byte)((inp&0x0000FF00)>>8);
			}
		};

		struct L8toB8G8R8A8
			: public PixelConverter<byte, uint, FMTCONVERTERID(FMT_Luminance8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return 0x000000FF|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16)|(((unsigned int)inp)<<24);
			}
		};


		struct L8toL16
			: public PixelConverter<byte, uint16, FMTCONVERTERID(FMT_Luminance8, FMT_Luminance16)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return (uint16)((((unsigned int)inp)<<8)|(((unsigned int)inp)));
			}
		};

		struct L16toL8
			: public PixelConverter<uint16, byte, FMTCONVERTERID(FMT_Luminance16, FMT_Luminance8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return (byte)(inp>>8);
			}
		};


		struct R8G8B8toB8G8R8
			: public PixelConverter<Col3b, Col3b, FMTCONVERTERID(FMT_R8G8B8, FMT_B8G8R8)>
		{
			inline static DstType ConvertPixel(const SrcType &inp)
			{
				return Col3b(inp.z, inp.y, inp.x);
			}  
		};

		struct B8G8R8toR8G8B8
			: public PixelConverter<Col3b, Col3b, FMTCONVERTERID(FMT_B8G8R8, FMT_R8G8B8)>
		{
			inline static DstType ConvertPixel(const SrcType &inp)
			{
				return Col3b(inp.z, inp.y, inp.x);
			}  
		};


		// X8Y8Z8 ->  X8<<xshift Y8<<yshift Z8<<zshift A8<<ashift
		template <int id, unsigned int xshift, unsigned int yshift, unsigned int zshift, unsigned int ashift>
		struct Col3btoUint32swizzler
			: public PixelConverter<Col3b, uint, id>
		{
			inline static uint ConvertPixel(const Col3b &inp)
			{
#if LITTLE_ENDIAN
				return (0xFF<<ashift) | (((unsigned int)inp.x)<<zshift) | (((unsigned int)inp.y)<<yshift) | (((unsigned int)inp.z)<<xshift);
#else
				return (0xFF<<ashift) | (((unsigned int)inp.x)<<xshift) | (((unsigned int)inp.y)<<yshift) | (((unsigned int)inp.z)<<zshift);
#endif
			}
		};

		struct R8G8B8toX8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_R8G8B8, FMT_X8R8G8B8), 16, 8, 0, 24> { };
		struct R8G8B8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_R8G8B8, FMT_A8R8G8B8), 16, 8, 0, 24> { };
		struct B8G8R8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_B8G8R8, FMT_A8R8G8B8), 0, 8, 16, 24> { };
		struct R8G8B8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_R8G8B8, FMT_A8B8G8R8), 0, 8, 16, 24> { };
		struct B8G8R8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_B8G8R8, FMT_A8B8G8R8), 16, 8, 0, 24> { };
		struct R8G8B8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_R8G8B8, FMT_B8G8R8A8), 8, 16, 24, 0> { };
		struct B8G8R8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(FMT_B8G8R8, FMT_B8G8R8A8), 24, 16, 8, 0> { };
		

		struct A8R8G8B8toR8G8B8
			: public PixelConverter<uint, Col3b, FMTCONVERTERID(FMT_A8R8G8B8, FMT_R8G8B8)>
		{
			inline static DstType ConvertPixel(uint inp)
			{
				return Col3b((byte)((inp>>16)&0xFF), (byte)((inp>>8)&0xFF), (byte)((inp>>0)&0xFF));
			}
		};
		struct A8R8G8B8toB8G8R8
			: public PixelConverter<uint, Col3b, FMTCONVERTERID(FMT_A8R8G8B8, FMT_B8G8R8)>
		{
			inline static DstType ConvertPixel(uint inp)
			{
				return Col3b((byte)((inp>>0)&0xFF), (byte)((inp>>8)&0xFF), (byte)((inp>>16)&0xFF));
			}
		};
		struct X8R8G8B8toA8R8G8B8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8R8G8B8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return inp | 0xFF000000;
			}
		};

		struct X8R8G8B8toA8B8G8R8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8R8G8B8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
			}
		};
		struct X8R8G8B8toB8G8R8A8
			: public PixelConverter <uint, uint, FMTCONVERTERID(FMT_X8R8G8B8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
			}
		};
		struct X8R8G8B8toR8G8B8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8R8G8B8, FMT_R8G8B8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0xFFFFFF)<<8)|0x000000FF;
			}
		};


		// X8B8G8R8
		struct X8B8G8R8toA8R8G8B8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8B8G8R8, FMT_A8R8G8B8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
			}
		};
		struct X8B8G8R8toA8B8G8R8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8B8G8R8, FMT_A8B8G8R8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return inp | 0xFF000000;
			}
		};
		struct X8B8G8R8toB8G8R8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8B8G8R8, FMT_B8G8R8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0xFFFFFF)<<8)|0x000000FF;
			}
		};
		struct X8B8G8R8toR8G8B8A8
			: public PixelConverter<uint, uint, FMTCONVERTERID(FMT_X8B8G8R8, FMT_R8G8B8A8)>
		{
			inline static DstType ConvertPixel(SrcType inp)
			{
				return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
			}
		};
		
#define CASECONVERTER(type) case type::ID : DataBoxConverter<type>::conversion(src, dst); return 1;

		int PixelFormatUtils::ConvertPixels(const DataBox& src, const DataBox& dst)
		{;
		switch(FMTCONVERTERID((uint)src.getFormat(), (uint)dst.getFormat()))
		{
			// Register converters here
			CASECONVERTER(A8R8G8B8toA8B8G8R8);
			CASECONVERTER(A8R8G8B8toB8G8R8A8);
			CASECONVERTER(A8R8G8B8toR8G8B8A8);
			CASECONVERTER(A8B8G8R8toA8R8G8B8);
			CASECONVERTER(A8B8G8R8toB8G8R8A8);
			CASECONVERTER(A8B8G8R8toR8G8B8A8);
			CASECONVERTER(B8G8R8A8toA8R8G8B8);
			CASECONVERTER(B8G8R8A8toA8B8G8R8);
			CASECONVERTER(B8G8R8A8toR8G8B8A8);
			CASECONVERTER(R8G8B8A8toA8R8G8B8);
			CASECONVERTER(R8G8B8A8toA8B8G8R8);
			CASECONVERTER(R8G8B8A8toB8G8R8A8);
			CASECONVERTER(A8B8G8R8toL8);
			CASECONVERTER(L8toA8B8G8R8);
			CASECONVERTER(A8R8G8B8toL8);
			CASECONVERTER(L8toA8R8G8B8);
			CASECONVERTER(B8G8R8A8toL8);
			CASECONVERTER(L8toB8G8R8A8);
			CASECONVERTER(L8toL16);
			CASECONVERTER(L16toL8);
			CASECONVERTER(B8G8R8toR8G8B8);
			CASECONVERTER(R8G8B8toB8G8R8);
			CASECONVERTER(R8G8B8toX8R8G8B8);
			CASECONVERTER(R8G8B8toA8R8G8B8);
			CASECONVERTER(B8G8R8toA8R8G8B8);
			CASECONVERTER(R8G8B8toA8B8G8R8);
			CASECONVERTER(B8G8R8toA8B8G8R8);
			CASECONVERTER(R8G8B8toB8G8R8A8);
			CASECONVERTER(B8G8R8toB8G8R8A8);
			CASECONVERTER(A8R8G8B8toR8G8B8);
			CASECONVERTER(A8R8G8B8toB8G8R8);
			CASECONVERTER(X8R8G8B8toA8R8G8B8);
			CASECONVERTER(X8R8G8B8toA8B8G8R8);
			CASECONVERTER(X8R8G8B8toB8G8R8A8);
			CASECONVERTER(X8R8G8B8toR8G8B8A8);
			CASECONVERTER(X8B8G8R8toA8R8G8B8);
			CASECONVERTER(X8B8G8R8toA8B8G8R8);
			CASECONVERTER(X8B8G8R8toB8G8R8A8);
			CASECONVERTER(X8B8G8R8toR8G8B8A8);

		default:
			return 0;
		}
		}
#undef CASECONVERTER

		inline uint16 floatToU16Channel(float v)
		{
			if (v<0)
				v = 0;
			if (v>1)
				v=1;
			return static_cast<uint16>(v*65535);
		}
		inline byte floatToU8Channel(float v)
		{
			if (v<0)
				v = 0;
			if (v>1)
				v=1;
			return static_cast<byte>(v*255);
		}
		void packColour(const float r, const float g, const float b, const float a, 
			const PixelFormat pf,  void* dest)
		{
			switch(pf)
			{
			case FMT_R32F:
				((float*)dest)[0] = r;
				break;
			case FMT_G32R32F:
				((float*)dest)[0] = g;
				((float*)dest)[1] = r;
				break;
			case FMT_A32B32G32R32F:
				((float*)dest)[0] = a;
				((float*)dest)[1] = b;
				((float*)dest)[2] = g;
				((float*)dest)[3] = r;
				break;
			case FMT_R16F:
				((uint16*)dest)[0] = FloatToHalf(r);
				break;
			case FMT_G16R16F:
				((uint16*)dest)[0] = FloatToHalf(g);
				((uint16*)dest)[1] = FloatToHalf(r);
				break;
			case FMT_A16B16G16R16F:
				((uint16*)dest)[0] = FloatToHalf(a);
				((uint16*)dest)[1] = FloatToHalf(b);
				((uint16*)dest)[2] = FloatToHalf(g);
				((uint16*)dest)[3] = FloatToHalf(r);
				break;
			case FMT_R16G16B16:
				((uint16*)dest)[0] = floatToU16Channel(r);
				((uint16*)dest)[1] = floatToU16Channel(g);
				((uint16*)dest)[2] = floatToU16Channel(b);
				break;
			case FMT_A16B16G16R16:
				((uint16*)dest)[0] = floatToU16Channel(a);
				((uint16*)dest)[1] = floatToU16Channel(b);
				((uint16*)dest)[2] = floatToU16Channel(g);
				((uint16*)dest)[3] = floatToU16Channel(r);
				break;
			case FMT_A8L8:
				((byte*)dest)[0] = floatToU8Channel(r);
				((byte*)dest)[1] = floatToU8Channel(a);
				break;
			default:
				throw Apoc3DException::createException(EX_NotSupported,L"");
				break;
			}
		}
		void unpackColour(float *r, float *g, float *b, float *a,
			PixelFormat pf,  const void* src)
		{
			switch(pf)
			{
			case FMT_R32F:
				*r = *g = *b = ((float*)src)[0];
				*a = 1.0f;
				break;
			case FMT_G32R32F:
				*g = ((float*)src)[0];
				*r = *b = ((float*)src)[1];
				*a = 1.0f;
				break;
			case FMT_A32B32G32R32F:
				*a = ((float*)src)[0];
				*b = ((float*)src)[1];
				*g = ((float*)src)[2];
				*r = ((float*)src)[3];

				break;
			case FMT_R16F:
				*r = *g = *b = HalfToFloat(reinterpret_cast<const uint16*>(src)[0]);
				*a = 1.0f;
				break;
			case FMT_G16R16F:
				*g =  HalfToFloat(reinterpret_cast<const uint16*>(src)[0]); 
				*r = *b = HalfToFloat(reinterpret_cast<const uint16*>(src)[1]);
				*a = 1.0f;
				break;
			case FMT_A16B16G16R16F:
				*a = HalfToFloat(reinterpret_cast<const uint16*>(src)[0]);
				*g = HalfToFloat(reinterpret_cast<const uint16*>(src)[1]);
				*g = HalfToFloat(reinterpret_cast<const uint16*>(src)[2]);
				*r = HalfToFloat(reinterpret_cast<const uint16*>(src)[3]);

				break;
			case FMT_R16G16B16:
				*r = reinterpret_cast<const uint16*>(src)[0] / (65535.0f);
				*g = reinterpret_cast<const uint16*>(src)[1] / (65535.0f);
				*b = reinterpret_cast<const uint16*>(src)[2] / (65535.0f);
				*a = 1.0f;
				break;
			case FMT_A16B16G16R16:
				*a = reinterpret_cast<const uint16*>(src)[0] / (65535.0f);
				*b = reinterpret_cast<const uint16*>(src)[1] / (65535.0f);
				*g = reinterpret_cast<const uint16*>(src)[2] / (65535.0f);
				*r = reinterpret_cast<const uint16*>(src)[3] / (65535.0f);

				break;
			case FMT_A8L8:
				*r = *g = *b = reinterpret_cast<const uint16*>(src)[0] / (255.0f);
				*a =  reinterpret_cast<const uint16*>(src)[1] / (255.0f);
				break;
			default:
				throw Apoc3DException::createException(EX_NotSupported,L"");
				break;
			}
		}

		void PixelFormatUtils::Resize(const float* src, int srcWidth, int srcHeight, float* dst, int dstWidth, int dstHeight, int numChannels)
		{

		}
	}
}