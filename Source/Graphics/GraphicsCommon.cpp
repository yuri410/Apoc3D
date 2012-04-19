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
#include "GraphicsCommon.h"

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			 Blend GraphicsCommonUtils::ParseBlend(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);

				 if (v == L"zero")
				 {
					 return BLEND_Zero;
				 }
				 else if (v == L"one")
				 {
					 return BLEND_One;
				 }
				 else if (v == L"sourcecolor")
				 {
					 return BLEND_SourceColor;
				 }
				 else if (v == L"inversesourcecolor")
				 {
					 return BLEND_InverseSourceColor;
				 }
				 else if (v == L"sourcealpha")
				 {
					 return BLEND_SourceAlpha;
				 }
				 else if (v == L"inversesourcealpha")
				 {
					 return BLEND_InverseSourceAlpha;
				 }
				 else if (v == L"destinationalpha")
				 {
					 return BLEND_DestinationAlpha;
				 }
				 else if (v == L"inversedestinationalpha")
				 {
					 return BLEND_InverseDestinationAlpha;
				 }
				 else if (v == L"destinationcolor")
				 {
					 return BLEND_DestinationColor;
				 }
				 else if (v == L"inversedestinationcolor")
				 {
					 return BLEND_InverseDestinationColor;
				 }
				 else if (v == L"sourcealphasaturation")
				 {
					 return BLEND_SourceAlphaSaturation;
				 }
				 else if (v == L"bothsourcealpha")
				 {
					 return BLEND_BothSourceAlpha;
				 }
				 else if (v == L"blendfactor")
				 {
					 return BLEND_BlendFactor;
				 }
				 return BLEND_One;
			 }
			 String GraphicsCommonUtils::ToString(Blend blend)
			 {
				 switch (blend)
				 {
				 case BLEND_Zero:
					 return L"Zero";
				 case BLEND_One:
					 return L"One";
				 case BLEND_SourceColor:
					 return L"SourceColor";
				 case BLEND_InverseSourceColor:
					 return L"InverseSourceColor";
				 case BLEND_SourceAlpha:
					 return L"SourceAlpha";
				 case BLEND_InverseSourceAlpha:
					 return L"InverseSourceAlpha";
				 case BLEND_DestinationAlpha:
					 return L"DestinationAlpha";
				 case BLEND_InverseDestinationAlpha:
					 return L"InverseDestinationAlpha";
				 case BLEND_DestinationColor:
					 return L"DestinationColor";
				 case BLEND_InverseDestinationColor:
					 return L"InverseDestinationColor";
				 case BLEND_SourceAlphaSaturation:
					 return L"SourceAlphaSaturation";
				 case BLEND_BothSourceAlpha:
					 return L"BothSourceAlpha";
				 case BLEND_BlendFactor:
					 return L"BlendFactor";
				 }
				 return L"One";
			 }

			 BlendFunction GraphicsCommonUtils::ParseBlendFunction(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);
				 if (v == L"add")
				 {
					 return BLFUN_Add;
				 }
				 else if (v == L"subtract")
				 {
					 return BLFUN_Subtract;
				 }
				 else if (v == L"reversesubtract")
				 {
					 return BLFUN_ReverseSubtract;
				 }
				 else if (v == L"min")
				 {
					 return BLFUN_Min;
				 }
				 else if (v == L"max")
				 {
					 return BLFUN_Max;
				 }
				 return BLFUN_Add;
			 }
			 String GraphicsCommonUtils::ToString(BlendFunction func)
			 {
				 switch (func)
				 {
				 case BLFUN_Add:
					 return L"Add";
				 case BLFUN_Subtract:
					 return L"Subtract";
				 case BLFUN_ReverseSubtract:
					 return L"ReverseSubtract";
				 case BLFUN_Min:
					 return L"Min";
				 case BLFUN_Max:
					 return L"Max";
				 }
				 return L"Add";
			 }

			 CullMode GraphicsCommonUtils::ParseCullMode(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);
				 if (v == L"none")
				 {
					 return CULL_None;
				 }
				 else if (v == L"clockwise")
				 {
					 return CULL_Clockwise;
				 }
				 else if (v == L"counterclockwise")
				 {
					 return CULL_CounterClockwise;
				 }
				 return CULL_None;
			 }
			 String GraphicsCommonUtils::ToString(CullMode cull)
			 {
				 switch (cull)
				 {
				 case CULL_Clockwise:
					 return L"Clockwise";
				 case CULL_CounterClockwise:
					 return L"CounterClockwise";
				 case CULL_None:
					 return L"None";
				 }
				 return L"None";
			 }



			 TextureAddressMode GraphicsCommonUtils::ParseTextureAddressMode(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);

				 if (v == L"wrap")
				 {
					 return TA_Wrap;
				 }
				 else if (v == L"mirror")
				 {
					 return TA_Mirror;
				 }
				 else if (v == L"clamp")
				 {
					 return TA_Clamp;
				 }
				 else if (v == L"border")
				 {
					 return TA_Border;
				 }
				 else if (v == L"mirroronce")
				 {
					 return TA_MirrorOnce;
				 }
				 return TA_Wrap;
			 }
			 String GraphicsCommonUtils::ToString(TextureAddressMode mode)
			 {
				 switch (mode)
				 {
				 case TA_Wrap:
					 return L"Wrap";
				 case TA_Mirror:
					 return L"Mirror";
				 case TA_Clamp:
					 return L"Clamp";
				 case TA_Border:
					 return L"Border";
				 case TA_MirrorOnce:
					 return L"MirrorOnce";
				 }
				 return L"Wrap";
			 }

			 TextureFilter GraphicsCommonUtils::ParseTextureFilter(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);

				 if (v == L"none")
				 {
					 return TFLT_None;
				 }
				 else if (v == L"point")
				 {
					 return TFLT_Point;
				 }
				 else if (v == L"linear")
				 {
					 return TFLT_Linear;
				 }
				 else if (v == L"anisotropic")
				 {
					 return TFLT_Anisotropic;
				 }
				 else if (v == L"pyramidalquad")
				 {
					 return TFLT_PyramidalQuad;
				 }
				 else if (v == L"gaussianquad")
				 {
					 return TFLT_GaussianQuad;
				 }
				 return TFLT_None;
			 }
			 
			 String GraphicsCommonUtils::ToString(TextureFilter filter)
			 {
				 switch (filter)
				 {
				 case TFLT_None:
					 return L"None";
				 case TFLT_Point:
					 return L"Point";
				 case TFLT_Linear:
					 return L"Linear";
				 case TFLT_Anisotropic:
					 return L"Anisotropic";
				 case TFLT_PyramidalQuad:
					 return L"PyramidalQuad";
				 case TFLT_GaussianQuad:
					 return L"GaussianQuad";
				 }
				 return L"None";
			 }

			 ShaderType GraphicsCommonUtils::ParseShaderType(const String& value)
			 {
				 String v = value;
				 StringUtils::ToLowerCase(v);

				 if (v==L"vertexshader")
				 {
					 return SHDT_Vertex;
				 }
				 else if (v==L"pixelshader")
				 {
					 return SHDT_Pixel;
				 }
				 return SHDT_Pixel;
			 }
			 String GraphicsCommonUtils::ToString(ShaderType type)
			 {
				 switch (type)
				 {
				 case SHDT_Pixel:
					 return L"PixelShader";
				 case SHDT_Vertex:
					 return L"VertexShader";
				 }
				 return L"VertexShader";
			 }
		}
	}
}