#pragma once
#ifndef APOC3D_IOUTILS_H
#define APOC3D_IOUTILS_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/ApocCommon.h"

using namespace Apoc3D;

inline bool isPlatformLittleEndian()
{
	const short int number = 1;
	return (const char&)number == 1;
}

/** Little-endian, used in file reading */
inline uint16 mb_u16_le(const char* src);
inline uint32 mb_u32_le(const char* src);
inline uint64 mb_u64_le(const char* src);
inline int16  mb_i16_le(const char* src);
inline int32  mb_i32_le(const char* src);
inline int64  mb_i64_le(const char* src);
inline float  mb_f32_le(const char* src);
inline double mb_f64_le(const char* src);

/** Convert to Little-endian multi bytes */
inline void u16_mb_le(uint16 v, char* dest);
inline void u32_mb_le(uint32 v, char* dest);
inline void u64_mb_le(uint64 v, char* dest);
inline void i16_mb_le(int16 v, char* dest);
inline void i32_mb_le(int32 v, char* dest);
inline void i64_mb_le(int64 v, char* dest);
inline void f32_mb_le(float v, char* dest);
inline void f64_mb_le(double v, char* dest);


/** Big-endian */
inline uint16 mb_u16_be(const char* src);
inline uint32 mb_u32_be(const char* src);
inline uint64 mb_u64_be(const char* src);
inline int16  mb_i16_be(const char* src);
inline int32  mb_i32_be(const char* src);
inline int64  mb_i64_be(const char* src);
inline float  mb_f32_be(const char* src);
inline double mb_f64_be(const char* src);

/** Convert to Big-endian multi bytes */
inline void u16_mb_be(uint16 v, char* dest);
inline void u32_mb_be(uint32 v, char* dest);
inline void u64_mb_be(uint64 v, char* dest);
inline void i16_mb_be(int16 v, char* dest);
inline void i32_mb_be(int32 v, char* dest);
inline void i64_mb_be(int64 v, char* dest);
inline void f32_mb_be(float v, char* dest);
inline void f64_mb_be(double v, char* dest);


/** Convert to multi bytes, the result is endian dependent */
inline void u16_mb_dep(uint16 v, char* dest){ reinterpret_cast<uint16&>(dest[0]) = v; }
inline void u32_mb_dep(uint32 v, char* dest){ reinterpret_cast<uint32&>(dest[0]) = v; }
inline void u64_mb_dep(uint64 v, char* dest){ reinterpret_cast<uint64&>(dest[0]) = v; }
inline void i16_mb_dep(int16 v, char* dest)	{ reinterpret_cast<int16&>(dest[0]) = v; }
inline void i32_mb_dep(int32 v, char* dest)	{ reinterpret_cast<int32&>(dest[0]) = v; }
inline void i64_mb_dep(int64 v, char* dest)	{ reinterpret_cast<int64&>(dest[0]) = v; }
inline void f32_mb_dep(float v, char* dest)	{ reinterpret_cast<float&>(dest[0]) = v; }
inline void f64_mb_dep(double v, char* dest){ reinterpret_cast<double&>(dest[0]) = v; }


/** Convert from memory, endian dependent */
inline uint16 mb_u16_dep(const char* src)	{ return *reinterpret_cast<const uint16*>(src); }
inline uint32 mb_u32_dep(const char* src)	{ return *reinterpret_cast<const uint32*>(src); }
inline uint64 mb_u64_dep(const char* src)	{ return *reinterpret_cast<const uint64*>(src); }
inline int16  mb_i16_dep(const char* src)	{ return *reinterpret_cast<const int16*>(src); }
inline int32  mb_i32_dep(const char* src)	{ return *reinterpret_cast<const int32*>(src); }
inline int64  mb_i64_dep(const char* src)	{ return *reinterpret_cast<const int64*>(src); }
inline float  mb_f32_dep(const char* src)	{ return *reinterpret_cast<const float*>(src); }
inline double mb_f64_dep(const char* src)	{ return *reinterpret_cast<const double*>(src); }

//////////////////////////////////////////////////////////////////////////

template <typename UT>
UT mb_ut_le(const byte* src)
{
	UT ret = 0;
	for (int i = 0; i < sizeof(UT); i++)
	{
		ret |= (UT)src[i] << (i * 8);
	}
	return ret;
}

template <typename FT, typename UT>
FT mb_ft_le(const byte* src)
{
	static_assert(sizeof(FT) == sizeof(UT), "UT and FT must match in size");
	UT ret = mb_ut_le<UT>(src);
	return reinterpret_cast<const FT&>(ret);
}

template <typename UT>
void ut_mb_le(UT v, char* dest)
{
	for (int i = 0; i < sizeof(UT); i++)
	{
		dest[i] = (char)((v >> (i * 8)) & 0xff);
	}
}


template <typename UT>
UT mb_ut_be(const byte* src)
{
	UT ret = 0;
	for (int i = 0; i < sizeof(UT); i++)
	{
		ret |= (UT)src[i] << ((sizeof(UT) - i - 1) * 8);
	}
	return ret;
}

template <typename FT, typename UT>
FT mb_ft_be(const byte* src)
{
	static_assert(sizeof(FT) == sizeof(UT), "UT and FT must match in size");
	UT ret = mb_ut_be<UT>(src);
	return reinterpret_cast<const FT&>(ret);
}

template <typename UT>
void ut_mb_be(UT v, char* dest)
{
	for (int i = 0; i < sizeof(UT); i++)
	{
		dest[i] = (char)((v >> ((sizeof(UT) - i - 1) * 8)) & 0xff);
	}
}

//////////////////////////////////////////////////////////////////////////

#ifdef BIG_ENDIAN


inline uint16 mb_u16_le(const char* src) { return mb_ut_le<uint16>((const byte*)src); }
inline uint32 mb_u32_le(const char* src) { return mb_ut_le<uint32>((const byte*)src);}
inline uint64 mb_u64_le(const char* src) { return mb_ut_le<uint64>((const byte*)src); }
inline float  mb_f32_le(const char* src) { return mb_ft_le<float, uint32>((const byte*)src); }
inline double mb_f64_le(const char* src) { return mb_ft_le<double, uint64>((const byte*)src); }

inline void u16_mb_le(uint16 v, char* dest) { ut_mb_le(v, dest); }
inline void u32_mb_le(uint32 v, char* dest) { ut_mb_le(v, dest); }
inline void u64_mb_le(uint64 v, char* dest) { ut_mb_le(v, dest); }
inline void f32_mb_le(float v, char* dest)  { ut_mb_le(reinterpret_cast<const uint32&>(v), dest); }
inline void f64_mb_le(double v, char* dest) { ut_mb_le(reinterpret_cast<const uint64&>(v), dest); }

#else

inline uint16 mb_u16_le(const char* src) { return *reinterpret_cast<const uint16*>(src); }
inline uint32 mb_u32_le(const char* src) { return *reinterpret_cast<const uint32*>(src); }
inline uint64 mb_u64_le(const char* src) { return *reinterpret_cast<const uint64*>(src); }
inline float  mb_f32_le(const char* src) { return *reinterpret_cast<const float*>(src); }
inline double mb_f64_le(const char* src) { return *reinterpret_cast<const double*>(src); }

inline void u16_mb_le(uint16 v, char* dest) { reinterpret_cast<uint16&>(dest[0]) = v; }
inline void u32_mb_le(uint32 v, char* dest) { reinterpret_cast<uint32&>(dest[0]) = v; }
inline void u64_mb_le(uint64 v, char* dest) { reinterpret_cast<uint64&>(dest[0]) = v; }
inline void f32_mb_le(float v, char* dest)  { reinterpret_cast<float&>(dest[0]) = v; }
inline void f64_mb_le(double v, char* dest) { reinterpret_cast<double&>(dest[0]) = v; }

#endif

inline int16 mb_i16_le(const char* src) { return (int16)mb_u16_le(src); }
inline int32 mb_i32_le(const char* src) { return (int32)mb_u32_le(src); }
inline int64 mb_i64_le(const char* src) { return (int64)mb_u64_le(src); }

inline void i16_mb_le(int16 v, char* dest) { u16_mb_le(reinterpret_cast<uint16&>(v), dest); }
inline void i32_mb_le(int32 v, char* dest) { u32_mb_le(reinterpret_cast<uint32&>(v), dest); }
inline void i64_mb_le(int64 v, char* dest) { u64_mb_le(reinterpret_cast<uint64&>(v), dest); }

//////////////////////////////////////////////////////////////////////////

#ifdef BIG_ENDIAN

inline uint16 mb_u16_be(const char* src) { return *reinterpret_cast<const uint16*>(src); }
inline uint32 mb_u32_be(const char* src) { return *reinterpret_cast<const uint32*>(src); }
inline uint64 mb_u64_be(const char* src) { return *reinterpret_cast<const uint64*>(src); }
inline float  mb_f32_be(const char* src) { return *reinterpret_cast<const float*>(src); }
inline double mb_f64_be(const char* src) { return *reinterpret_cast<const uint64*>(src); }

inline void u16_mb_be(uint16 v, char* dest) { reinterpret_cast<uint16&>(dest[0]) = v; }
inline void u32_mb_be(uint32 v, char* dest) { reinterpret_cast<uint32&>(dest[0]) = v; }
inline void u64_mb_be(uint64 v, char* dest) { reinterpret_cast<uint64&>(dest[0]) = v; }
inline void f32_mb_be(float v, char* dest)  { reinterpret_cast<float&>(dest[0]) = v; }
inline void f64_mb_be(double v, char* dest) { reinterpret_cast<double&>(dest[0]) = v; }

#else

inline uint16 mb_u16_be(const char* src) { return mb_ut_be<uint16>((const byte*)src); }
inline uint32 mb_u32_be(const char* src) { return mb_ut_be<uint32>((const byte*)src); }
inline uint64 mb_u64_be(const char* src) { return mb_ut_be<uint64>((const byte*)src); }
inline float  mb_f32_be(const char* src) { return mb_ft_be<float, uint32>((const byte*)src); }
inline double mb_f64_be(const char* src) { return mb_ft_be<double, uint64>((const byte*)src); }

inline void u16_mb_be(uint16 v, char* dest) { ut_mb_be(v, dest); }
inline void u32_mb_be(uint32 v, char* dest) { ut_mb_be(v, dest); }
inline void u64_mb_be(uint64 v, char* dest) { ut_mb_be(v, dest); }
inline void f32_mb_be(float v, char* dest)  { ut_mb_be(reinterpret_cast<const uint32&>(v), dest); }
inline void f64_mb_be(double v, char* dest) { ut_mb_be(reinterpret_cast<const uint64&>(v), dest); }

#endif

inline int16  mb_i16_be(const char* src) { return (int16)mb_u16_be(src); }
inline int32  mb_i32_be(const char* src) { return (int32)mb_u32_be(src); }
inline int64  mb_i64_be(const char* src) { return (int64)mb_u64_be(src); }

inline void i16_mb_be(int16 v, char* dest) { u16_mb_be(reinterpret_cast<uint16&>(v), dest); }
inline void i32_mb_be(int32 v, char* dest) { u32_mb_be(reinterpret_cast<uint32&>(v), dest); }
inline void i64_mb_be(int64 v, char* dest) { u64_mb_be(reinterpret_cast<uint64&>(v), dest); }

#endif