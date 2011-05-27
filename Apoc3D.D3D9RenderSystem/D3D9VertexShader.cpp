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

#include "D3D9VertexShader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			void D3D9VertexShader::Init(const char* code, int32 length)
			{

			}

			D3D9VertexShader(D3D9RenderDevice* device, const ResourceLocation* rl)
			{

			}
			~D3D9VertexShader()
			{

			}

			int D3D9VertexShader::GetContantIndex(const String& name)
			{

			}

			void D3D9VertexShader::SetVector2(int index, Vector2 value)
			{

			}
			void D3D9VertexShader::SetVector3(int index, Vector3 value)
			{

			}
			void D3D9VertexShader::SetVector4(int index, Vector3 value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, Quaternion value)  
			{

			}
			void D3D9VertexShader::SetValue(int index, const Matrix &value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, const Color4 &value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, Plane value) 
			{

			}

			void D3D9VertexShader::SetValueDirect(int reg, Vector3 value) 
			{

			}


			void D3D9VertexShader::SetVector2(int index, const Vector2* value, int count) 
			{

			}
			void D3D9VertexShader::SetVector3(int index, const Vector3* value, int count) 
			{

			}
			void D3D9VertexShader::SetVector4(int index, const Vector3* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(int index, const Quaternion* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(int index, const Matrix* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int index, const Color4* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int index, const Plane* value, int count) 
			{

			}

			void D3D9VertexShader::SetValueDirect(int reg, float value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, bool value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, float value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, int value) 
			{

			}
			void D3D9VertexShader::SetValue(int index, bool* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int index, float* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int index, int* value, int count) 
			{

			}

			void D3D9VertexShader::SetTexture(int index, Texture* tex) 
			{

			}
			void D3D9VertexShader::SetSamplerState(int index, const ShaderSamplerState &state) 
			{

			}

			void D3D9VertexShader::SetTextureDirect(int index, Texture* tex)
			{

			}
			void D3D9VertexShader::SetSamplerStateDirect(int index, const ShaderSamplerState &state) 
			{

			}

			void D3D9VertexShader::SetVector2(const String &paramName, Vector2 value) 
			{

			}
			void D3D9VertexShader::SetVector3(const String &paramName, Vector3 value) 
			{

			}
			void D3D9VertexShader::SetVector4(const String &paramName, Vector3 value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, Quaternion value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Matrix &value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, Color4 value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, Plane value)
			{

			}



			void D3D9VertexShader::SetVector2(const String &paramName, const Vector2* value, int count)
			{

			}
			void D3D9VertexShader::SetVector3(const String &paramName, const Vector3* value, int count)
			{

			}
			void D3D9VertexShader::SetVector4(const String &paramName, const Vector3* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Quaternion* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Matrix* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Plane* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Color4* value, int count)
			{

			}

			void D3D9VertexShader::SetValue(const String &paramName, bool value)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, float value)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, int value)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, bool* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, float* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, int* value, int count)
			{

			}

			void D3D9VertexShader::SetTexture(const String &paramName, Texture* tex)
			{

			}
			void D3D9VertexShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{

			}

			void D3D9VertexShader::AutoSetParameters(const Material* mtrl)
			{

			}
		}
	}
}