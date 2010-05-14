
#include "Common.h"


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