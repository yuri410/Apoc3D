#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "APBCommon.h"

#include "Apoc3D.D3D9RenderSystem/ConstantTable.h"

using namespace Apoc3D::Graphics::D3D9RenderSystem;

namespace APBuild
{
	bool CompileAsHLSLDX9(const String& src, const String& entryPoint, const char* pfName, bool debugEnabled, bool noOptimization,
		const List<std::pair<std::string, std::string>>* defines, ConstantTable*& constantTable, char*& codePtr, int32& codeSize);

	bool PreprocessShaderCode(const String& srcFile, const List<std::pair<std::string, std::string>>* defines, std::string& result);
}