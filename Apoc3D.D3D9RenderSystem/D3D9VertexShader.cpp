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

#include "IOLib/BinaryReader.h"
#include "Vfs/ResourceLocation.h"
#include "IOLib/Streams.h"
#include "ConstantTable.h"
#include "D3D9Utils.h"
#include "D3D9Texture.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9VertexShader::D3D9VertexShader(D3D9RenderDevice* device, const byte* byteCode)
				: VertexShader(device), m_device(device)
			{
				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreateVertexShader(reinterpret_cast<const DWORD*>(byteCode), &m_shader);
				assert(SUCCEEDED(hr));

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(byteCode));
			}
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
				m_shader->Release();
			}

			void D3D9VertexShader::SetVector2(int reg, Vector2 value)
			{
				Vector2Utils::Store(value, m_buffer);
				m_buffer[3] = m_buffer[4] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);
			}
			void D3D9VertexShader::SetVector3(int reg, Vector3 value)
			{
				Vector3Utils::Store(value, m_buffer);
				m_buffer[4] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);
			}
			void D3D9VertexShader::SetVector4(int reg, Vector4 value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, Vector4Utils::GetElementAddress(value), 1);
			}
			void D3D9VertexShader::SetValue(int reg, const Quaternion& value)  
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.X, 1);
			}
			void D3D9VertexShader::SetValue(int reg, const Matrix& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.M11, 4);
			}
			void D3D9VertexShader::SetValue(int reg, const Color4& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.Red, 1);
			}
			void D3D9VertexShader::SetValue(int reg, const Plane& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.X, 1);
			}



			void D3D9VertexShader::SetVector2(int reg, const Vector2* value, int count) 
			{
				while (count > 8)
				{
					for (int i=0;i<8;i++)
					{
						int ofs = i*4;
						Vector2Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs+3] = m_buffer[ofs+4] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 8);
					count -= 8;
				}
				while (count > 4)
				{
					for (int i=0;i<4;i++)
					{
						int ofs = i*4;
						Vector2Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs+3] = m_buffer[ofs+4] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 4);
					count -= 4;
				}
				for (int i=0;i<count;i++)
				{
					int ofs = i*4;
					Vector2Utils::Store(value[i], &m_buffer[ofs]);
					m_buffer[ofs+3] = m_buffer[ofs+4] = 0;
				}
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, count);
			}
			void D3D9VertexShader::SetVector3(int reg, const Vector3* value, int count) 
			{
				while (count > 8)
				{
					for (int i=0;i<8;i++)
					{
						int ofs = i*4;
						Vector3Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs+4] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 8);
					count -= 8;
				}
				while (count > 4)
				{
					for (int i=0;i<4;i++)
					{
						int ofs = i*4;
						Vector3Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs+4] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 4);
					count -= 4;
				}
				for (int i=0;i<count;i++)
				{
					int ofs = i*4;
					Vector3Utils::Store(value[i], &m_buffer[ofs]);
					m_buffer[ofs+4] = 0;
				}
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, count);
			}
			void D3D9VertexShader::SetVector4(int reg, const Vector4* value, int count)
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9VertexShader::SetValue(int reg, const Quaternion* value, int count)
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9VertexShader::SetValue(int reg, const Matrix* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count * 4);
			}
			void D3D9VertexShader::SetValue(int reg, const Color4* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9VertexShader::SetValue(int reg, const Plane* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}

			void D3D9VertexShader::SetValue(int reg, bool value) 
			{
				const BOOL b = value ? 1 : 0;
				m_device->getDevice()->SetPixelShaderConstantB(reg, &b, 1);
			}
			void D3D9VertexShader::SetValue(int reg, float value) 
			{
				m_buffer[0] = value;
				m_buffer[1] = m_buffer[2] = m_buffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);
			}
			void D3D9VertexShader::SetValue(int reg, int value) 
			{
				m_intBuffer[0] = value;
				m_intBuffer[1] = m_intBuffer[2] = m_intBuffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantI(reg, m_intBuffer, 1);
			}
			void D3D9VertexShader::SetValue(int reg, bool* value, int count) 
			{
				if ((count%4))
				{
					throw Apoc3DException::createException(EX_Argument, L"count");
				}
				for (int i=0;i<count;i++)
				{
					m_boolBuffer[i] = value ? 1 : 0;
				}
				m_device->getDevice()->SetPixelShaderConstantB(reg, m_boolBuffer, count / 4);
			}
			void D3D9VertexShader::SetValue(int reg, float* value, int count) 
			{
				if ((count%4))
				{
					throw Apoc3DException::createException(EX_Argument, L"count");
				}
				m_device->getDevice()->SetPixelShaderConstantF(reg, value, count / 4);
			}
			void D3D9VertexShader::SetValue(int reg, int* value, int count) 
			{
				if ((count%4))
				{
					throw Apoc3DException::createException(EX_Argument, L"count");
				}
				m_device->getDevice()->SetPixelShaderConstantI(reg, value, count / 4);
			}

			void D3D9VertexShader::SetTexture(int samIndex, Texture* tex) 
			{
				D3DBaseTexture* value = 0;
				if (tex)
				{
					if (tex->getState() == RS_Loaded || !tex->isManaged())
					{
						D3D9Texture* d3dTex = static_cast<D3D9Texture*>(tex);
						if (d3dTex->getInternal2D())
						{
							value = d3dTex->getInternal2D();
						}
						else if (d3dTex->getInternalCube())
						{
							value = d3dTex->getInternalCube();
						}
						else if (d3dTex->getInternal3D())
						{
							value = d3dTex->getInternalCube();
						}
					}
				}
				m_device->getDevice()->SetTexture(samIndex, value);
			}
			void D3D9VertexShader::SetSamplerState(int samIndex, const ShaderSamplerState &state) 
			{
				D3DDevice* dev = m_device->getDevice();

				dev->SetSamplerState(samIndex, D3DSAMP_ADDRESSU, D3D9Utils::ConvertTextureAddress(state.AddressU));
				dev->SetSamplerState(samIndex, D3DSAMP_ADDRESSV, D3D9Utils::ConvertTextureAddress(state.AddressV));
				dev->SetSamplerState(samIndex, D3DSAMP_ADDRESSW, D3D9Utils::ConvertTextureAddress(state.AddressW));

				dev->SetSamplerState(samIndex, D3DSAMP_BORDERCOLOR, state.BorderColor);
				dev->SetSamplerState(samIndex, D3DSAMP_MAGFILTER, D3D9Utils::ConvertTextureFilter(state.MagFilter));
				dev->SetSamplerState(samIndex, D3DSAMP_MINFILTER, D3D9Utils::ConvertTextureFilter(state.MinFilter));
				dev->SetSamplerState(samIndex, D3DSAMP_MIPFILTER, D3D9Utils::ConvertTextureFilter(state.MipFilter));

				dev->SetSamplerState(samIndex, D3DSAMP_MAXANISOTROPY, state.MaxAnisotropy);
				dev->SetSamplerState(samIndex, D3DSAMP_MAXMIPLEVEL, state.MaxMipLevel);
				dev->SetSamplerState(samIndex, D3DSAMP_MIPMAPLODBIAS, state.MipMapLODBias);

			}


			void D3D9VertexShader::SetVector2(const String &paramName, Vector2 value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				Vector2Utils::Store(value, m_buffer);
				m_buffer[3] = m_buffer[4] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, m_buffer, 1);
			}
			void D3D9VertexShader::SetVector3(const String &paramName, Vector3 value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				Vector3Utils::Store(value, m_buffer);
				m_buffer[4] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, m_buffer, 1);
			}
			void D3D9VertexShader::SetVector4(const String &paramName, Vector4 value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, Vector4Utils::GetElementAddress(value), 1);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Quaternion& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Matrix& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Color4& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Plane& value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}



			void D3D9VertexShader::SetVector2(const String &paramName, const Vector2* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetVector2(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetVector3(const String &paramName, const Vector3* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetVector3(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetVector4(const String &paramName, const Vector4* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetVector4(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Quaternion* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Matrix* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Plane* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, const Color4* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}

			void D3D9VertexShader::SetValue(const String &paramName, bool value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, float value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, int value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9VertexShader::SetValue(const String &paramName, bool* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, float* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9VertexShader::SetValue(const String &paramName, int* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (cons.RegisterCount)
				{
					throw Apoc3DException::createException(EX_KeyNotFound, paramName.c_str());
				}
				SetValue(cons.RegisterIndex, value, count);
			}

			void D3D9VertexShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetTexture(cons.SamplerIndex[0], tex);
			}
			void D3D9VertexShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetSamplerState(cons.SamplerIndex[0], state);
			}

			void D3D9VertexShader::AutoSetParameters(const Material* mtrl)
			{

			}
		}
	}
}