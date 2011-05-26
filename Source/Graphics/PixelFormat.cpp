
#include "PixelFormat.h"

namespace Apoc3D
{
	namespace Graphics
	{
		static void createSizeTable(int st[])
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
		static void createDepsizeTable(int st[])
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

		int PixelFormatUtils::sizeTable[FMT_Count];
		int PixelFormatUtils::depSizeTable[DEPFMT_Count];
		auto_ptr<PixelFormatUtils> PixelFormatUtils::initializer = auto_ptr<PixelFormatUtils>(new PixelFormatUtils());

		PixelFormatUtils::PixelFormatUtils()
		{
			createSizeTable(sizeTable);
			createDepsizeTable(depSizeTable);
		}

	
	}
}