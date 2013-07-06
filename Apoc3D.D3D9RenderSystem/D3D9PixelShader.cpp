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

#include "D3D9PixelShader.h"
#include "D3D9RenderDevice.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/ApocException.h"
#include "apoc3d/Utility/StringUtils.h"

#include "ConstantTable.h"
#include "D3D9Texture.h"
#include "D3D9Utils.h"
#include "D3D9RenderStateManager.h"


using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9PixelShader::D3D9PixelShader(D3D9RenderDevice* device, const byte* byteCode)
				: PixelShader(device), m_device(device)
			{
				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreatePixelShader(reinterpret_cast<const DWORD*>(byteCode), &m_shader);
				assert(SUCCEEDED(hr));

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(byteCode));

			}
			D3D9PixelShader::D3D9PixelShader(D3D9RenderDevice* device, const ResourceLocation* rl)
				: PixelShader(device), m_device(device)
			{
				BinaryReader* br = new BinaryReader(rl);
				int64 len = br->getBaseStream()->getLength();
				char* buffer = new char[static_cast<uint>(len)];
				br->ReadBytes(buffer, len);
				br->Close();
				delete br;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreatePixelShader(reinterpret_cast<const DWORD*>(buffer), &m_shader);
				assert(SUCCEEDED(hr));
				

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(buffer));

				delete[] buffer;
			}
			D3D9PixelShader::~D3D9PixelShader()
			{
				m_shader->Release();
			}

			int D3D9PixelShader::GetParamIndex(const String& paramName)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				return cons.RegisterIndex;
			}
			int D3D9PixelShader::GetSamplerIndex(const String& paramName)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				return cons.SamplerIndex[0];
			}

			void D3D9PixelShader::SetVector2(int reg, const Vector2& value)
			{
				m_buffer[0] = value.X;
				m_buffer[1] = value.Y;
				m_buffer[2] = m_buffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);
			}
			void D3D9PixelShader::SetVector3(int reg, const Vector3& value)
			{
				m_buffer[0] = value.X;
				m_buffer[1] = value.Y;
				m_buffer[2] = value.Z;
				m_buffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);

			}
			void D3D9PixelShader::SetVector4(int reg, const Vector4& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, value, 1);
			}
			void D3D9PixelShader::SetValue(int reg, const Quaternion& value)  
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.X, 1);
			}
			void D3D9PixelShader::SetValue(int reg, const Matrix& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.M11, 4);
			}
			void D3D9PixelShader::SetValue(int reg, const Color4& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.Red, 1);
			}
			void D3D9PixelShader::SetValue(int reg, const Plane& value) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, &value.X, 1);
			}


			void D3D9PixelShader::SetVector2(int reg, const Vector2* value, int count) 
			{
				while (count > 8)
				{
					for (int i=0;i<8;i++)
					{
						int ofs = i*4;
						//Vector2Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 8);
					count -= 8;
					value += 8;
				}
				while (count > 4)
				{
					for (int i=0;i<4;i++)
					{
						int ofs = i*4;
						//Vector2Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 4);
					count -= 4;
					value += 4;
				}
				if (count>0)
				{
					for (int i=0;i<count;i++)
					{
						int ofs = i*4;
						//Vector2Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, count);
				}
				
			}
			void D3D9PixelShader::SetVector3(int reg, const Vector3* value, int count) 
			{
				while (count > 8)
				{
					for (int i=0;i<8;i++)
					{
						int ofs = i*4;
						//Vector3Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = value[i].Z;
						m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 8);
					count -= 8;
					value += 8;
				}
				while (count > 4)
				{
					for (int i=0;i<4;i++)
					{
						int ofs = i*4;
						//Vector3Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = value[i].Z;
						m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 4);
					count -= 4;
					value += 4;
				}
				if (count>0)
				{
					for (int i=0;i<count;i++)
					{
						int ofs = i*4;
						//Vector3Utils::Store(value[i], &m_buffer[ofs]);
						m_buffer[ofs] = value[i].X;
						m_buffer[ofs+1] = value[i].Y;
						m_buffer[ofs+2] = value[i].Z;
						m_buffer[ofs+3] = 0;
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, count);
				}
				
			}
			void D3D9PixelShader::SetVector4(int reg, const Vector4* value, int count)
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9PixelShader::SetValue(int reg, const Quaternion* value, int count)
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9PixelShader::SetValue(int reg, const Matrix* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count * 4);
			}
			void D3D9PixelShader::SetMatrix4x3(int reg, const Matrix* value, int count)
			{
				while (count > 16)
				{
					for (int i=0;i<16;i++)
					{
						int ofs = i*12;
						memcpy(m_buffer+ofs, value+i, sizeof(float)*12);
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 16 * 3);
					count -= 16;
					value += 16;
				}
				if (count>0)
				{
					for (int i=0;i<count;i++)
					{
						int ofs = i*12;
						memcpy(m_buffer+ofs, value+i, sizeof(float)*12);
					}
					m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, count * 3);
				}
			}
			void D3D9PixelShader::SetValue(int reg, const Color4* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}
			void D3D9PixelShader::SetValue(int reg, const Plane* value, int count) 
			{
				m_device->getDevice()->SetPixelShaderConstantF(reg, reinterpret_cast<const float*>(value), count);
			}

			void D3D9PixelShader::SetValue(int reg, bool value) 
			{
				const BOOL b = value ? 1 : 0;
				m_device->getDevice()->SetPixelShaderConstantB(reg, &b, 1);
			}
			void D3D9PixelShader::SetValue(int reg, float value) 
			{
				m_buffer[0] = value;
				m_buffer[1] = m_buffer[2] = m_buffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(reg, m_buffer, 1);
			}
			void D3D9PixelShader::SetValue(int reg, int value) 
			{
				m_intBuffer[0] = value;
				m_intBuffer[1] = m_intBuffer[2] = m_intBuffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantI(reg, m_intBuffer, 1);
			}
			void D3D9PixelShader::SetValue(int reg, const bool* value, int count) 
			{
				if ((count%4))
				{
					if (count == 1)
					{
						SetValue(reg, *value);
						return;
					}
					throw AP_EXCEPTION(EX_Argument, L"count");
				}
				for (int i=0;i<count;i++)
				{
					m_boolBuffer[i] = value ? 1 : 0;
				}
				m_device->getDevice()->SetPixelShaderConstantB(reg, m_boolBuffer, count / 4);
			}
			void D3D9PixelShader::SetValue(int reg, const float* value, int count) 
			{
				if ((count%4))
				{
					switch (count)
					{
					case 1:
						SetValue(reg, *value);
						return;
					case 2:
						SetVector2(reg, *reinterpret_cast<const Vector2*>(value));
						return;
					case 3:
						SetVector3(reg, *reinterpret_cast<const Vector3*>(value));
						return;
					}
					
					throw AP_EXCEPTION(EX_Argument, L"count");
				}
				m_device->getDevice()->SetPixelShaderConstantF(reg, value, count / 4);
			}
			void D3D9PixelShader::SetValue(int reg, const int* value, int count) 
			{
				if ((count%4))
				{
					if (count == 1)
					{
						SetValue(reg, *value);
						return;
					}
					throw AP_EXCEPTION(EX_Argument, L"count");
				}
				m_device->getDevice()->SetPixelShaderConstantI(reg, value, count / 4);
			}

			void D3D9PixelShader::SetTexture(int samIndex, Texture* tex) 
			{
				m_device->getNativeStateManager()->SetTexture(samIndex, static_cast<D3D9Texture*>(tex));
			}
			void D3D9PixelShader::SetSamplerState(int samIndex, const ShaderSamplerState &state) 
			{
				m_device->getNativeStateManager()->SetPixelSampler(samIndex, state);
			}


			void D3D9PixelShader::SetVector2(const String &paramName, const Vector2& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				m_buffer[0] = value.X;
				m_buffer[1] = value.Y;
				m_buffer[2] = m_buffer[3] = 0;

				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, m_buffer, 1);
			}
			void D3D9PixelShader::SetVector3(const String &paramName, const Vector3& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				//Vector3Utils::Store(value, m_buffer);
				m_buffer[0] = value.X;
				m_buffer[1] = value.Y;
				m_buffer[2] = value.Z;
				m_buffer[3] = 0;
				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, m_buffer, 1);
			}
			void D3D9PixelShader::SetVector4(const String &paramName, const Vector4& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				m_device->getDevice()->SetPixelShaderConstantF(cons.RegisterIndex, value, 1);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Quaternion& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Matrix& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Color4& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Plane& value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}



			void D3D9PixelShader::SetVector2(const String &paramName, const Vector2* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector2(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetVector3(const String &paramName, const Vector3* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector3(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetVector4(const String &paramName, const Vector4* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector4(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Quaternion* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Matrix* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Plane* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const Color4* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}

			void D3D9PixelShader::SetValue(const String &paramName, bool value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, float value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, int value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const bool* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const float* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			void D3D9PixelShader::SetValue(const String &paramName, const int* value, int count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}

			void D3D9PixelShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetTexture(cons.SamplerIndex[0], tex);
			}
			void D3D9PixelShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetSamplerState(cons.SamplerIndex[0], state);
			}
			
			void D3D9PixelShader::ThrowKeyNotFoundEx(const String& name)
			{
				throw AP_EXCEPTION(EX_KeyNotFound, name);
			}
		}
	}
}