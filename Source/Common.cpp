/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#include "Common.h"

inline int32 ci32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int32*>(src);
#else
	return (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
#endif
}
inline int16 ci16_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int16*>(src);
#else
	return (src[0] << 8) | src[1];
#endif
}
inline int64 ci64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int64*>(src);
#else
	return (src[0] << 56) | (src[1] << 48) | (src[2] << 40) | (src[3] << 32) | 
		(src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7];
#endif
}
inline uint32 cui32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint32*>(src);
#else
	return (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
#endif
}
inline uint16 cui16_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint16*>(src);
#else
	return (src[0] << 8) | src[1];
#endif
}
inline uint64 cui64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint64*>(src);
#else
	return (src[0] << 56) | (src[1] << 48) | (src[2] << 40) | (src[3] << 32) | 
		(src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7];
#endif
}
inline const float cr32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const float*>(src);
#else
	return reinterpret_cast<float&>((src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3]);		
#endif
}
inline const double cr64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const double*>(src);
#else
	return reinterpret_cast<double&>(
		(src[0] << 56) | (src[1] << 48) | (src[2] << 40) | (src[3] << 32) | 
		(src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7]);
#endif
}

inline int32 cint32_le(const char* const src)
{
	return *reinterpret_cast<const int32*>(src);
}
inline int16 cint16_le(const char* const src)
{
	return *reinterpret_cast<const int16*>(src);
}
inline int64 cint64_le(const char* const src)
{
	return *reinterpret_cast<const int64*>(src);
}
inline uint32 cuint32_le(const char* const src)
{
	return *reinterpret_cast<const uint32*>(src);
}
inline uint16 cuint16_le(const char* const src)
{
	return *reinterpret_cast<const uint16*>(src);
}
inline uint64 cuint64_le(const char* const src)
{
	return *reinterpret_cast<const uint64*>(src);
}
inline const float cr32_le(const char* const src)
{
	return *reinterpret_cast<const float*>(src);
}
inline const double cr64_le(const char* const src)
{
	return *reinterpret_cast<const double*>(src);
}

inline void i16tomb_le(int16 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int16&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 8);
	dest[1] = 0xff & v;
#endif
}
inline void i32tomb_le(int32 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int32&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 24);
	dest[1] = 0xff & (v >> 16);
	dest[2] = 0xff & (v >> 8);
	dest[3] = 0xff & (v);
#endif
}
inline void i64tomb_le(int64 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int64&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 56);
	dest[1] = 0xff & (v >> 48);
	dest[2] = 0xff & (v >> 40);
	dest[3] = 0xff & (v >> 32);
	dest[4] = 0xff & (v >> 24);
	dest[5] = 0xff & (v >> 16);
	dest[6] = 0xff & (v >> 8);
	dest[7] = 0xff & (v);
#endif
}
inline void ui16tomb_le(uint16 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint16&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 8);
	dest[1] = 0xff & v;
#endif
}
inline void ui32tomb_le(uint32 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint32&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 24);
	dest[1] = 0xff & (v >> 16);
	dest[2] = 0xff & (v >> 8);
	dest[3] = 0xff & (v);
#endif
}
inline void ui64tomb_le(uint64 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint64&>(dest[0]) = v;
#else
	dest[0] = 0xff & (v >> 56);
	dest[1] = 0xff & (v >> 48);
	dest[2] = 0xff & (v >> 40);
	dest[3] = 0xff & (v >> 32);
	dest[4] = 0xff & (v >> 24);
	dest[5] = 0xff & (v >> 16);
	dest[6] = 0xff & (v >> 8);
	dest[7] = 0xff & (v);
#endif
}
inline void r32tomb_le(float v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<float&>(dest[0]) = v;
#else
	uint32 r = reinterpret_cast<const uint32&>(v);
	dest[0] = 0xff & (v >> 24);
	dest[1] = 0xff & (v >> 16);
	dest[2] = 0xff & (v >> 8);
	dest[3] = 0xff & (v);
#endif
}
inline void r64tomb_le(double v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<double&>(dest[0]) = v;
#else
	uint64 r = reinterpret_cast<const uint64&>(v);
	dest[0] = 0xff & (v >> 56);
	dest[1] = 0xff & (v >> 48);
	dest[2] = 0xff & (v >> 40);
	dest[3] = 0xff & (v >> 32);
	dest[4] = 0xff & (v >> 24);
	dest[5] = 0xff & (v >> 16);
	dest[6] = 0xff & (v >> 8);
	dest[7] = 0xff & (v);
#endif
}

inline void i16tomb_mem(int16 v, char* dest)
{
	reinterpret_cast<int16&>(dest[0]) = v;
}
inline void i32tomb_mem(int32 v, char* dest)
{
	reinterpret_cast<int32&>(dest[0]) = v;
}
inline void i64tomb_mem(int64 v, char* dest)
{
	reinterpret_cast<int64&>(dest[0]) = v;
}
inline void ui16tomb_mem(uint16 v, char* dest)
{
	reinterpret_cast<uint16&>(dest[0]) = v;
}
inline void ui32tomb_mem(uint32 v, char* dest)
{
	reinterpret_cast<uint32&>(dest[0]) = v;
}
inline void ui64tomb_mem(uint64 v, char* dest)
{
	reinterpret_cast<uint64&>(dest[0]) = v;
}
inline void r32tomb_mem(float v, char* dest)
{
	reinterpret_cast<float&>(dest[0]) = v;
}
inline void r64tomb_mem(double v, char* dest)
{
	reinterpret_cast<double&>(dest[0]) = v;
}



void* memcpy_sse( char* pDest, const char* pSrc, size_t nBytes )
{
	assert( nBytes >= (15 + 64) );
	void* pDestOrg = pDest;

	uint nAlignDest = (16 - (uintptr_t)pDest) & 15;
	memcpy( pDest, pSrc, nAlignDest );
	pDest += nAlignDest;
	pSrc  += nAlignDest;
	nBytes -= nAlignDest;

	uint nLoops = nBytes >> 6; // no. of loops to copy 64 bytes
	nBytes -= nLoops << 6;
	if( ((uintptr_t)pSrc & 15) == 0 )
	{
		for( int i = nLoops; i > 0; --i )
		{
			__m128 tmp0 = _mm_load_ps( (float*)(pSrc + 0 ) );
			__m128 tmp1 = _mm_load_ps( (float*)(pSrc + 16) );
			__m128 tmp2 = _mm_load_ps( (float*)(pSrc + 32) );
			__m128 tmp3 = _mm_load_ps( (float*)(pSrc + 48) );
			_mm_store_ps( (float*)(pDest + 0 ), tmp0 );
			_mm_store_ps( (float*)(pDest + 16), tmp1 );
			_mm_store_ps( (float*)(pDest + 32), tmp2 );
			_mm_store_ps( (float*)(pDest + 48), tmp3 );
			pSrc  += 64;
			pDest += 64;
		}
	}
	else
	{
		for( int i = nLoops; i > 0; --i )
		{
			__m128 tmp0 = _mm_loadu_ps( (float*)(pSrc + 0 ) );
			__m128 tmp1 = _mm_loadu_ps( (float*)(pSrc + 16) );
			__m128 tmp2 = _mm_loadu_ps( (float*)(pSrc + 32) );
			__m128 tmp3 = _mm_loadu_ps( (float*)(pSrc + 48) );
			_mm_store_ps( (float*)(pDest + 0 ), tmp0 );
			_mm_store_ps( (float*)(pDest + 16), tmp1 );
			_mm_store_ps( (float*)(pDest + 32), tmp2 );
			_mm_store_ps( (float*)(pDest + 48), tmp3 );
			pSrc  += 64;
			pDest += 64;
		}
	}
	memcpy( pDest, pSrc, nBytes );
	return pDestOrg;
}

//void memcpyf(void* src, void* dst, uint s)
//{
//	memcpy(src,dst,s);
//	_asm
//	{
//		test s, 4
//		jnz B
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		rep movsd
//		ret
//B:
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		rep movsb
//	}
//}
//
//int memcmpf(void* src, void* dst, uint s)
//{
//	_asm
//	{
//		mov eax, 1
//		test s, 4
//		jnz B
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		repz cmpsd
//		jnz END
//		ret
//B:
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		repz cmpsb
//		jnz END
//		ret
//END:
//		mov eax, 0
//		ret
//	}
//}
//
//void memsetf(void* dst, int value, uint count)
//{
//	_asm
//	{
//		test s, 4
//		jnz B
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		rep movsd
//		ret
//B:
//		mov esi, src
//		mov edi, dst
//		mov ecx, s
//		cld
//		rep movsb
//	}
//}