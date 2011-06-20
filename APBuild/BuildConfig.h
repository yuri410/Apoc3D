
#ifndef BUILDCONFIG_H
#define BUILDCONFIG_H
#include "APBCommon.h"

#include "Graphics/PixelFormat.h"
#include "Collections/FastList.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;

namespace APBuild
{
	enum TextureFilterType
	{
		TFLT_Nearest,
		TFLT_Box,
		TFLT_BSpline
	};
	struct TextureBuildConfig 
	{
		String SourceFile;
		String DestinationFile;
		bool GenerateMipmaps;
		bool Resize;
		int NewWidth;
		int NewHeight;
		int NewDepth;
		TextureFilterType ResizeFilterType;
		Apoc3D::Graphics::PixelFormat NewFormat;

		void Parse(const ConfigurationSection* sect);
	};

	struct CharRange
	{
		int MinChar;
		int MaxChar;

	};
	struct FontBuildConfig
	{
		FastList<CharRange> Ranges;
		uint Style;
		String Name;
		float Size;

		String DestFile;

		void Parse(const ConfigurationSection* sect);
	};

	struct MeshConfig
	{
		String SrcFile;
		String DstFile;

		void Parse(const ConfigurationSection* sect);
		
	};
}
#endif