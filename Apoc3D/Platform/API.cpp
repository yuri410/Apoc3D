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

		std::string GetPlatformNarrowString(const wchar_t* str)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			size_t len = wcslen(str);
			if (len == 0) return std::string();
			int size_needed = WideCharToMultiByte(CP_ACP, 0, &str[0], (int)len, NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_ACP, 0, &str[0], (int)len, &strTo[0], size_needed, NULL, NULL);
			return strTo;
#else
			size_t bufSize = wcstombs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				char* buffer = new char[bufSize + 1];
				buffer[bufSize] = 0;
				wcstombs(buffer, str, bufSize);

				nstring result = buffer;
				delete[] buffer;
				return result;
			}
			return nstring();
#endif
		}

		String GetPlatformWideString(const char* str)
		{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			size_t len = strlen(str);
			if (len == 0) return String();
			int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)len, NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_ACP, 0, &str[0], (int)len, &wstrTo[0], size_needed);
			return wstrTo;
#else
			size_t bufSize = mbstowcs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				wchar_t* buffer = new wchar_t[bufSize + 1];
				buffer[bufSize] = 0;
				mbstowcs(buffer, str, bufSize);

				String result = buffer;
				delete[] buffer;
				return result;
			}
			return L"";
#endif
		}
	}
}