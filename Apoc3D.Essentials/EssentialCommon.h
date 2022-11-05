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

#ifndef APOC3DEX_ESSENTIALCOMMON_H
#define APOC3DEX_ESSENTIALCOMMON_H

#include "apoc3d/ApocCommon.h"
#include <fmt/format.h>
#include <json.hpp>

#ifndef APOC3D_DYNLIB
#	define APEXAPI
#else
#	ifdef APOC3D_ESS_DYLIB_EXPORT
#		define APEXAPI __declspec( dllexport )
#	else
#		define APEXAPI __declspec( dllimport )
#	endif
#endif

#pragma comment (lib, "Apoc3d.lib")

namespace Apoc3D
{
	namespace AI
	{
		class AStarNode;
		class PathFinderField;
		class PathFinder;
		class PathFinderResult;
		class AStarNode3;
		class VolumePathFinderResult;
		class VolumePathFinder;
		class VolumePathFinderField;
		struct VolumePathFinderResultPoint;
	};

	using ByteBuffer = std::string;

	namespace UI
	{
		class IChartDataSource;
		class IChartAxisFomatter;

		class NumberRangeAxisFormatter;

		class Tab;
		class Pane;
		class TabRenderer;
		class WorkerPanel;
	}

	namespace Utility
	{
		class DistributionHistogram;
	}
}

using nlohmann::json;

using namespace Apoc3D;
using namespace Apoc3D::Collections;

namespace fmt
{
	template <typename CharType, typename AF>
	class BasicFormatter;

#define DEFINE_FMT_FORMAT(type) \
	template <typename AF> void format_arg(BasicFormatter<char, AF> &f, const char *&format_str, const type& t) \
	{ \
		f.writer().write(_ToString(t)); \
	} \
	template <typename AF> void format_arg(BasicFormatter<wchar_t, AF> &f, const wchar_t *&format_str, const type& t) \
	{ \
		f.writer().write(StringUtils::toASCIIWideString(_ToString(t))); \
	}
}

#endif