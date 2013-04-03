#pragma once
#ifndef APOC3D_IOUTILS_H
#define APOC3D_IOUTILS_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"

using namespace Apoc3D;

/** Little-endian, used in file reading
*/
inline uint16 cui16_le(const char* const src);
inline uint32 cui32_le(const char* const src);
inline uint64 cui64_le(const char* const src);
inline int16 ci16_le(const char* const src);
inline int32 ci32_le(const char* const src);	
inline int64 ci64_le(const char* const src);
inline const float cr32_le(const char* const src);
inline const double cr64_le(const char* const src);

/** Convert from memory, endian dependent
*/
inline uint16 cui16_dep(const char* const src);
inline uint32 cui32_dep(const char* const src);
inline uint64 cui64_dep(const char* const src);
inline int16 ci16_dep(const char* const src);
inline int32 ci32_dep(const char* const src);	
inline int64 ci64_dep(const char* const src);
inline const float cr32_dep(const char* const src);
inline const double cr64_dep(const char* const src);

/** Convert to Little-endian multi bytes
*/
inline void i16tomb_le(int16 v, char* dest);
inline void i32tomb_le(int32 v, char* dest);
inline void i64tomb_le(int64 v, char* dest);
inline void ui16tomb_le(uint16 v, char* dest);
inline void ui32tomb_le(uint32 v, char* dest);
inline void ui64tomb_le(uint64 v, char* dest);
inline void r32tomb_le(float v, char* dest);
inline void r64tomb_le(double v, char* dest);

/** Convert to multi bytes, the result is endian dependent
*/
inline void i16tomb_dep(int16 v, char* dest);
inline void i32tomb_dep(int32 v, char* dest);
inline void i64tomb_dep(int64 v, char* dest);
inline void ui16tomb_dep(uint16 v, char* dest);
inline void ui32tomb_dep(uint32 v, char* dest);
inline void ui64tomb_dep(uint64 v, char* dest);
inline void r32tomb_dep(float v, char* dest);
inline void r64tomb_dep(double v, char* dest);


#endif