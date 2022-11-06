#include "Logger.h"

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Apoc3D
{
	void LogErrorBreak(const std::string& txt)
	{
		LogError(txt);

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		if (IsDebuggerPresent())
			::DebugBreak();
#endif
	}
}