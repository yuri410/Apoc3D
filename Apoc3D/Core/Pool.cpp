/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2014 Tao Xin
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

#include "Pool.h"

#include "apoc3d/Library/tinyxml.h"
#include "apoc3d/Config/ConfigurationSection.h"

using namespace Apoc3D::Config;

namespace Apoc3D
{
	namespace Core
	{
		FixedList<std::pair<int32, SizedPool*>, 10> customPools;

		//////////////////////////////////////////////////////////////////////////
		const uint32 PoolMemorySignature = 0xa5afd100;

		enum
		{
			MF_None = 0,
			MF_PreallocatedHead = 1 << 0,
			MF_PreallocatedBody = 1 << 1,
			MF_Used = 1 << 2
		};

		

		static bool tagCheckInMemoryBlock(void*& b)
		{
			char* ptr = (char*)b;
			ptr -= sizeof(PoolMemorySignature);

			// check signature
			uint32& id = *(uint32*)ptr;
			if ((id & 0xffffff00U) != PoolMemorySignature)
				return false;

			byte flags = (byte)(0xff & id);
			assert((flags & MF_Used)); // already freed

			flags &= ~MF_Used;
			id = PoolMemorySignature | flags;

			b = ptr;

			return true;
		}
		static void tagCheckOutMemoryBlock(void*& b)
		{
			char* ptr = (char*)b;

			uint32& id = *(uint32*)ptr;
			assert((id & MF_Used) == 0);
			id |= MF_Used;

			ptr += sizeof(PoolMemorySignature);
			b = ptr;
		}
		static void tagNewMemeoryBlock(void* b, byte flags)
		{
			uint32& id = *(uint32*)b;
			id = PoolMemorySignature | flags;
		}

		const int32 PreallocationCount = 50;
		
		SizedPool::SizedPool(int32 instanceSize)
			: m_instanceSize(instanceSize), m_pool(PreallocationCount)
		{
			customPools.Add({ instanceSize, this });

			EnsureReserve(false, PreallocationCount);
		}
		SizedPool::~SizedPool()
		{
			m_poolLock.lock();

			List<void*> blocksToClear;
			for (int32 i = 0; i < m_pool.getCount();i++)
			{
				char* buf = (char*)m_pool[i];
				uint32 id = *(uint32*)buf;

				assert((id & 0xffffff00U) == PoolMemorySignature);

				byte flags = (byte)(id & 0xff);
				if (flags & MF_PreallocatedHead)
				{
					blocksToClear.Add(buf);
				}
			}

			for (void* m : blocksToClear)
				free(m);

			m_poolLock.unlock();
		}

		bool SizedPool::Checkin(void* o)
		{
			if (o == nullptr)
				return true;

			if (!tagCheckInMemoryBlock(o))
				return false;

			m_poolLock.lock();
			m_pool.Enqueue(o);
			m_poolLock.unlock();

			return true;
		}

		void* SizedPool::CheckOut()
		{
			void* result = nullptr;
			m_poolLock.lock();
			
			EnsureReserve(true, 1);

			result = m_pool.Dequeue();

			m_poolLock.unlock();

			tagCheckOutMemoryBlock(result);
			return result;
		}

		void SizedPool::EnsureReserve(bool noLock, int32 amount)
		{
			if (!noLock) m_poolLock.lock();

			if (m_pool.getCount() == 0)
			{
				const int32 PaddedInstanceSize = m_instanceSize + sizeof(PoolMemorySignature);
				const int32 PreallocationSize = amount * PaddedInstanceSize;

				char* buf = (char*)malloc(PreallocationSize);

				for (int32 i = 0; i < amount; i++)
				{
					tagNewMemeoryBlock(buf, i == 0 ? MF_PreallocatedHead : MF_PreallocatedBody);
					m_pool.Enqueue(buf);
					buf += PaddedInstanceSize;
				}

			}

			if (!noLock) m_poolLock.unlock();
		}

		//////////////////////////////////////////////////////////////////////////

#define DEFINE_POOL_TYPE(type) SizedPool TypedPoolInstance<type>::Instance(sizeof(type))

		DEFINE_POOL_TYPE(ConfigurationSection);

		DEFINE_POOL_TYPE(TiXmlDeclaration);
		DEFINE_POOL_TYPE(TiXmlAttribute);
		DEFINE_POOL_TYPE(TiXmlElement);
		DEFINE_POOL_TYPE(TiXmlComment);
		DEFINE_POOL_TYPE(TiXmlText);
		DEFINE_POOL_TYPE(TiXmlUnknown);

	}
}