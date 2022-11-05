#include "Compression.h"
#include "apoc3d/Library/lz4hc.h"
#include "apoc3d/Library/lz4.h"
#include "apoc3d/IOLib/IOUtils.h"

namespace Apoc3D
{
	namespace Utility
	{

		bool Compress(const char* src, int32 srcSize, char*& dst, int32& dstSize)
		{
			dst = nullptr;
			dstSize = 0;

			char* compressedData = new char[LZ4_compressBound(srcSize) + sizeof(int32)];
			memcpy(compressedData, &srcSize, sizeof(int32));

			int32 compressedSize = LZ4_compressHC2(src, compressedData + sizeof(int32), srcSize, 16);
			if (compressedSize)
			{
				dst = compressedData;
				dstSize = compressedSize + sizeof(int32);

				return true;
			}
			delete[] compressedData;
			return false;
		}

		bool Decompress(const char* src, int32 srcSize, char*& dst, int32& dstSize)
		{
			dst = nullptr;
			dstSize = 0;

			int32 decompressedSize;
			memcpy(&decompressedSize, src, sizeof(int32));

			char* decompressedData = new char[decompressedSize];
			if (LZ4_decompress_safe(src + sizeof(int32), decompressedData, srcSize - sizeof(int32), decompressedSize) == decompressedSize)
			{
				dst = decompressedData;
				dstSize = decompressedSize;

				return true;
			}
			delete[] decompressedData;
			return false;
		}

		bool Compress(const char* src, int32 srcSize, ByteBuffer& dst)
		{
			dst.resize(LZ4_compressBound(srcSize) + sizeof(int32));

			char* compressedData = &dst[0];
			i32_mb_le(srcSize, compressedData);

			int32 compressedSize = LZ4_compressHC2(src, compressedData + sizeof(int32), srcSize, 16);
			if (compressedSize)
			{
				dst.resize(compressedSize + sizeof(int32));

				return true;
			}
			return false;
		}

		bool Decompress(const char* src, int32 srcSize, ByteBuffer& dst)
		{
			int32 decompressedSize = mb_i32_le(src);
			dst.resize(decompressedSize);

			char* decompressedData = &dst[0];
			int ret = LZ4_decompress_safe(src + sizeof(int32), decompressedData, srcSize - sizeof(int32), decompressedSize);
			if (ret == decompressedSize)
			{
				return true;
			}
			return false;
		}

		bool Compress(const ByteBuffer& src, ByteBuffer& dst)
		{
			return Compress(src.c_str(), (int32)src.size(), dst);
		}

		bool Decompress(const ByteBuffer& src, ByteBuffer& dst)
		{
			return Decompress(src.c_str(), (int32)src.size(), dst);
		}

	}
}