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

#include "APBCommon.h"

#include "Apoc3D.D3D9RenderSystem/ConstantTable.h"

using namespace Apoc3D::Graphics::D3D9RenderSystem;

namespace APBuild
{
	bool CompileAsHLSLDX9(const String& src, const String& entryPoint, const char* pfName, bool debugEnabled, bool noOptimization,
		const List<std::pair<std::string, std::string>>& defines, ConstantTable*& constantTable, char*& codePtr, int32& codeSize);

	bool PreprocessShaderCode(const String& srcFile, const List<std::pair<std::string, std::string>>& defines, std::string& result);
}