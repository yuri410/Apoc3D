#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#ifndef GL3SHADER_H
#define GL3SHADER_H

#include "GL3Common.h"

#include "apoc3d/Math/Math.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Math/Quaternion.h"

#include "apoc3d/Graphics/RenderSystem/Shader.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3Shader : public Shader
			{
			public:
				GL3Shader(GL3RenderDevice* device, GLenum shaderType, const byte* byteCode);
				GL3Shader(GL3RenderDevice* device, GLenum shaderType, const ResourceLocation& rl);
				virtual ~GL3Shader();

				int GetParamIndex(const String& paramName) override;
				int GetSamplerIndex(const String& paramName) override;
				bool TryGetParamIndex(const String& paramName, int32& result) override;
				bool TryGetSamplerIndex(const String& paramName, int32& result) override;

				void SetTexture(int samIndex, Texture* tex) override;
				void SetSamplerState(int samIndex, const ShaderSamplerState& state);

				void SetTexture(const String& paramName, Texture* tex);
				void SetSamplerState(const String& paramName, const ShaderSamplerState& state);


				void SetVector2(int32 reg, const Vector2& value) override { }
				void SetVector3(int32 reg, const Vector3& value) override { }
				void SetVector4(int32 reg, const Vector4& value) override { }
				void SetValue(int32 reg, const Quaternion& value) override { }
				void SetValue(int32 reg, const Matrix& value) override { }
				void SetValue(int32 reg, const Color4& value) override { }
				void SetValue(int32 reg, const Plane& value) override { }

				void SetVector2(int32 reg, const Vector2* value, int32 count) override { }
				void SetVector3(int32 reg, const Vector3* value, int32 count) override { }
				void SetVector4(int32 reg, const Vector4* value, int32 count) override { }
				void SetValue(int32 reg, const Quaternion* value, int32 count) override { }
				void SetValue(int32 reg, const Matrix* value, int32 count) override { }
				void SetValue(int32 reg, const Color4* value, int32 count) override { }
				void SetValue(int32 reg, const Plane* value, int32 count) override { }
				void SetMatrix4x3(int32 reg, const Matrix* value, int32 count) override { }

				void SetValue(int32 reg, bool value) override { }
				void SetValue(int32 reg, float value) override { }
				void SetValue(int32 reg, int32 value) override { }
				void SetValue(int32 reg, const bool* value, int32 count) override { }
				void SetValue(int32 reg, const float* value, int32 count) override { }
				void SetValue(int32 reg, const int32* value, int32 count) override { }

				void SetVector2(const String& paramName, const Vector2& value) override { }
				void SetVector3(const String& paramName, const Vector3& value) override { }
				void SetVector4(const String& paramName, const Vector4& value) override { }
				void SetValue(const String& paramName, const Quaternion& value) override { }
				void SetValue(const String& paramName, const Matrix& value) override { }
				void SetValue(const String& paramName, const Color4& value) override { }
				void SetValue(const String& paramName, const Plane& value) override { }

				void SetVector2(const String& paramName, const Vector2* value, int32 count) override { }
				void SetVector3(const String& paramName, const Vector3* value, int32 count) override { }
				void SetVector4(const String& paramName, const Vector4* value, int32 count) override { }
				void SetValue(const String& paramName, const Quaternion* value, int32 count) override { }
				void SetValue(const String& paramName, const Matrix* value, int32 count) override { }
				void SetValue(const String& paramName, const Plane* value, int32 count) override { }
				void SetValue(const String& paramName, const Color4* value, int32 count) override { }

				void SetValue(const String& paramName, bool value) override { }
				void SetValue(const String& paramName, float value) override { }
				void SetValue(const String& paramName, int32 value) override { }
				void SetValue(const String& paramName, const bool* value, int32 count) override { }
				void SetValue(const String& paramName, const float* value, int32 count) override { }
				void SetValue(const String& paramName, const int32* value, int32 count) override { }

				void NotifyLinkage(const List<Shader*>& shaders) override;

				GLuint getGLShaderID() const { return m_shaderID; }
			protected:
				static void KeyNotFoundError(const String& name);

				void CompileShader(GLenum shaderType, const byte* byteCode);

				GL3RenderDevice* m_device;
				GLuint m_shaderID = 0;

				GLProgram* m_prog = nullptr;
			};


			class GL3PixelShader : public GL3Shader
			{
			public:
				GL3PixelShader(GL3RenderDevice* device, const byte* byteCode);
				GL3PixelShader(GL3RenderDevice* device, const ResourceLocation& rl);
				~GL3PixelShader();

				virtual ShaderType getType() const { return ShaderType::Pixel; }
			};

			class GL3VertexShader : public GL3Shader
			{
			public:
				GL3VertexShader(GL3RenderDevice* device, const byte* byteCode);
				GL3VertexShader(GL3RenderDevice* device, const ResourceLocation& rl);
				~GL3VertexShader();

				virtual ShaderType getType() const { return ShaderType::Vertex; }
			};

		}
	}
}

#endif