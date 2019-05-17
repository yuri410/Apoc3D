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
			void Shader::NotifyLinkage(void* platformData)
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