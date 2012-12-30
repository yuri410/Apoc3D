#include "API.h"

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
			ULONGLONG result;
			GetPhysicallyInstalledSystemMemory(&result);

			return result;
#else
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return status.ullTotalPhys;
#endif
		}
	}
}