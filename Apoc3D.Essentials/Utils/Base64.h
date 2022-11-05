#pragma once

#include "apoc3d/ApocCommon.h"
#include "apoc3d.Essentials/EssentialCommon.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Base64
	{
		std::string Encode(const ByteBuffer& data);
		std::string Encode(const MemoryOutStream& data);
		std::string Encode(const char* data, int64 dataSize);

		bool Decode(const std::string& ascdata, ByteBuffer& bindata);

		std::string ToHexString(const byte* src, int32 size, char sep = 0);
	}
}