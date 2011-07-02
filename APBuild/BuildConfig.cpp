/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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

#include "BuildConfig.h"

#include "Config/ConfigurationSection.h"
#include "Graphics/PixelFormat.h"
#include "Utility/StringUtils.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Apoc3D::Utility;


namespace APBuild
{
	void TextureBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SourceFile = sect->getAttribute(L"SourceFile");
		DestinationFile = sect->getAttribute(L"DestinationFile");

		GenerateMipmaps = false;
		sect->TryGetAttributeBool(L"GenerateMipmaps", GenerateMipmaps);

		bool passed = true;
		passed |= sect->TryGetAttributeInt(L"Width", NewWidth);
		passed |= sect->TryGetAttributeInt(L"Height", NewHeight);
		passed |= sect->TryGetAttributeInt(L"Depth", NewDepth);

		String flt;
		if (sect->tryGetAttribute(L"ResizeFilter", flt))
		{
			StringUtils::ToLowerCase(flt);

			if (flt == L"nearest")
			{
				ResizeFilterType = TFLT_Nearest;
			}
			else if (flt == L"box")
			{
				ResizeFilterType = TFLT_Box;
			}
			else 
			{
				ResizeFilterType = TFLT_BSpline;
			}
		}
		else
		{
			passed = false;
		}


		Resize = passed;
		
		NewFormat = FMT_Unknown;
		String fmt;
		if (sect->tryGetAttribute(L"PixelFormat", fmt))
		{
			NewFormat = PixelFormatUtils::ConvertFormat(fmt);
		}
	}

	void FontBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getAttribute(L"Name");
		Size = sect->GetAttributeSingle(L"Size");

		Style = Gdiplus::FontStyleRegular;
		String strStyle;
		if (sect->tryGetAttribute(L"Style", strStyle))
		{
			StringUtils::ToLowerCase(strStyle);

			if (strStyle == L"regular")
			{
				Style = Gdiplus::FontStyleRegular;
			}
			else if (strStyle == L"bold")
			{
				Style = Gdiplus::FontStyleBold;
			}
			else if (strStyle == L"italic")
			{
				Style = Gdiplus::FontStyleItalic;
			}
			else if (strStyle == L"bolditalic")
			{
				Style = Gdiplus::FontStyleBoldItalic;
			}
			else if (strStyle == L"underline")
			{
				Style = Gdiplus::FontStyleStrikeout;
			}
		}


		for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			const ConfigurationSection* ss = iter->second;

			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void PakBuildConfig::Parse(const ConfigurationSection* sect)
	{
		for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();
			iter != sect->SubSectionEnd(); iter++)
		{
			const ConfigurationSection* ss = iter->second;

			Files.Add(ss->getAttribute(L"FilePath"));
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void AFXBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DestFile = sect->getAttribute(L"DestinationFile");
		EntryPoint = sect->getAttribute(L"EntryPoint");
		Profile = sect->getAttribute(L"Profile");
	}
	void MeshBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");

	}
	
}