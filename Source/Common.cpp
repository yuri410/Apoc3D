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