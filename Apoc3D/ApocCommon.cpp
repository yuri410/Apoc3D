/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "ApocCommon.h"
#include "Collections/List.h"

namespace Apoc3D
{
	static_assert(sizeof(wchar_t) == 2, "wchar_t need to be configured to be 2 bytes.");


	void DebugBreak()
	{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		__debugbreak();
#else
		raise(SIGTRAP);
#endif
	}
}
