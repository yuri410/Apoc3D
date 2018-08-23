#pragma once
#ifndef APOC3D_HASH
#define APOC3D_HASH

#include "apoc3d/ApocCommon.h"

namespace Apoc3D
{
	namespace Utility
	{
		class FNVHash32
		{
		public:
			FNVHash32() { }
			FNVHash32(uint32 current) : m_result(current) { }

			FNVHash32& Accumulate(const void* buffer, size_t size)
			{
				const uint32 p = 16777619;
				const byte* src = (const byte*)buffer;

				while (size-- > 0)
				{
					m_result = (*src++ ^ m_result) * p;
				}
				return *this;
			}

			uint32 getResult() const { return m_result; }

		private:
			uint32 m_result = 2166136261;
		};

		class FNVHash64
		{
		public:
			FNVHash64() { }
			FNVHash64(uint64 current) : m_result(current) { }

			FNVHash64& Accumulate(const void* buffer, size_t size)
			{
				const uint64 p = 1099511628211;
				const byte* src = (const byte*)buffer;

				while (size-- > 0)
				{
					m_result = (*src++ ^ m_result) * p;
				}
				return *this;
			}

			uint64 getResult() const { return m_result; }
		private:
			uint64 m_result = 14695981039346656037;
		};

		// not including null terminator
		constexpr uint32 FNVHash32Const(const char* str, const uint32 val = 2166136261u)
		{
			return (*str == '\0') ? val : FNVHash32Const(str + 1, (uint64)(val ^ (uint32)*str) * 16777619u);
		}

		//template <uint32 N>
		//constexpr uint32 FNVHash32Const(const char(&str)[N], uint32 I = N)
		//{
		//	return (I == 1 ? (2166136261u ^ str[0]) : (uint64)(FNVHash32Const(str, I - 1) ^ str[I - 1])) * 16777619u;
		//}


		inline int32 MurmurHash(const void * key, size_t len)
		{
			/**
			 * MurmurHash derived from http://sites.google.com/site/murmurhash/
			 * Code is released to the public domain. 
			 * For business purposes, Murmurhash is under the MIT license.
			 * Developed by Austin Appleby
			 */

			const int32 seed = 2166136261;

			const uint32 m = 0x5bd1e995;
			const int32 r = 24;

			uint32 h = seed ^ (uint32)len;

			const unsigned char * data = (const unsigned char *)key;

			while(len >= 4)
			{
				unsigned int k = *(unsigned int *)data;

				k *= m; 
				k ^= k >> r; 
				k *= m; 

				h *= m; 
				h ^= k;

				data += 4;
				len -= 4;
			}

			switch(len)
			{
			case 3: h ^= data[2] << 16;
			case 2: h ^= data[1] << 8;
			case 1: h ^= data[0];
				h *= m;
			};

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			return h;
		}
		
		APAPI uint32 CalculateCRC32(const void *buf, int32 size, uint32 crc = 0);

		APAPI uint32 CalculateCRC32(Apoc3D::IO::Stream& strm);
		
	}
}

#endif