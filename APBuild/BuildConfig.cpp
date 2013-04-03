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

#include "BuildConfig.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Utility/StringUtils.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Apoc3D::Utility;

namespace APBuild
{
	void TextureBuildConfig::Parse(const ConfigurationSection* sect)
	{
		String method = L"d3d";
		sect->tryGetAttribute(L"Method", method);
		StringUtils::ToLowerCase(method);

		if (method == L"d3d")
		{
			Method = TEXBUILD_D3D;
		}
		else if (method == L"d3dl")
		{
			Method = TEXBUILD_D3D_LatencySupport;
		}
		else if (method == L"devil")
		{
			Method = TEXBUILD_Devil;
		}
		else if (method == L"default")
		{
			Method = TEXBUILD_BuiltIn;
		}

		AssembleCubemap = false;
		AssembleVolumeMap = false;

		String assembleType;
		if (!sect->tryGetAttribute(L"Assemble", assembleType))
		{
			SourceFile = sect->getAttribute(L"SourceFile");
		}
		else
		{
			StringUtils::ToLowerCase(assembleType);
			if (assembleType == L"cubemap")
			{
				AssembleCubemap = true;
				AssembleVolumeMap = false;

				String file = sect->getAttribute(L"NegX");
				SubMapTable.Add((uint)CUBE_NegativeX, file);
				file = sect->getAttribute(L"NegY");
				SubMapTable.Add((uint)CUBE_NegativeY, file);
				file = sect->getAttribute(L"NegZ");
				SubMapTable.Add((uint)CUBE_NegativeZ, file);

				file = sect->getAttribute(L"PosX");
				SubMapTable.Add((uint)CUBE_PositiveX, file);
				file = sect->getAttribute(L"PosY");
				SubMapTable.Add((uint)CUBE_PositiveY, file);
				file = sect->getAttribute(L"PosZ");
				SubMapTable.Add((uint)CUBE_PositiveZ, file);

				if (sect->tryGetAttribute(L"NegXAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeX, file);
				if (sect->tryGetAttribute(L"NegYAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeY, file);
				if (sect->tryGetAttribute(L"NegZAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_NegativeZ, file);

				if (sect->tryGetAttribute(L"PosXAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveX, file);
				if (sect->tryGetAttribute(L"PosYAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveY, file);
				if (sect->tryGetAttribute(L"PosZAlpha", file))
					SubAlphaMapTable.Add((uint)CUBE_PositiveZ, file);

			}
			else
			{
				AssembleCubemap = false;
				AssembleVolumeMap = true;

				uint i =0;
				ConfigurationSection* srcsect = sect->getSection(L"Source");
				for (ConfigurationSection::SubSectionEnumerator iter = srcsect->GetSubSectionEnumrator();
					iter.MoveNext();)
				{
					const ConfigurationSection* ss = *iter.getCurrentValue();

					SubMapTable.Add(i, ss->getAttribute(L"FilePath"));
					i++;
				}
				srcsect = sect->getSection(L"AlphaSource");
				i=0;
				for (ConfigurationSection::SubSectionEnumerator iter = srcsect->GetSubSectionEnumrator();
					iter.MoveNext();)
				{
					const ConfigurationSection* ss = *iter.getCurrentValue();

					SubMapTable.Add(i, ss->getAttribute(L"FilePath"));
					i++;
				}
				
			}
			
		}

		DestinationFile = sect->getAttribute(L"DestinationFile");

		GenerateMipmaps = false;
		sect->TryGetAttributeBool(L"GenerateMipmaps", GenerateMipmaps);

		bool passed = false;
		passed |= sect->TryGetAttributeInt(L"Width", NewWidth);
		passed |= sect->TryGetAttributeInt(L"Height", NewHeight);
		passed |= sect->TryGetAttributeInt(L"Depth", NewDepth);
		
		ResizeFilterType = TFLT_BSpline;
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

		AntiAlias = true;
		sect->TryGetAttributeBool(L"AntiAlias", AntiAlias);

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


		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			CharRange range = { ss->GetAttributeInt(L"Start"), ss->GetAttributeInt(L"End") };
			Ranges.Add(range);
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void PakBuildConfig::Parse(const ConfigurationSection* sect)
	{
		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			String path;
			if (ss->tryGetAttribute(L"FilePath", path))
			{
				Files.Add(path);
			}
			else
			{
				path = ss->getAttribute(L"DirPath");
				bool flatten = false;
				ss->TryGetAttributeBool(L"Flatten", flatten);

				PakDirEntryConfig ent;
				ent.Flatten = flatten;
				ent.Path = path;
				Dirs.Add(ent);
			}

			
		}

		DestFile = sect->getAttribute(L"DestinationFile");
	}
	void AFXBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();

		SrcVSFile = sect->getAttribute(L"VSSource");
		SrcPSFile = sect->getAttribute(L"PSSource");
		DestFile = sect->getAttribute(L"DestinationFile");
		PListFile = sect->getAttribute(L"ParamList");
		EntryPointVS = sect->getAttribute(L"EntryPointVS");
		EntryPointPS = sect->getAttribute(L"EntryPointPS");
		Profile = sect->getAttribute(L"Profile");
	}
	void CFXBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();

		SrcVSFile = sect->getAttribute(L"VSSource");
		SrcPSFile = sect->getAttribute(L"PSSource");
		DestFile = sect->getAttribute(L"DestinationFile");
		EntryPointVS = sect->getAttribute(L"EntryPointVS");
		EntryPointPS = sect->getAttribute(L"EntryPointPS");
		Profile = sect->getAttribute(L"Profile");
	}
	void FXListBuildConfig::Parse(const ConfigurationSection* sect)
	{
		Name = sect->getName();
		DestFile = sect->getAttribute(L"DestinationFile");



	}
	void MeshBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		sect->tryGetAttribute(L"DestinationAnimFile", DstAnimationFile);

		String method = L"ass";
		sect->tryGetAttribute(L"Method", method);
		StringUtils::ToLowerCase(method);

		if (method == L"ass")
		{
			Method = MESHBUILD_ASS;
		}
		else if (method == L"fbx")
		{
			Method = MESHBUILD_FBX;
		}
		else if (method == L"d3d")
		{
			Method = MESHBUILD_D3D;
		}

		UseVertexFormatConversion = false;
		CollapseMeshs = false;

		ConfigurationSection* subs = sect->getSection(L"VertexFormatConversion");
		if (subs && subs->getSubSectionCount()>0)
		{
			UseVertexFormatConversion = true;
			for (ConfigurationSection::SubSectionEnumerator iter = subs->GetSubSectionEnumrator();
				iter.MoveNext();)
			{
				const ConfigurationSection* ent = *iter.getCurrentValue();

				VertexElementUsage usage = GraphicsCommonUtils::ParseVertexElementUsage(ent->getName());
				int index = 0;
				if (usage == VEU_TextureCoordinate)
				{
					index = StringUtils::ParseInt32( ent->getValue() );
				}
				// the vertex elements here only has usage and index. 
				// They only store info here, not for normal use in graphics
				VertexElements.Add(VertexElement(0,VEF_Count,usage,index));
			}
		}
		subs = sect->getSection(L"CollapseMeshs");
		if (subs)
		{
			CollapseMeshs = true;
		}
	}

	void MaterialBuildConfig::Parse(const ConfigurationSection* sect)
	{
		DstFile = sect->getAttribute(L"DestinationFile");
	}

	void TransformAnimBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
		Reverse = sect->GetAttributeBool(L"Reverse");

		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();
			iter.MoveNext();)
		{
			const ConfigurationSection* ss = *iter.getCurrentValue();

			String name = ss->getName();
			int objIdx = StringUtils::ParseInt32(ss->getValue());

			ObjectIndexMapping.Add(name, objIdx);
		}
	}
	void MaterialAnimBuildConfig::Parse(const ConfigurationSection* sect)
	{
		SrcFile = sect->getAttribute(L"SourceFile");
		DstFile = sect->getAttribute(L"DestinationFile");
	}
	
}