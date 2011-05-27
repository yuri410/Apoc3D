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
#include "D3D9RenderDevice.h"

#include "IO/BinaryReader.h"
#include "Vfs/ResourceLocation.h"
#include "IO/Streams.h"
#include "ConstantTable.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			D3D9PixelShader::D3D9PixelShader(D3D9RenderDevice* device, const ResourceLocation* rl)
				: PixelShader(device), m_device(device)
			{
				BinaryReader* br = new BinaryReader(rl);
				int64 len = br->getBaseStream()->getLength();
				char* buffer = new char[len];
				br->ReadBytes(buffer, len);
				br->Close();
				delete br;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreatePixelShader(reinterpret_cast<const DWORD*>(buffer), &m_shader);
				assert(SUCCEEDED(hr));
				

				ID3DXConstantTable* constants;
				hr = D3DXGetShaderConstantTableEx(reinterpret_cast<const DWORD*>(buffer), 
					D3DXCONSTTABLE_LARGEADDRESSAWARE, &constants);
				assert(SUCCEEDED(hr));

				m_constantTable = new ConstantTable(buffer);

				delete[] buffer;



				constants->Release();
			}
			D3D9PixelShader::~D3D9PixelShader()
			{
				m_shader->Release();
			}


			void D3D9PixelShader::SetVector2(int reg, Vector2 value)
			{

			}
			void D3D9PixelShader::SetVector3(int reg, Vector3 value)
			{

			}
			void D3D9PixelShader::SetVector4(int reg, Vector4 value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Quaternion& value)  
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Matrix& value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Color4& value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Plane& value) 
			{

			}


			void D3D9PixelShader::SetVector2(int reg, const Vector2* value, int count) 
			{

			}
			void D3D9PixelShader::SetVector3(int reg, const Vector3* value, int count) 
			{

			}
			void D3D9PixelShader::SetVector4(int reg, const Vector4* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Quaternion* value, int count)
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Matrix* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Color4* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, const Plane* value, int count) 
			{

			}

			void D3D9PixelShader::SetValue(int reg, bool value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, float value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, int value) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, bool* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, float* value, int count) 
			{

			}
			void D3D9PixelShader::SetValue(int reg, int* value, int count) 
			{

			}

			void D3D9PixelShader::SetTexture(int reg, Texture* tex) 
			{

			}
			void D3D9PixelShader::SetSamplerState(int reg, const ShaderSamplerState &state) 
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