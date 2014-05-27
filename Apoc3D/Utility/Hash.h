#pragma once
#ifndef APOC3D_HASH
#define APOC3D_HASH

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Utility
	{
		class FNVHash32
		{
		public:
			FNVHash32()
				: m_result(2166136261)
			{

			}

			void Accumulate(const void* buffer, int32 count)
			{
				const int32 p = 16777619;
				const byte* src = (const byte*)buffer;

				while (--count>=0)
				{
					m_result = (*src++ ^ m_result) * p;
				}
			}

			int32 GetResult() const 
			{
				int32 hash = m_result;
				hash += hash << 13;
				hash ^= hash >> 7;
				hash += hash << 3;
				hash ^= hash >> 17;
				hash += hash << 5;
				return hash; 
			}
		private:
			int32 m_result;
		};

		template <unsigned int N, unsigned int I>
		struct FNVHash32Fixed
		{
			static int32 Hash(const char (&str)[N])
			{
				return (FNVHash32Fixed<N, I-1>::Hash(str) ^ str[I-1])*16777619;
			}
		};

		template <unsigned int N>
		struct FNVHash32Fixed<N, 1>
		{
			static int32 Hash(const char (&str)[N])
			{
				return (2166136261u ^ str[0])*16777619;
			}
		};


		inline int32 MurmurHash(const void * key, int len)
		{
			const int32 seed = 2166136261;

			const unsigned int m = 0x5bd1e995;
			const int r = 24;

			unsigned int h = seed ^ len;

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
	}
}

#endif