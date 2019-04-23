#pragma once
#ifndef APOC3D_API_H
#define APOC3D_API_H

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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/List.h"

namespace Apoc3D
{
	namespace Platform
	{
		/** Define how well a platform is supported based on score. */
		struct APAPI PlatformAPISupport
		{
			/** A score on a scale of 0-100 showing how well the API supports the platform */
			int Score;
			String PlatformName;
		};

		/** Contains information of an API */
		struct APAPI APIDescription
		{
			/** The name of the graphics API. */
			String Name;
			/** A vector containing all platform supported by this API. */
			Apoc3D::Collections::List<PlatformAPISupport> SupportedPlatforms;
		};

		APAPI uint64 GetInstalledSystemMemorySizeInKB();

		APAPI int32 GetSystemDoubleClickIntervalInMS();

		APAPI std::string GetPlatformNarrowString(const wchar_t* str);

		APAPI String GetPlatformWideString(const char* str);

	}
}

#endif