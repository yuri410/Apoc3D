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

#include "Shader.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Shader::Shader(RenderDevice* rd)
				: m_renderDevice(rd)
			{
				
			}

			VertexShader::VertexShader(RenderDevice* rd)
				: Shader(rd)
			{

			}

			PixelShader::PixelShader(RenderDevice* rd)
				: Shader(rd)
			{

			}

			ShaderSamplerState::ShaderSamplerState()
				: AddressU(TA_Wrap), AddressV(TA_Wrap), AddressW(TA_Wrap),
				BorderColor(0), MagFilter(TFLT_Point), MinFilter(TFLT_Point), MipFilter(TFLT_None), MaxAnisotropy(1), MaxMipLevel(0), MipMapLODBias(0)
			{

			}
			void ShaderSamplerState::Parse(const ConfigurationSection* sect)
			{
				String v = GraphicsCommonUtils::ToString(TA_Wrap);

				sect->tryGetAttribute(L"AddressU", v);
				AddressU = GraphicsCommonUtils::ParseTextureAddressMode(v);

				v = GraphicsCommonUtils::ToString(TA_Wrap);
				sect->tryGetAttribute(L"AddressV", v);
				AddressV = GraphicsCommonUtils::ParseTextureAddressMode(v);

				v = GraphicsCommonUtils::ToString(TA_Wrap);
				sect->tryGetAttribute(L"AddressW", v);
				AddressW = GraphicsCommonUtils::ParseTextureAddressMode(v);

				v = L"0x00000000";
				sect->tryGetAttribute(L"BorderColor", v);
				BorderColor = StringUtils::ParseUInt32Hex(v);
				
				v = GraphicsCommonUtils::ToString(TFLT_Point);
				sect->tryGetAttribute(L"MagFilter", v);
				MagFilter = GraphicsCommonUtils::ParseTextureFilter(v);

				v = GraphicsCommonUtils::ToString(TFLT_Point);
				sect->tryGetAttribute(L"MinFilter", v);
				MinFilter = GraphicsCommonUtils::ParseTextureFilter(v);

				v = GraphicsCommonUtils::ToString(TFLT_None);
				sect->tryGetAttribute(L"MipFilter", v);
				MipFilter = GraphicsCommonUtils::ParseTextureFilter(v);


				MaxAnisotropy = 1;
				sect->TryGetAttributeInt(L"MaxAnisotropy", MaxAnisotropy);

				MaxMipLevel = 0;
				sect->TryGetAttributeInt(L"MaxMipLevel", MaxMipLevel);

				MipMapLODBias = 0;
				sect->TryGetAttributeInt(L"MipMapLODBias", MipMapLODBias);
			}
			void ShaderSamplerState::Save(ConfigurationSection* sect)
			{
				sect->AddAttributeString(L"AddressU", GraphicsCommonUtils::ToString(AddressU));
				sect->AddAttributeString(L"AddressV", GraphicsCommonUtils::ToString(AddressU));
				sect->AddAttributeString(L"AddressW", GraphicsCommonUtils::ToString(AddressU));

				sect->AddAttributeString(L"MagFilter", GraphicsCommonUtils::ToString(MagFilter));
				sect->AddAttributeString(L"MinFilter", GraphicsCommonUtils::ToString(MinFilter));
				sect->AddAttributeString(L"MipFilter", GraphicsCommonUtils::ToString(MipFilter));

				sect->AddAttributeString(L"BorderColor", StringUtils::UIntToStringHex(BorderColor));

				sect->AddAttributeString(L"MaxAnisotropy", StringUtils::IntToString(MaxAnisotropy));
				sect->AddAttributeString(L"MaxMipLevel", StringUtils::IntToString(MaxMipLevel));
				sect->AddAttributeString(L"MipMapLODBias", StringUtils::IntToString(MipMapLODBias));


			}
		}
	}
}