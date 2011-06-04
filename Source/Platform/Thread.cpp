/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "Thread.h"
#include <Windows.h>


namespace Apoc3D
{
	namespace Platform
	{
		void ApocSleep(uint32 ms)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			Sleep(ms);
#else
			// Single thread sleep code thanks to Furquan Shaikh
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
			pthread_mutex_unlock(&fakeMutex);
#endif
		}
	}
}