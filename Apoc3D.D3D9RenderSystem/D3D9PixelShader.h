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
#ifndef D3D9PIXELSHADER_H
#define D3D9PIXELSHADER_H

#include "D3D9Common.h"
#include "Math/Math.h"
#include "Graphics/RenderSystem/Shader.h"

using namespace std;
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
			private:
				unordered_map<String, int> m_constIndexTable;

				void Init(const char* code, int32 length);
			public:
				D3D9PixelShader(D3D9RenderDevice* device, const ResourceLocation* rl);
				~D3D9PixelShader();

				virtual int GetContantIndex(const String& name) inline;

				virtual void SetVector2(int index, Vector2 value) inline;
				virtual void SetVector3(int index, Vector3 value)  inline;
				virtual void SetVector4(int index, Vector3 value)  inline;
				virtual void SetValue(int index, Quaternion value)  inline;
				virtual void SetValue(int index, const Matrix &value) inline;
				virtual void SetValue(int index, const Color4 &value) inline;
				virtual void SetValue(int index, Plane value) inline;

				virtual void SetValueDirect(int reg, Vector3 value) inline;


				virtual void SetVector2(int index, const Vector2* value, int count) inline;
				virtual void SetVector3(int index, const Vector3* value, int count) inline;
				virtual void SetVector4(int index, const Vector3* value, int count) inline;
				virtual void SetValue(int index, const Quaternion* value, int count) inline;
				virtual void SetValue(int index, const Matrix* value, int count) inline;
				virtual void SetValue(int index, const Color4* value, int count) inline;
				virtual void SetValue(int index, const Plane* value, int count) inline;

				virtual void SetValueDirect(int reg, float value) inline;
				virtual void SetValue(int index, bool value) inline;
				virtual void SetValue(int index, float value) inline;
				virtual void SetValue(int index, int value) inline;
				virtual void SetValue(int index, bool* value, int count) inline;
				virtual void SetValue(int index, float* value, int count) inline;
				virtual void SetValue(int index, int* value, int count) inline;

				virtual void SetTexture(int index, Texture* tex) inline;
				virtual void SetSamplerState(int index, const ShaderSamplerState &state) inline;

				virtual void SetTextureDirect(int index, Texture* tex) inline;
				virtual void SetSamplerStateDirect(int index, const ShaderSamplerState &state) inline;

				virtual void SetVector2(const String &paramName, Vector2 value) inline;
				virtual void SetVector3(const String &paramName, Vector3 value) inline;
				virtual void SetVector4(const String &paramName, Vector3 value) inline;
				virtual void SetValue(const String &paramName, Quaternion value) inline;
				virtual void SetValue(const String &paramName, const Matrix &value) inline;
				virtual void SetValue(const String &paramName, Color4 value) inline;
				virtual void SetValue(const String &paramName, Plane value) inline;



				virtual void SetVector2(const String &paramName, const Vector2* value, int count);
				virtual void SetVector3(const String &paramName, const Vector3* value, int count);
				virtual void SetVector4(const String &paramName, const Vector3* value, int count);
				virtual void SetValue(const String &paramName, const Quaternion* value, int count);
				virtual void SetValue(const String &paramName, const Matrix* value, int count);
				virtual void SetValue(const String &paramName, const Plane* value, int count);
				virtual void SetValue(const String &paramName, const Color4* value, int count);

				virtual void SetValue(const String &paramName, bool value);
				virtual void SetValue(const String &paramName, float value);
				virtual void SetValue(const String &paramName, int value);
				virtual void SetValue(const String &paramName, bool* value, int count);
				virtual void SetValue(const String &paramName, float* value, int count);
				virtual void SetValue(const String &paramName, int* value, int count);

				virtual void SetTexture(const String &paramName, Texture* tex);
				virtual void SetSamplerState(const String &paramName, const ShaderSamplerState &state);

				virtual void AutoSetParameters(const Material* mtrl);
			};
		}
	}
}

#endif