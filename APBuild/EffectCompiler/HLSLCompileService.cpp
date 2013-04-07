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


#include "HLSLCompileService.h"

#include "../BuildConfig.h"
#include "../BuildEngine.h"
#include "../CompileLog.h"
#include "apoc3d/Utility/StringUtils.h"

#include <d3d9.h>
#include <d3dx9.h>

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;


namespace APBuild
{
	bool CompileShader(const String& src, const String& entryPoint, const String& profile, char*& dest, int& size, bool isVS)
	{
		String prof = profile;
		StringUtils::ToLowerCase(prof);

		String spf;
		if (prof == L"sm2.0")
		{
			spf = isVS ? L"vs_2_0" : L"ps_2_0";
		}
		else if (prof == L"sm3.0")
		{
			spf = isVS ? L"vs_3_0" : L"ps_3_0";
		}
		else if (prof == L"sm1.0")
		{
			spf = isVS ? L"vs_1_1" : L"ps_1_1";
		}

		ID3DXBuffer* error = 0;
		ID3DXBuffer* shader = 0;
		ID3DXConstantTable* constants;

		HRESULT hr = D3DXCompileShaderFromFile(src.c_str(), 0, 0, 
			StringUtils::toString(entryPoint.c_str()).c_str(), StringUtils::toString(spf.c_str()).c_str(), 
			D3DXSHADER_PACKMATRIX_ROWMAJOR, &shader, &error, &constants);

		if (hr != S_OK)
		{
			if (error)
			{
				std::string errmsg(reinterpret_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize());

				List<String> errs;
				StringUtils::Split(StringUtils::toWString(errmsg), errs, L"\n\r");

				for (int32 i=0;i<errs.getCount();i++)
				{
					CompileLog::WriteError(errs[i], src);
				}
				error->Release();
				return false;
			}
			else
			{
				CompileLog::WriteError(L"Failed due to unknown problem.", src);
				return false;
			}
		}
		constants->Release();

		size = static_cast<int>( shader->GetBufferSize());
		dest = new char[size];

		memcpy(dest, shader->GetBufferPointer(), size);
		return true;
	}
}