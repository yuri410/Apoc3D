#pragma once

#include "apoc3d.Essentials/EssentialCommon.h"
#include "apoc3d/Core/Logging.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	inline void LogInfo(const std::string& txt)
	{
		ApocLog(LOG_App, txt, LOGLVL_Infomation);
	}
	inline void LogError(const std::string& txt)
	{
		ApocLog(LOG_App, txt, LOGLVL_Error);
	}
	inline void LogWarning(const std::string& txt)
	{
		ApocLog(LOG_App, txt, LOGLVL_Warning);
	}

	void LogErrorBreak(const std::string& txt);

	template <typename... Args>
	void LogInfo(const std::string& format, const Args& ... args) { LogInfo(fmt::format(format, args...)); }

	template <typename... Args>
	void LogError(const std::string& format, const Args& ... args) { LogError(fmt::format(format, args...)); }

	template <typename... Args>
	void LogErrorBreak(const std::string& format, const Args& ... args) { LogErrorBreak(fmt::format(format, args...)); }

	template <typename... Args>
	void LogWarning(const std::string& format, const Args& ... args) { LogWarning(fmt::format(format, args...)); }
}
