#pragma once
#ifndef APOC3D_SHADER_H
#define APOC3D_SHADER_H

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

#include "apoc3d/Math/Vector.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** Contains the parameters required for texture samplers. */
			struct APAPI ShaderSamplerState 
			{
				TextureAddressMode AddressU = TextureAddressMode::Wrap;
				TextureAddressMode AddressV = TextureAddressMode::Wrap;
				TextureAddressMode AddressW = TextureAddressMode::Wrap;

				uint BorderColor = 0;
				TextureFilter MagFilter = TextureFilter::Point;
				TextureFilter MinFilter = TextureFilter::Point;
				TextureFilter MipFilter = TextureFilter::None;
				int32 MaxAnisotropy = 1;
				int32 MaxMipLevel = 0;
				int32 MipMapLODBias = 0;

				ShaderSamplerState() { }
				void Parse(const ConfigurationSection* sect);
				void Save(ConfigurationSection* sect);
			};

			class APAPI Shader
			{
			public:
				RenderDevice* getRenderDevice() { return m_renderDevice; }

				virtual int32 GetParamIndex(const String& paramName) = 0;
				virtual int32 GetSamplerIndex(const String& paramName) = 0;
				virtual bool TryGetParamIndex(const String& paramName, int& result) = 0;
				virtual bool TryGetSamplerIndex(const String& paramName, int& result) = 0;

				virtual void SetVector2(int reg, const Vector2& value) = 0;
				virtual void SetVector3(int reg, const Vector3& value) = 0;
				virtual void SetVector4(int reg, const Vector4& value) = 0;
				virtual void SetValue(int reg, const Quaternion& value) = 0;
				virtual void SetValue(int reg, const Matrix& value) = 0;
				virtual void SetValue(int reg, const Color4& value) = 0;
				virtual void SetValue(int reg, const Plane& value) = 0;

				virtual void SetVector2(int reg, const Vector2* value, int count) = 0;
				virtual void SetVector3(int reg, const Vector3* value, int count) = 0;
				virtual void SetVector4(int reg, const Vector4* value, int count) = 0;
				virtual void SetValue(int reg, const Quaternion* value, int count) = 0;
				virtual void SetValue(int reg, const Matrix* value, int count) = 0;
				virtual void SetValue(int reg, const Color4* value, int count) = 0;
				virtual void SetValue(int reg, const Plane* value, int count) = 0;
				virtual void SetMatrix4x3(int reg, const Matrix* value, int count) = 0;

				virtual void SetValue(int reg, bool value) = 0;
				virtual void SetValue(int reg, float value) = 0;
				virtual void SetValue(int reg, int value) = 0;
				virtual void SetValue(int reg, const bool* value, int count) = 0;
				virtual void SetValue(int reg, const float* value, int count) = 0;
				virtual void SetValue(int reg, const int* value, int count) = 0;

				virtual void SetTexture(int samIndex, Texture* tex) = 0;
				virtual void SetSamplerState(int samIndex, const ShaderSamplerState &state) = 0;


				virtual void SetVector2(const String& paramName, const Vector2& value) = 0;
				virtual void SetVector3(const String& paramName, const Vector3& value) = 0;
				virtual void SetVector4(const String& paramName, const Vector4& value) = 0;
				virtual void SetValue(const String& paramName, const Quaternion& value) = 0;
				virtual void SetValue(const String& paramName, const Matrix& value) = 0;
				virtual void SetValue(const String& paramName, const Color4& value) = 0;
				virtual void SetValue(const String& paramName, const Plane& value) = 0;



				virtual void SetVector2(const String& paramName, const Vector2* value, int count) = 0;
				virtual void SetVector3(const String& paramName, const Vector3* value, int count) = 0;
				virtual void SetVector4(const String& paramName, const Vector4* value, int count) = 0;
				virtual void SetValue(const String& paramName, const Quaternion* value, int count) = 0;
				virtual void SetValue(const String& paramName, const Matrix* value, int count) = 0;
				virtual void SetValue(const String& paramName, const Plane* value, int count) = 0;
				virtual void SetValue(const String& paramName, const Color4* value, int count) = 0;

				virtual void SetValue(const String& paramName, bool value) = 0;
				virtual void SetValue(const String& paramName, float value) = 0;
				virtual void SetValue(const String& paramName, int value) = 0;
				virtual void SetValue(const String& paramName, const bool* value, int count) = 0;
				virtual void SetValue(const String& paramName, const float* value, int count) = 0;
				virtual void SetValue(const String& paramName, const int* value, int count) = 0;

				virtual void SetTexture(const String& paramName, Texture* tex) = 0;
				virtual void SetSamplerState(const String& paramName, const ShaderSamplerState& state) = 0;

				virtual ~Shader() {}

				virtual ShaderType getType() const = 0;
			protected:
				Shader(RenderDevice* rd);

			private:
				RenderDevice* m_renderDevice;

			};

			
		}
	}
}
#endif