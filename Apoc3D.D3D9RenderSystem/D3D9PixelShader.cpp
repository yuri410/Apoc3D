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

#include "D3D9PixelShader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			void D3D9PixelShader::Init(const char* code, int32 length)
			{

			}

			D3D9PixelShader(D3D9RenderDevice* device, const ResourceLocation* rl)
			{

			}
			~D3D9PixelShader()
			{

			}

			int D3D9PixelShader::GetContantIndex(const String& name)
			{

			}

			void D3D9PixelShader::SetVector2(int index, Vector2 value)
			{

			}
			void D3D9PixelShader::SetVector3(int index, Vector3 value)
			{

			}
			void D3D9PixelShader::SetVector4(int index, Vector4 value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, const Quaternion& value)  
			{

			}
			void D3D9PixelShader::SetValue(int index, const Matrix& value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, const Color4& value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, const Plane& value) 
			{

			}

			void D3D9PixelShader::SetValueDirect(int reg, Vector3 value) 
			{

			}


			void D3D9PixelShader::SetVector2(int index, const Vector2* value, int count) 
			{

			}
			void D3D9PixelShader::SetVector3(int index, const Vector3* value, int count) 
			{

			}
			void D3D9PixelShader::SetVector4(int index, const Vector4* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(int index, const Quaternion* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(int index, const Matrix* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int index, const Color4* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int index, const Plane* value, int count) 
			{

			}

			void D3D9PixelShader::SetValueDirect(int reg, float value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, bool value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, float value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, int value) 
			{

			}
			void D3D9PixelShader::SetValue(int index, bool* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int index, float* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int index, int* value, int count) 
			{

			}

			void D3D9PixelShader::SetTexture(int index, Texture* tex) 
			{

			}
			void D3D9PixelShader::SetSamplerState(int index, const ShaderSamplerState &state) 
			{

			}

			void D3D9PixelShader::SetTextureDirect(int index, Texture* tex)
			{

			}
			void D3D9PixelShader::SetSamplerStateDirect(int index, const ShaderSamplerState &state) 
			{

			}

			void D3D9PixelShader::SetVector2(const String &paramName, Vector2 value) 
			{

			}
			void D3D9PixelShader::SetVector3(const String &paramName, Vector3 value) 
			{

			}
			void D3D9PixelShader::SetVector4(const String &paramName, Vector4 value) 
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Quaternion& value) 
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Matrix& value) 
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Color4& value) 
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Plane& value)
			{

			}



			void D3D9PixelShader::SetVector2(const String &paramName, const Vector2* value, int count)
			{

			}
			void D3D9PixelShader::SetVector3(const String &paramName, const Vector3* value, int count)
			{

			}
			void D3D9PixelShader::SetVector4(const String &paramName, const Vector4* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Quaternion* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Matrix* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Plane* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, const Color4* value, int count)
			{

			}

			void D3D9PixelShader::SetValue(const String &paramName, bool value)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, float value)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, int value)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, bool* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, float* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(const String &paramName, int* value, int count)
			{

			}

			void D3D9PixelShader::SetTexture(const String &paramName, Texture* tex)
			{

			}
			void D3D9PixelShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{

			}

			void D3D9PixelShader::AutoSetParameters(const Material* mtrl)
			{

			}
		}
	}
}