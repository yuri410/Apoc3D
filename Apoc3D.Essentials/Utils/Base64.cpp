#include "Base64.h"

#include "apoc3D/IOLib/Streams.h"

namespace Apoc3D
{
	namespace Base64
	{
		static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		static const char reverse_table[128] =
		{
		   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
		   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
		   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
		   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
		};

		std::string Encode(const char* data, int64 dataSize)
		{
			// Use = signs so the end is properly padded.
			std::string retval((((dataSize + 2) / 3) * 4), '=');
			size_t outpos = 0;
			int bits_collected = 0;
			unsigned int accumulator = 0;

			for (int64 i = 0; i < dataSize; i++)
			{
				byte b = (byte)data[i];
				accumulator = (accumulator << 8) | ((byte)b & 0xffu);
				bits_collected += 8;
				while (bits_collected >= 6)
				{
					bits_collected -= 6;
					retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
				}
			}

			if (bits_collected > 0)
			{
				// Any trailing bits that are missing.
				assert(bits_collected < 6);
				accumulator <<= 6 - bits_collected;
				retval[outpos++] = b64_table[accumulator & 0x3fu];
			}
			assert(outpos >= (retval.size() - 2));
			assert(outpos <= retval.size());
			return retval;
		}

		std::string Encode(const MemoryOutStream& data)
		{
			return Encode(data.getDataPointer(), data.getLength());
		}
		
		std::string Encode(const ByteBuffer& bindata)
		{
			return Encode(bindata.c_str(), bindata.size());
		}

		bool Decode(const std::string& ascdata, ByteBuffer& bindata)
		{
			int bits_collected = 0;
			unsigned int accumulator = 0;

			for (const char& c : ascdata)
			{
				if (isspace(c) || c == '=')
				{
					// Skip whitespace and padding. Be liberal in what you accept.
					continue;
				}
				if ((c > 127) || (c < 0) || (reverse_table[c] > 63))
				{
					return false;
				}

				accumulator = (accumulator << 6) | reverse_table[c];
				bits_collected += 6;
				if (bits_collected >= 8)
				{
					bits_collected -= 8;
					bindata += static_cast<char>((accumulator >> bits_collected) & 0xffu);
				}
			}
			return true;
		}

		std::string ToHexString(const byte* src, int32 size, char sep)
		{
			const char digits[] = "0123456789ABCDEF";
			const int memlen = size;

			std::string r;
			r.reserve(sep ? (memlen * 3) : (memlen * 2));

			for (int i = 0; i < memlen; i++)
			{
				if (i > 0)
					r.push_back(sep);

				r.push_back(digits[(src[i] >> 4) & 0xF]);
				r.push_back(digits[src[i] & 0xF]);
			}
			return r;
		}

	}
}
