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

#include "D3D9RenderDevice.h"

#include "IO/BinaryReader.h"
#include "Vfs/ResourceLocation.h"
#include "IO/Streams.h"
#include "ConstantTable.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			D3D9VertexShader::D3D9VertexShader(D3D9RenderDevice* device, const ResourceLocation* rl)
				: VertexShader(device), m_device(device)
			{
				BinaryReader* br = new BinaryReader(rl);
				int64 len = br->getBaseStream()->getLength();
				char* buffer = new char[static_cast<uint>(len)];
				br->ReadBytes(buffer, len);
				br->Close();
				delete br;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreateVertexShader(reinterpret_cast<const DWORD*>(buffer), &m_shader);
				assert(SUCCEEDED(hr));
				

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(buffer));

				delete[] buffer;
			}
			D3D9VertexShader::~D3D9VertexShader()
			{

			}

			void D3D9VertexShader::SetVector2(int reg, Vector2 value)
			{

			}
			void D3D9VertexShader::SetVector3(int reg, Vector3 value)
			{

			}
			void D3D9VertexShader::SetVector4(int reg, Vector3 value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Quaternion& value)  
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Matrix& value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Color4& value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Plane& value) 
			{

			}



			void D3D9VertexShader::SetVector2(int reg, const Vector2* value, int count) 
			{

			}
			void D3D9VertexShader::SetVector3(int reg, const Vector3* value, int count) 
			{

			}
			void D3D9VertexShader::SetVector4(int reg, const Vector3* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Quaternion* value, int count)
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Matrix* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Color4* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, const Plane* value, int count) 
			{

			}

			void D3D9VertexShader::SetValue(int reg, bool value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, float value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, int value) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, bool* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, float* value, int count) 
			{

			}
			void D3D9VertexShader::SetValue(int reg, int* value, int count) 
			{

			}

			void D3D9VertexShader::SetTexture(int reg, Texture* tex) 
			{

			}
			void D3D9VertexShader::SetSamplerState(int reg, const ShaderSamplerState &state) 
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
			void D3D9VertexShader::SetValue(const String &paramName, const Quaternion& value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Matrix& value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Color4& value) 
			{

			}
			void D3D9VertexShader::SetValue(const String &paramName, const Plane& value)
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