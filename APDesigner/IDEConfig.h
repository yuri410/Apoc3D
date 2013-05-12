#pragma once

#include "APDCommon.h"

using namespace Apoc3D;

namespace APDesigner
{
	void cfInitializeConfig();
	void cfFlushConfig();

	const Queue<std::pair<String, String>>& cfGetRecentProjects();
	void cfAddRecentProject(const String& name, const String& path);
	void cfRemoveRecentProject(int index);
}