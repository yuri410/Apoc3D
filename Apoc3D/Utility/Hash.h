#pragma once
#ifndef APOC3D_HASH
#define APOC3D_HASH

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Utility
	{
		class FNVHash
		{
		public:
			FNVHash()
				: m_result(2166136261)
			{

			}

			void Accumulate(const void* buffer, int32 count)
			{
				const int32 p = 16777619;
				for (int i=0;i<count;i++)
				{
					m_result = (m_result ^ ((byte*)buffer)[i]) * p;
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
	}
}

#endif