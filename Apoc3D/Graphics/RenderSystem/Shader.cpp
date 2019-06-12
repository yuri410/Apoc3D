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
#include "apoc3d/Core/Logging.h"

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

			void Shader::NotifyLinkage(const List<Shader*>& shaders)
			{

			}

			int32 Shader::GetParamIndex(const String& paramName)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					return idx;
				KeyNotFoundError(paramName);
				return 0;
			}

			int32 Shader::GetSamplerIndex(const String& paramName)
			{
				int32 idx;
				if (TryGetSamplerIndex(paramName, idx))
					return idx;
				KeyNotFoundError(paramName);
				return 0;
			}

			void Shader::SetVector2(const String& paramName, const Vector2& value)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector2(idx, value);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetVector3(const String& paramName, const Vector3& value)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector3(idx, value);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetVector4(const String& paramName, const Vector4& value)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector4(idx, value);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetValue(const String& paramName, const Quaternion& value) { SetValueGeneric(paramName, value); }
			void Shader::SetValue(const String& paramName, const Matrix& value)		{ SetValueGeneric(paramName, value); }
			void Shader::SetValue(const String& paramName, const Color4& value)		{ SetValueGeneric(paramName, value); }
			void Shader::SetValue(const String& paramName, const Plane& value)		{ SetValueGeneric(paramName, value); }

			void Shader::SetVector2(const String& paramName, const Vector2* value, int32 count)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector2(idx, value, count);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetVector3(const String& paramName, const Vector3* value, int32 count)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector3(idx, value, count);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetVector4(const String& paramName, const Vector4* value, int32 count)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetVector4(idx, value, count);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetValue(const String& paramName, const Quaternion* value, int32 count){ SetValueGeneric(paramName, value, count); }
			void Shader::SetValue(const String& paramName, const Matrix* value, int32 count)	{ SetValueGeneric(paramName, value, count); }
			void Shader::SetValue(const String& paramName, const Plane* value, int32 count)		{ SetValueGeneric(paramName, value, count); }
			void Shader::SetValue(const String& paramName, const Color4* value, int32 count)	{ SetValueGeneric(paramName, value, count); }

			void Shader::SetValue(const String& paramName, bool value)  { SetValueGeneric(paramName, value); }
			void Shader::SetValue(const String& paramName, float value) { SetValueGeneric(paramName, value); }
			void Shader::SetValue(const String& paramName, int32 value) { SetValueGeneric(paramName, value); }

			void Shader::SetValue(const String& paramName, const bool* value, int32 count)	{ SetValueGeneric(paramName, value, count); }
			void Shader::SetValue(const String& paramName, const float* value, int32 count)	{ SetValueGeneric(paramName, value, count); }
			void Shader::SetValue(const String& paramName, const int32* value, int32 count)	{ SetValueGeneric(paramName, value, count); }

			void Shader::SetTexture(const String& paramName, Texture* tex)
			{
				int32 idx;
				if (TryGetSamplerIndex(paramName, idx))
					SetTexture(idx, tex);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::SetSamplerState(const String& paramName, const ShaderSamplerState& state)
			{
				int32 idx;
				if (TryGetSamplerIndex(paramName, idx))
					SetSamplerState(idx, state);
				else
					KeyNotFoundError(paramName);
			}


			template <typename T>
			void Shader::SetValueGeneric(const String& paramName, const T& value)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetValue(idx, value);
				else
					KeyNotFoundError(paramName);
			}

			template <typename T>
			void Shader::SetValueGeneric(const String& paramName, const T* value, int32 count)
			{
				int32 idx;
				if (TryGetParamIndex(paramName, idx))
					SetValue(idx, value, count);
				else
					KeyNotFoundError(paramName);
			}

			void Shader::KeyNotFoundError(const String& name)
			{
				Apoc3D::Core::ApocLog(LOG_Graphics, L"Shader parameter " + name + L" not found.", LOGLVL_Warning);
			}

			//////////////////////////////////////////////////////////////////////////

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