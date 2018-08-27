#pragma once

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

#ifndef BUILDENGINE_H
#define BUILDENGINE_H

#include "APBCommon.h"

using namespace Apoc3D;

namespace APBuild
{
	namespace BuildSystem
	{
		int Initialize();
		void Finalize();

		int Build(ConfigurationSection* sect, ConfigurationSection* attachmentSect);
		
		void EnsureDirectory(const String& path);

		enum CompileLogType
		{
			COMPILE_Warning,
			COMPILE_Information,
			COMPILE_Error
		};

		void SetLoggingOutputPathRelativeBase(const String& basePath);

		void Log(CompileLogType type, const String& message, const String& location);

		void LogEntryProcessed(const String& destFile, const String& virtualItemPath, const String& prefix = L"");

		void LogInformation(const String& message, const String& location);
		void LogError(const String& message, const String& location);
		void LogWarning(const String& message, const String& location);
		void LogClear();
	}
}

#endif