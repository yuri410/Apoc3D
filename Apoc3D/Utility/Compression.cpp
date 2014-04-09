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

		
		int32 lz4Decompress(char* destBuffer, int32 outputSize, Apoc3D::IO::BufferedStreamReader* bsr)
		{
			if (bsr->getLength() == 0)
				return 0;

			if (outputSize == 0) 
				return 0;

			#define ML_BITS  4
			#define ML_MASK  ((1U<<ML_BITS)-1)
			#define RUN_BITS (8-ML_BITS)
			#define RUN_MASK ((1U<<RUN_BITS)-1)
			#define STEPSIZE sizeof(size_t)
			#define COPYLENGTH 8
			#define LASTLITERALS 5
			#define MINMATCH 4
			#define MFLIMIT (COPYLENGTH+MINMATCH)

			#if defined(__GNUC__)  && !defined(LZ4_FORCE_UNALIGNED_ACCESS)
			#  define _PACKED __attribute__ ((packed))
			#else
			#  define _PACKED
			#endif

				typedef struct { uint32 v; }  _PACKED U32_S;
				typedef struct {size_t v;} _PACKED size_t_S;

			#define A32(x)   (((U32_S *)(x))->v)
			#define AARCH(x) (((size_t_S *)(x))->v)

			#define LZ4_COPYSTEP(d,s)         { AARCH(d) = AARCH(s); d+=STEPSIZE; s+=STEPSIZE; }
			#define LZ4_COPY8(d,s)            { LZ4_COPYSTEP(d,s); if (STEPSIZE<8) LZ4_COPYSTEP(d,s); }

			#if LZ4_ARCH64 || !defined(__GNUC__)
			#  define LZ4_WILDCOPY(d,s,e)     { do { LZ4_COPY8(d,s) } while (d<e); }           /* at the end, d>=e; */
			#else
			#  define LZ4_WILDCOPY(d,s,e)     { if (likely(e-d <= 8)) LZ4_COPY8(d,s) else do { LZ4_COPY8(d,s) } while (d<e); }
			#endif
			#define LZ4_SECURECOPY(d,s,e)     { if (d<e) LZ4_WILDCOPY(d,s,e); }

			byte* dst = (byte*) destBuffer;
			byte* const dstEnd = dst + outputSize;
   
			char temp[2];

			const size_t dec32table[] = {4-0, 4-3, 4-2, 4-3, 4-0, 4-0, 4-0, 4-0};   /* static reduces speed for LZ4_decompress_safe() on GCC64 */
			static const size_t dec64table[] = {0, 0, 0, (size_t)-1, 0, 1, 2, 3};


			/* Main Loop */
			while (1)
			{
				uint32 token = 0;
				uint32 length;

				/* get runlength */
				bool rr =  bsr->ReadByte((char&)token);
				assert(rr);

				if ((length=(token>>ML_BITS)) == RUN_MASK)
				{
					uint32 s=255;
					while (s==255)
					{
						if (!bsr->ReadByte((char&)s))
							break;

						length += s;
					}
				}

				/* copy literals */
				int32 r = bsr->Read((char*)dst, length);
				
				if (dst + length > dstEnd - MFLIMIT || (r < (int32)length))
				{
					dst += r;
					break;
				}
				dst += r;

				/* get offset */
				r = bsr->Read(temp, 2);
				assert(r == 2);

				uint16 offset = reinterpret_cast<uint16&>(*temp);
				byte* ref = dst - offset;

				if (ref < (byte*)destBuffer)
					goto _output_error;


				/* get matchlength */
				if ((length=(token&ML_MASK)) == ML_MASK)
				{
					for (;;)
					{
						uint32 s = 0;
						rr = bsr->ReadByte((char&)s);
						assert(rr);
						//s = *sourceBuffer++;

						length += s;
						if (s==255) continue;
						break;
					}
				}

				/* copy repeated sequence */
				if (dst-ref<(int)STEPSIZE)
				{
					const size_t dec64 = dec64table[(sizeof(void*)==4) ? 0 : dst-ref];
					dst[0] = ref[0];
					dst[1] = ref[1];
					dst[2] = ref[2];
					dst[3] = ref[3];

					ref += dec32table[dst-ref];
					A32(dst+4) = A32(ref);
					dst += STEPSIZE; ref -= dec64;
				}
				else 
				{
					LZ4_COPYSTEP(dst,ref); 
				}

				byte* cpyEnd = dst + length - (STEPSIZE-4);
        
				if (cpyEnd > dstEnd-COPYLENGTH-(STEPSIZE-4))
				{
					if (cpyEnd > dstEnd-LASTLITERALS) 
						goto _output_error;    /* Error : last 5 bytes must be literals */
            
					LZ4_SECURECOPY(dst, ref, (dstEnd-COPYLENGTH));
					while(dst<cpyEnd) *dst++=*ref++;
					dst=cpyEnd;
					continue;
				}
				LZ4_WILDCOPY(dst, ref, cpyEnd);
				dst = cpyEnd;
			}

			/* end of decoding */
			return (char*)dst - destBuffer;

			/* Overflow error detected */
		_output_error:
			return -1;
		}



	}
}