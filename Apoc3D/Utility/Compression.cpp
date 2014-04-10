#include "Compression.h"
#include "../IOLib/Streams.h"
#include "../IOLib/IOUtils.h"

namespace Apoc3D
{
	namespace Utility
	{
		int32 rleDecompress(char* dstBuffer, int32 dstBufferSize, const char* srcBuffer, int32 srcBufferSize)
		{
#if _DEBUG
			int32 orginalDstBufferSize = dstBufferSize;
			char* orginalDstBuffer = dstBuffer;
#endif

			byte token;
			int32 length;
			int32 total = 0;

			while (dstBufferSize > 0 && srcBufferSize > 0)
			{
				token = *(const byte*)srcBuffer++;
				srcBufferSize--;

				length = (token & ~0x80) + 1;

				if (length > dstBufferSize)
					return total;

				if (token & 0x80)
				{
					if (srcBufferSize <= 0)
						return total;
					token = *(const byte*)srcBuffer++;
					srcBufferSize--;

					dstBufferSize -= length;
					total += length;

					while (length-->0)
						*dstBuffer++ = token;
					//memset(dstBuffer, token, length);
					//dstBuffer += length;
				}
				else
				{
					if (srcBufferSize < length)
						return total;

					srcBufferSize -= length;
					dstBufferSize -= length;
					total += length;

					while (length-->0)
					{
						*dstBuffer++ = *srcBuffer++;
					}

					//memcpy(dstBuffer, srcBuffer, length);
					//srcBuffer += length;
					//dstBuffer += length;
				}

				assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);
			}

			return total;
		}
		int32 rleCompress(char* dstBuffer, int32 dstBufferSize, const char* srcBuffer, int32 srcBufferSize)
		{
#if _DEBUG
			int32 orginalDstBufferSize = dstBufferSize;
			char* orginalDstBuffer = dstBuffer;
#endif

			byte token;
			int32 i;
			int32 total = 0;

			while (srcBufferSize > 0)
			{
				i = 2;
				while (i<srcBufferSize && i<128 && 
					srcBuffer[i] == srcBuffer[i-1] && srcBuffer[i-1] == srcBuffer[i-2])
					i++;

				if (i>2)
				{
					token = i - 1 | 0x80;

					if (dstBufferSize <= 0)
						return total;
					*(byte*)dstBuffer++ = token;
					dstBufferSize--;
					total++;

					assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);

					if (dstBufferSize <= 0)
						return total;
					*dstBuffer++ = *srcBuffer;
					dstBufferSize--;
					total++;

					assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);

					srcBuffer += i;
					srcBufferSize -= i;
				}

				i = 0;
				while (i < srcBufferSize && i < 128 && (i+2>srcBufferSize ? 1 : 
					srcBuffer[i] != srcBuffer[i+1] || srcBuffer[i+1] != srcBuffer[i+2]))
					i++;

				if (i)
				{
					token = i-1;

					if (dstBufferSize <= 0)
						return total;
					*(byte*)dstBuffer++ = token;
					dstBufferSize--;
					total++;

					assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);

					if (dstBufferSize < i)
						return total;
					memcpy(dstBuffer, srcBuffer, i);
					dstBuffer += i;
					dstBufferSize -= i;
					total += i;

					assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);


					srcBuffer += i;
					srcBufferSize -= i;
				}
			}
			return total;
		}

		int32 rleDecompress(char* dstBuffer, int32 dstBufferSize, Apoc3D::IO::BufferedStreamReader* srcStrm)
		{
#if _DEBUG
			int32 orginalDstBufferSize = dstBufferSize;
			char* orginalDstBuffer = dstBuffer;
#endif
			byte token;
			int32 length;
			int32 total = 0;

			while (dstBufferSize > 0 && srcStrm->ReadByte((char&)token))
			{
				length = (token & ~0x80) + 1;

				if (length > dstBufferSize)
					return total;

				if (token & 0x80)
				{
					if (!srcStrm->ReadByte((char&)token))
						return total;

					dstBufferSize -= length;
					total += length;

					//memset(dstBuffer, token, length);
					while (length-->0)
						*dstBuffer++ = token;
				}
				else
				{
					if (srcStrm->Read(dstBuffer, length) != length)
						return total;

					dstBuffer += length;
					dstBufferSize -= length;
					total += length;
				}

				assert(dstBuffer - orginalDstBuffer <= orginalDstBufferSize);
			}

			return total;
		}
		int32 rleCompress(const char* srcBuffer, int32 srcBufferSize, Apoc3D::IO::Stream* dstStrm)
		{
			byte token;
			int32 i;
			int32 total = 0;

			while (srcBufferSize > 0)
			{
				i = 2;
				while (i<srcBufferSize && i<128 && 
					srcBuffer[i] == srcBuffer[i-1] && srcBuffer[i-1] == srcBuffer[i-2])
					i++;

				if (i>2)
				{
					token = i - 1 | 0x80;

					dstStrm->WriteByte(token);
					dstStrm->Write(srcBuffer, 1);
					total += 2;

					srcBuffer += i;
					srcBufferSize -= i;
				}

				i = 0;
				while (i < srcBufferSize && i < 128 && (i+2>srcBufferSize ? 1 : 
					srcBuffer[i] != srcBuffer[i+1] || srcBuffer[i+1] != srcBuffer[i+2]))
					i++;

				if (i)
				{
					token = i-1;

					dstStrm->WriteByte(token);
					dstStrm->Write(srcBuffer, i);
					total += 1 + i;

					srcBuffer += i;
					srcBufferSize -= i;
				}
			}
			return total;
		}

		int32 rleEvalDecompressedSize(const char* srcBuffer, int32 srcBufferSize)
		{
			byte token;
			int32 length;
			int32 total = 0;

			while (srcBufferSize > 0)
			{
				token = *(const byte*)srcBuffer++;
				srcBufferSize--;

				length = (token & ~0x80) + 1;

				if (token & 0x80)
				{
					if (srcBufferSize <= 0)
						return total;
					//token = *(const byte*)srcBuffer++;
					srcBuffer++;
					srcBufferSize--;
				}
				else
				{
					if (srcBufferSize < length)
						return total;

					srcBuffer += length;
					srcBufferSize -= length;
				}

				total += length;
			}

			return total;
		}
		int32 rleEvalCompressedSize(const char* srcBuffer, int32 srcBufferSize)
		{
			int32 i;
			int32 total = 0;

			while (srcBufferSize>0)
			{
				i = 2;
				while (i<srcBufferSize && i<128 && 
					srcBuffer[i] == srcBuffer[i-1] && srcBuffer[i-1] == srcBuffer[i-2])
					i++;

				if (i>2)
				{
					srcBuffer += i;
					srcBufferSize -= i;
					total += 2;
				}

				i = 0;
				while (i < srcBufferSize && i < 128 && (i+2>srcBufferSize ? 1 : 
					srcBuffer[i] != srcBuffer[i+1] || srcBuffer[i+1] != srcBuffer[i+2]))
					i++;

				if (i)
				{
					srcBuffer += i;
					srcBufferSize -= i;
					total += 1 + i;
				}
			}
			return total;
		}



	}
}