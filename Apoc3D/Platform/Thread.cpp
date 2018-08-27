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

#include "Thread.h"

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <Windows.h>

const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

#elif APOC3D_PLATFORM == APOC3D_PLATFORM_MAC
#include <Carbon/Carbon.h>
#else
#include <pthread.h>
#endif
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Library/tinythread.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Platform
	{
		void ApocSleep(uint32 ms)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			Sleep(ms);
#else
			usleep(ms*1000);
#endif

			/*// Single thread sleep code thanks to Furquan Shaikh
			// http://somethingswhichidintknow.blogspot.com/2009/09/sleep-in-pthread.html
			// Modified slightly from the original
			pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
			pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;
			struct timespec timeToWait;
			struct timeval now;
			int rt;

			gettimeofday(&now,NULL);

			long seconds = ms/1000;
			long nanoseconds = (ms - seconds * 1000) * 1000000;
			timeToWait.tv_sec = now.tv_sec + seconds;
			timeToWait.tv_nsec = now.tv_usec*1000 + nanoseconds;

			if (timeToWait.tv_nsec >= 1000000000)
			{
				timeToWait.tv_nsec -= 1000000000;
				timeToWait.tv_sec++;
			}

			pthread_mutex_lock(&fakeMutex);
			rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
			pthread_mutex_unlock(&fakeMutex);*/
		}
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#if _DEBUG
		void SetThreadNameInternal( DWORD dwThreadID, LPCSTR szThreadName)
		{
#if _MSC_VER > 1400
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = szThreadName;
			info.dwThreadID = dwThreadID;
			info.dwFlags = 0;

			__try
			{
				RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (const ULONG_PTR*)&info );
			}
			__except(EXCEPTION_CONTINUE_EXECUTION)
			{
			}
#endif
		}
#endif
#endif
		
		void SetThreadName( tthread::thread* th, const String& name)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#if _DEBUG
			const tthread::thread::native_handle_type handle = th->native_handle();
			SetThreadNameInternal(GetThreadId(handle), StringUtils::toPlatformNarrowString(name).c_str());
#endif
#endif
		}
	}
}