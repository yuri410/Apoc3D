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

			void ShaderSamplerState::Parse(const ConfigurationSection* sect)
			{	
				sect->TryGetAttributeEnum(L"AddressU", AddressU, TextureAddressModeConverter);
				sect->TryGetAttributeEnum(L"AddressV", AddressV, TextureAddressModeConverter);
				sect->TryGetAttributeEnum(L"AddressW", AddressW, TextureAddressModeConverter);

				if (const String* v = sect->tryGetAttribute(L"BorderColor"))
					BorderColor = StringUtils::ParseUInt32Hex(*v);
				
				sect->TryGetAttributeEnum(L"MagFilter", MagFilter, TextureFilterConverter);
				sect->TryGetAttributeEnum(L"MinFilter", MinFilter, TextureFilterConverter);
				sect->TryGetAttributeEnum(L"MipFilter", MipFilter, TextureFilterConverter);


				sect->TryGetAttributeInt(L"MaxAnisotropy", MaxAnisotropy);
				sect->TryGetAttributeInt(L"MaxMipLevel", MaxMipLevel);
				sect->TryGetAttributeInt(L"MipMapLODBias", MipMapLODBias);
			}
			void ShaderSamplerState::Save(ConfigurationSection* sect)
			{
				sect->AddAttributeString(L"AddressU", TextureAddressModeConverter.ToString(AddressU));
				sect->AddAttributeString(L"AddressV", TextureAddressModeConverter.ToString(AddressU));
				sect->AddAttributeString(L"AddressW", TextureAddressModeConverter.ToString(AddressU));

				sect->AddAttributeString(L"MagFilter", TextureFilterConverter.ToString(MagFilter));
				sect->AddAttributeString(L"MinFilter", TextureFilterConverter.ToString(MinFilter));
				sect->AddAttributeString(L"MipFilter", TextureFilterConverter.ToString(MipFilter));

				sect->AddAttributeString(L"BorderColor", StringUtils::UIntToStringHex(BorderColor, StringUtils::SF_ShowHexBase));

				sect->AddAttributeString(L"MaxAnisotropy", StringUtils::IntToString(MaxAnisotropy));
				sect->AddAttributeString(L"MaxMipLevel", StringUtils::IntToString(MaxMipLevel));
				sect->AddAttributeString(L"MipMapLODBias", StringUtils::IntToString(MipMapLODBias));


			}
		}
	}
}