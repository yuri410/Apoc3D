#pragma once
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
#ifndef D3D9PIXELSHADER_H
#define D3D9PIXELSHADER_H

#include "D3D9Common.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Graphics/RenderSystem/Shader.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9PixelShader : public PixelShader
			{
			public:
				IDirect3DPixelShader9* getD3DPS() const { return m_shader; }

				D3D9PixelShader(D3D9RenderDevice* device, const byte* byteCode);
				D3D9PixelShader(D3D9RenderDevice* device, const ResourceLocation* rl);
				~D3D9PixelShader();

				virtual int GetParamIndex(const String& paramName);
				virtual int GetSamplerIndex(const String& paramName);

				inline virtual void SetVector2(int reg, Vector2 value);
				inline virtual void SetVector3(int reg, Vector3 value);
				inline virtual void SetVector4(int reg, Vector4 value);
				inline virtual void SetValue(int reg, const Quaternion& value);
				inline virtual void SetValue(int reg, const Matrix& value);
				inline virtual void SetValue(int reg, const Color4& value);
				inline virtual void SetValue(int reg, const Plane& value);

				inline virtual void SetVector2(int reg, const Vector2* value, int count);
				inline virtual void SetVector3(int reg, const Vector3* value, int count);
				inline virtual void SetVector4(int reg, const Vector4* value, int count);
				inline virtual void SetValue(int reg, const Quaternion* value, int count);
				inline virtual void SetValue(int reg, const Matrix* value, int count);
				inline virtual void SetValue(int reg, const Color4* value, int count);
				inline virtual void SetValue(int reg, const Plane* value, int count);
				virtual void SetMatrix4x3(int reg, const Matrix* value, int count);
					 
				virtual void SetValue(int reg, bool value);
				virtual void SetValue(int reg, float value);
				virtual void SetValue(int reg, int value);
				virtual void SetValue(int reg, const bool* value, int count);
				virtual void SetValue(int reg, const float* value, int count);
				virtual void SetValue(int reg, const int* value, int count);

				virtual void SetTexture(int samIndex, Texture* tex);
				virtual void SetSamplerState(int samIndex, const ShaderSamplerState &state);


				virtual void SetVector2(const String& paramName, Vector2 value);
				virtual void SetVector3(const String& paramName, Vector3 value);
				virtual void SetVector4(const String& paramName, Vector4 value);
				virtual void SetValue(const String& paramName, const Quaternion& value);
				virtual void SetValue(const String& paramName, const Matrix& value);
				virtual void SetValue(const String& paramName, const Color4& value);
				virtual void SetValue(const String& paramName, const Plane& value);

				virtual void SetVector2(const String& paramName, const Vector2* value, int count);
				virtual void SetVector3(const String& paramName, const Vector3* value, int count);
				virtual void SetVector4(const String& paramName, const Vector4* value, int count);
				virtual void SetValue(const String& paramName, const Quaternion* value, int count);
				virtual void SetValue(const String& paramName, const Matrix* value, int count);
				virtual void SetValue(const String& paramName, const Plane* value, int count);
				virtual void SetValue(const String& paramName, const Color4* value, int count);

				virtual void SetValue(const String& paramName, bool value);
				virtual void SetValue(const String& paramName, float value);
				virtual void SetValue(const String& paramName, int value);
				virtual void SetValue(const String& paramName, const bool* value, int count);
				virtual void SetValue(const String& paramName, const float* value, int count);
				virtual void SetValue(const String& paramName, const int* value, int count);

				virtual void SetTexture(const String& paramName, Texture* tex);
				virtual void SetSamplerState(const String& paramName, const ShaderSamplerState &state);

				virtual void AutoSetParameters(const Material* mtrl);

			private:
				NoInline static void ThrowKeyNotFoundEx(const String& name);

				float m_buffer[192];
				int m_intBuffer[32];
				BOOL m_boolBuffer[8];

				D3D9RenderDevice* m_device;
				IDirect3DPixelShader9* m_shader;

				ConstantTable* m_constantTable;
			};
		}
	}
}

#endif