#include "API.h"
#include "apoc3d/Utility/StringUtils.h"
#include <dirent.h>
#include <sys/utime.h>

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace Apoc3D
{
	namespace Platform
	{
		uint64 GetInstalledSystemMemorySizeInKB()
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			//ULONGLONG result;
			//GetPhysicallyInstalledSystemMemory(&result);
			//return result;
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			if (GlobalMemoryStatusEx(&statex))
				return statex.ullTotalPhys / 1024;
			return 0;
#else
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return status.ullTotalPhys;
#endif
		}

		int32 GetSystemDoubleClickIntervalInMS()
		{

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			UINT result = GetDoubleClickTime();
			return static_cast<int32>(result);
#else
			return 500;
#endif
		}

		void TimetToFileTime(time_t t, FILETIME& ft)
		{
			int64 temp = t * 10000000 + 116444736000000000LL;
			
			ft.dwLowDateTime = static_cast<DWORD>(temp);
			ft.dwHighDateTime = static_cast<DWORD>(temp >> 32);
		}


	}
}