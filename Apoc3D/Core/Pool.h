#pragma once

#ifndef APOC3D_OBJECTPOOL_H
#define APOC3D_OBJECTPOOL_H

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

#include "apoc3d/Common.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Library/tinythread.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Core
	{
		class SizedPool
		{
		public:
			explicit SizedPool(int32 instanceSize);
			~SizedPool();

			bool Checkin(void* o);

			void* CheckOut();
		private:
			void EnsureReserve(bool noLock, int32 amount);

			int32 m_instanceSize;
			tthread::mutex m_poolLock;
			Queue<void*> m_pool;
		};

		template <typename T>
		struct TypedPoolInstance
		{
			static SizedPool Instance;
		};


#define DECL_POOL_TYPE(type) \
		static void* operator new(std::size_t sz) { \
			if (sz == sizeof(type))  \
				return Apoc3D::Core::TypedPoolInstance<type>::Instance.CheckOut(); \
			return ::operator new(sz); \
		} \
		static void operator delete(void* ptr, std::size_t sz) { \
			if (sz != sizeof(type) || !Apoc3D::Core::TypedPoolInstance<type>::Instance.Checkin(ptr)) \
				::operator delete(ptr); \
		}
	}
}

#endif