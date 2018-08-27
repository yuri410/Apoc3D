#pragma once
#ifndef APOC3D_THREAD_H
#define APOC3D_THREAD_H

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

namespace Apoc3D
{
	namespace Platform
	{
		/** 
		 *  Suspends the calling thread for certain amount of time
		 *  @param ms The time for suspension in milliseconds
		 */
		APAPI void ApocSleep(uint32 ms);

		/**
		 *  This is only useful when debugging in windows. Works with Visual Studio.
		 */
		APAPI void SetThreadName(tthread::thread* th, const String& name);
	}
}

#endif