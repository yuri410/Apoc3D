#pragma once

#include "apoc3d/ApocCommon.h"
#include "apoc3d.Essentials/EssentialCommon.h"

namespace Apoc3D
{
	namespace Utility
	{
		bool Compress(const char* src, int32 srcSize, char*& dst, int32& dstSize);
		bool Decompress(const char* src, int32 srcSize, char*& dst, int32& dstSize);

		bool Compress(const char* src, int32 srcSize, ByteBuffer& dst);
		bool Decompress(const char* src, int32 srcSize, ByteBuffer& dst);

		bool Compress(const ByteBuffer& src, ByteBuffer& dst);
		bool Decompress(const ByteBuffer& src, ByteBuffer& dst);
	}
}