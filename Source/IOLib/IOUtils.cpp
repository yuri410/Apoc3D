#include "Common.h"


int32 ci32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int32*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	uint32 r = (src2[0] << 24) | (src2[1] << 16) | (src2[2] << 8) | src2[3];
	return reinterpret_cast<int32>(r);
#endif
}
int16 ci16_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int16*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	uint16 r = return (src2[0] << 8) | src2[1];
	return reinterpret_cast<int16>(r);
#endif
}
int64 ci64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const int64*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	uint64 r = ((uint64)src2[0] << 56) | ((uint64)src2[1] << 48) | ((uint64)src2[2] << 40) | ((uint64)src2[3] << 32) | 
		((uint64)src2[4] << 24) | ((uint64)src2[5] << 16) | ((uint64)src2[6] << 8) | (uint64)src2[7];
	return reinterpret_cast<const uint64&>(r);
#endif
}
uint32 cui32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint32*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	return (src2[0] << 24) | (src2[1] << 16) | (src2[2] << 8) | src2[3];
#endif
}
uint16 cui16_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint16*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	return (src2[0] << 8) | src2[1];
#endif
}
uint64 cui64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const uint64*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	return ((uint64)src2[0] << 56) | ((uint64)src2[1] << 48) | ((uint64)src2[2] << 40) | ((uint64)src2[3] << 32) | 
		((uint64)src2[4] << 24) | ((uint64)src2[5] << 16) | ((uint64)src2[6] << 8) | (uint64)src2[7];
#endif
}
const float cr32_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const float*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	uint32 r = (src2[0] << 24) | (src2[1] << 16) | (src2[2] << 8) | src2[3];
	return reinterpret_cast<const float&>(r);		
#endif
}
const double cr64_dep(const char* const src)
{
#if LITTLE_ENDIAN
	return *reinterpret_cast<const double*>(src);
#else
	const byte* const src2 = reinterpret_cast<const byte*>(src);
	uint64 r = ((uint64)src2[0] << 56) | ((uint64)src2[1] << 48) | ((uint64)src2[2] << 40) | ((uint64)src2[3] << 32) | 
		((uint64)src2[4] << 24) | ((uint64)src2[5] << 16) | ((uint64)src2[6] << 8) | (uint64)src2[7];
	return reinterpret_cast<const double&>(r);
#endif
}

int32 ci32_le(const char* const src)
{
	return *reinterpret_cast<const int32*>(src);
}
int16 ci16_le(const char* const src)
{
	return *reinterpret_cast<const int16*>(src);
}
int64 ci64_le(const char* const src)
{
	return *reinterpret_cast<const int64*>(src);
}
uint32 cui32_le(const char* const src)
{
	return *reinterpret_cast<const uint32*>(src);
}
uint16 cui16_le(const char* const src)
{
	return *reinterpret_cast<const uint16*>(src);
}
uint64 cui64_le(const char* const src)
{
	return *reinterpret_cast<const uint64*>(src);
}
const float cr32_le(const char* const src)
{
	return *reinterpret_cast<const float*>(src);
}
const double cr64_le(const char* const src)
{
	return *reinterpret_cast<const double*>(src);
}

void i16tomb_le(int16 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int16&>(dest[0]) = v;
#else
	uint16 v2 = reinterpret_cast<uint16>(v);
	byte* dest2 = reinterpret_cast<byte*>(dest);
	dest2[0] = 0xff & (v2 >> 8);
	dest2[1] = 0xff & v2;
#endif
}
void i32tomb_le(int32 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int32&>(dest[0]) = v;
#else
	uint32 v2 = reinterpret_cast<uint32>(v);
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (v2 >> 24);
	dest2[1] = 0xff & (v2 >> 16);
	dest2[2] = 0xff & (v2 >> 8);
	dest2[3] = 0xff & (v2);
#endif
}
void i64tomb_le(int64 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<int64&>(dest[0]) = v;
#else
	uint64 v2 = reinterpret_cast<uint64>(v);
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (v2 >> 56);
	dest2[1] = 0xff & (v2 >> 48);
	dest2[2] = 0xff & (v2 >> 40);
	dest2[3] = 0xff & (v2 >> 32);
	dest2[4] = 0xff & (v2 >> 24);
	dest2[5] = 0xff & (v2 >> 16);
	dest2[6] = 0xff & (v2 >> 8);
	dest2[7] = 0xff & (v2);
#endif
}
void ui16tomb_le(uint16 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint16&>(dest[0]) = v;
#else
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (v >> 8);
	dest2[1] = 0xff & v;
#endif
}
void ui32tomb_le(uint32 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint32&>(dest[0]) = v;
#else
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (v >> 24);
	dest2[1] = 0xff & (v >> 16);
	dest2[2] = 0xff & (v >> 8);
	dest2[3] = 0xff & (v);
#endif
}
void ui64tomb_le(uint64 v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<uint64&>(dest[0]) = v;
#else
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (v >> 56);
	dest2[1] = 0xff & (v >> 48);
	dest2[2] = 0xff & (v >> 40);
	dest2[3] = 0xff & (v >> 32);
	dest2[4] = 0xff & (v >> 24);
	dest2[5] = 0xff & (v >> 16);
	dest2[6] = 0xff & (v >> 8);
	dest2[7] = 0xff & (v);
#endif
}
void r32tomb_le(float v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<float&>(dest[0]) = v;
#else
	uint32 r = reinterpret_cast<const uint32&>(v);
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (r >> 24);
	dest2[1] = 0xff & (r >> 16);
	dest2[2] = 0xff & (r >> 8);
	dest2[3] = 0xff & (r);
#endif
}
void r64tomb_le(double v, char* dest)
{
#if LITTLE_ENDIAN
	reinterpret_cast<double&>(dest[0]) = v;
#else
	uint64 r = reinterpret_cast<const uint64&>(v);
	byte* dest2 = reinterpret_cast<byte*>(dest);

	dest2[0] = 0xff & (r >> 56);
	dest2[1] = 0xff & (r >> 48);
	dest2[2] = 0xff & (r >> 40);
	dest2[3] = 0xff & (r >> 32);
	dest2[4] = 0xff & (r >> 24);
	dest2[5] = 0xff & (r >> 16);
	dest2[6] = 0xff & (r >> 8);
	dest2[7] = 0xff & (r);
#endif
}

void i16tomb_dep(int16 v, char* dest)
{
	reinterpret_cast<int16&>(dest[0]) = v;
}
void i32tomb_dep(int32 v, char* dest)
{
	reinterpret_cast<int32&>(dest[0]) = v;
}
void i64tomb_dep(int64 v, char* dest)
{
	reinterpret_cast<int64&>(dest[0]) = v;
}
void ui16tomb_dep(uint16 v, char* dest)
{
	reinterpret_cast<uint16&>(dest[0]) = v;
}
void ui32tomb_dep(uint32 v, char* dest)
{
	reinterpret_cast<uint32&>(dest[0]) = v;
}
void ui64tomb_dep(uint64 v, char* dest)
{
	reinterpret_cast<uint64&>(dest[0]) = v;
}
void r32tomb_dep(float v, char* dest)
{
	reinterpret_cast<float&>(dest[0]) = v;
}
void r64tomb_dep(double v, char* dest)
{
	reinterpret_cast<double&>(dest[0]) = v;
}



