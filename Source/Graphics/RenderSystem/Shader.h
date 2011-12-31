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
#ifndef SHADER_H
#define SHADER_H

#include "Common.h"
#include "Math\Vector.h"
#include "Math\Color.h"
#include "Graphics\GraphicsCommon.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			struct APAPI ShaderSamplerState 
			{
				TextureAddressMode AddressU;
				TextureAddressMode AddressV;
				TextureAddressMode AddressW;

				uint BorderColor;
				TextureFilter MagFilter;
				TextureFilter MinFilter;
				TextureFilter MipFilter;
				int MaxAnisotropy;
				int MaxMipLevel;
				uint32 MipMapLODBias;

				void Parse(ConfigurationSection* sect);
			};
			class APAPI Shader
			{
			private:
				RenderDevice* m_renderDevice;
			protected:
				Shader(RenderDevice* rd);

			public:
				RenderDevice* getRenderDevice() { return m_renderDevice; }

				virtual int GetParamIndex(const String& paramName) = 0;
				virtual int GetSamplerIndex(const String& paramName) = 0;

				virtual void SetVector2(int reg, Vector2 value) = 0;
				virtual void SetVector3(int reg, Vector3 value) = 0;
				virtual void SetVector4(int reg, Vector4 value) = 0;
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
				virtual void SetValue(int reg, bool* value, int count) = 0;
				virtual void SetValue(int reg, float* value, int count) = 0;
				virtual void SetValue(int reg, int* value, int count) = 0;

				virtual void SetTexture(int samIndex, Texture* tex) = 0;
				virtual void SetSamplerState(int samIndex, const ShaderSamplerState &state) = 0;


				virtual void SetVector2(const String& paramName, Vector2 value) = 0;
				virtual void SetVector3(const String& paramName, Vector3 value) = 0;
				virtual void SetVector4(const String& paramName, Vector4 value) = 0;
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
				virtual void SetValue(const String& paramName, bool* value, int count) = 0;
				virtual void SetValue(const String& paramName, float* value, int count) = 0;
				virtual void SetValue(const String& paramName, int* value, int count) = 0;

				virtual void SetTexture(const String& paramName, Texture* tex) = 0;
				virtual void SetSamplerState(const String& paramName, const ShaderSamplerState& state) = 0;

				/** This method is obsolete
				*/
				virtual void AutoSetParameters(const Material* mtrl) = 0;
			};

			class APAPI VertexShader : public Shader
			{
			protected:
				VertexShader(RenderDevice* rd);
			};
			class APAPI PixelShader : public Shader
			{
			protected:
				PixelShader(RenderDevice* rd);
			};
		}
	}
}
#endif