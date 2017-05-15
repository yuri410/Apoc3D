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
#ifndef D3D9HADER_H
#define D3D9HADER_H

#include "D3D9Common.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Math/Quaternion.h"

#include "apoc3d/Graphics/RenderSystem/Shader.h"
#include "apoc3d/Exception.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "ConstantTable.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			const int SharedBufferSize = 1024; // reaching max possible num of scalars in constants in shader model 3

			extern float SharedFloatBuffer[];
			extern int SharedIntBuffer[];
			extern BOOL SharedBoolBuffer[];

#define GST \
			template <typename APIShader, \
				HRESULT (__stdcall IDirect3DDevice9::*CreateShader)(const DWORD*, APIShader**), \
				HRESULT (__stdcall IDirect3DDevice9::*SetConstantF)(UINT, const float*, UINT), \
				HRESULT (__stdcall IDirect3DDevice9::*SetConstantI)(UINT, const int*, UINT), \
				HRESULT (__stdcall IDirect3DDevice9::*SetConstantB)(UINT, const BOOL*, UINT) \
			>

			
			GST class D3D9Shader : public Shader
			{
			public:
				D3D9Shader(D3D9RenderDevice* device, const byte* byteCode);
				D3D9Shader(D3D9RenderDevice* device, const ResourceLocation& rl);
				virtual ~D3D9Shader();

				int GetParamIndex(const String& paramName) override;
				int GetSamplerIndex(const String& paramName) override;
				bool TryGetParamIndex(const String& paramName, int& result) override;
				bool TryGetSamplerIndex(const String& paramName, int& result) override;


				void SetVector2(int32 reg, const Vector2& value) override;
				void SetVector3(int32 reg, const Vector3& value) override;
				void SetVector4(int32 reg, const Vector4& value) override;
				void SetValue(int32 reg, const Quaternion& value) override;
				void SetValue(int32 reg, const Matrix& value) override;
				void SetValue(int32 reg, const Color4& value) override;
				void SetValue(int32 reg, const Plane& value) override;

				void SetVector2(int32 reg, const Vector2* value, int32 count) override;
				void SetVector3(int32 reg, const Vector3* value, int32 count) override;
				void SetVector4(int32 reg, const Vector4* value, int32 count) override;
				void SetValue(int32 reg, const Quaternion* value, int32 count) override;
				void SetValue(int32 reg, const Matrix* value, int32 count) override;
				void SetValue(int32 reg, const Color4* value, int32 count) override;
				void SetValue(int32 reg, const Plane* value, int32 count) override;
				void SetMatrix4x3(int32 reg, const Matrix* value, int32 count) override;

				void SetValue(int32 reg, bool value) override;
				void SetValue(int32 reg, float value) override;
				void SetValue(int32 reg, int32 value) override;
				void SetValue(int32 reg, const bool* value, int32 count) override;
				void SetValue(int32 reg, const float* value, int32 count) override;
				void SetValue(int32 reg, const int32* value, int32 count) override;

				void SetVector2(const String& paramName, const Vector2& value) override;
				void SetVector3(const String& paramName, const Vector3& value) override;
				void SetVector4(const String& paramName, const Vector4& value) override;
				void SetValue(const String& paramName, const Quaternion& value) override;
				void SetValue(const String& paramName, const Matrix& value) override;
				void SetValue(const String& paramName, const Color4& value) override;
				void SetValue(const String& paramName, const Plane& value) override;

				void SetVector2(const String& paramName, const Vector2* value, int32 count) override;
				void SetVector3(const String& paramName, const Vector3* value, int32 count) override;
				void SetVector4(const String& paramName, const Vector4* value, int32 count) override;
				void SetValue(const String& paramName, const Quaternion* value, int32 count) override;
				void SetValue(const String& paramName, const Matrix* value, int32 count) override;
				void SetValue(const String& paramName, const Plane* value, int32 count) override;
				void SetValue(const String& paramName, const Color4* value, int32 count) override;

				void SetValue(const String& paramName, bool value) override;
				void SetValue(const String& paramName, float value) override;
				void SetValue(const String& paramName, int32 value) override;
				void SetValue(const String& paramName, const bool* value, int32 count) override;
				void SetValue(const String& paramName, const float* value, int32 count) override;
				void SetValue(const String& paramName, const int32* value, int32 count) override;

			protected:
				NO_INLINE static void ThrowKeyNotFoundEx(const String& name)
				{
					throw AP_EXCEPTION(ExceptID::KeyNotFound, name);
				}

				D3D9RenderDevice* m_device;
				APIShader* m_shader;

				ConstantTable* m_constantTable;
			};

#define ShaderTT D3D9Shader<APIShader, CreateShader, SetConstantF, SetConstantI, SetConstantB>

			GST ShaderTT::D3D9Shader(D3D9RenderDevice* device, const byte* byteCode)
				: Shader(device), m_device(device)
			{
				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = (dev->*CreateShader)(reinterpret_cast<const DWORD*>(byteCode), &m_shader);
				assert(SUCCEEDED(hr));

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(byteCode));
			}
			GST ShaderTT::D3D9Shader(D3D9RenderDevice* device, const ResourceLocation& rl)
				: Shader(device), m_device(device)
			{
				BinaryReader br(rl);
				int64 len = br.getBaseStream()->getLength();
				char* buffer = new char[static_cast<uint>(len)];
				br.ReadBytes(buffer, len);


				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = (dev->*CreateShader)(reinterpret_cast<const DWORD*>(buffer), &m_shader);
				assert(SUCCEEDED(hr));

				m_constantTable = new ConstantTable(reinterpret_cast<const DWORD*>(buffer));

				delete[] buffer;
			}
			GST ShaderTT::~D3D9Shader()
			{
				m_shader->Release();
				delete m_constantTable;
			}



			GST int32 ShaderTT::GetParamIndex(const String& paramName)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				return cons.RegisterIndex;
			}
			GST int32 ShaderTT::GetSamplerIndex(const String& paramName)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				return cons.SamplerIndex;
			}
			GST bool ShaderTT::TryGetParamIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->tryGetConstant(paramName);
				if (sc && !sc->IsSampler)
				{
					result = sc->RegisterIndex;
					return true;
				}
				return false;
			}			
			GST bool ShaderTT::TryGetSamplerIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->tryGetConstant(paramName);
				if (sc && sc->IsSampler)
				{
					result = sc->SamplerIndex;
					return true;
				}
				return false;
			}

			
			GST void ShaderTT::SetVector2(int32 reg, const Vector2& value)
			{
				SharedFloatBuffer[0] = value.X;
				SharedFloatBuffer[1] = value.Y;
				SharedFloatBuffer[2] = SharedFloatBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, 1);
			}
			GST void ShaderTT::SetVector3(int32 reg, const Vector3& value)
			{
				SharedFloatBuffer[0] = value.X;
				SharedFloatBuffer[1] = value.Y;
				SharedFloatBuffer[2] = value.Z;
				SharedFloatBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, 1);
			}
			GST void ShaderTT::SetVector4(int32 reg, const Vector4& value)
			{
				(m_device->getDevice()->*SetConstantF)(reg, value, 1);
			}
			GST void ShaderTT::SetValue(int32 reg, const Quaternion& value)
			{
				(m_device->getDevice()->*SetConstantF)(reg, &value.X, 1);
			}
			GST void ShaderTT::SetValue(int32 reg, const Matrix& value)
			{
				(m_device->getDevice()->*SetConstantF)(reg, &value.M11, 4);
			}
			GST void ShaderTT::SetValue(int32 reg, const Color4& value)
			{
				(m_device->getDevice()->*SetConstantF)(reg, &value.Red, 1);
			}
			GST void ShaderTT::SetValue(int32 reg, const Plane& value)
			{
				(m_device->getDevice()->*SetConstantF)(reg, &value.X, 1);
			}

			
			GST void ShaderTT::SetVector2(int32 reg, const Vector2* value, int32 count)
			{
				assert(count<=SharedBufferSize/4);

				for (int i=0;i<count;i++)
				{
					int ofs = i*4;
					SharedFloatBuffer[ofs] = value[i].X;
					SharedFloatBuffer[ofs+1] = value[i].Y;
					SharedFloatBuffer[ofs+2] = SharedFloatBuffer[ofs+3] = 0;
				}
				(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, count);
			}
			GST void ShaderTT::SetVector3(int32 reg, const Vector3* value, int32 count)
			{
				assert(count<=SharedBufferSize/4);

				for (int i=0;i<count;i++)
				{
					int ofs = i*4;
					SharedFloatBuffer[ofs] = value[i].X;
					SharedFloatBuffer[ofs+1] = value[i].Y;
					SharedFloatBuffer[ofs+2] = value[i].Z;
					SharedFloatBuffer[ofs+3] = 0;
				}
				(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, count);
			}
			GST void ShaderTT::SetVector4(int32 reg, const Vector4* value, int32 count)
			{
				(m_device->getDevice()->*SetConstantF)(reg, (const float*)value, count);
			}
			GST void ShaderTT::SetValue(int32 reg, const Quaternion* value, int32 count)
			{
				(m_device->getDevice()->*SetConstantF)(reg, (const float*)value, count);
			}
			GST void ShaderTT::SetValue(int32 reg, const Matrix* value, int32 count)
			{
				(m_device->getDevice()->*SetConstantF)(reg, (const float*)value, count * 4);
			}
			GST void ShaderTT::SetMatrix4x3(int32 reg, const Matrix* value, int32 count)
			{
				assert(count*12<=SharedBufferSize);

				if (count>0)
				{
					for (int i=0;i<count;i++)
					{
						int ofs = i*12;
						memcpy(SharedFloatBuffer+ofs, value+i, sizeof(float)*12);
					}
					(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, count * 3);
				}
			}
			GST void ShaderTT::SetValue(int32 reg, const Color4* value, int32 count)
			{
				(m_device->getDevice()->*SetConstantF)(reg, (const float*)value, count);
			}
			GST void ShaderTT::SetValue(int32 reg, const Plane* value, int32 count)
			{
				(m_device->getDevice()->*SetConstantF)(reg, (const float*)value, count);
			}

			
			GST void ShaderTT::SetValue(int32 reg, bool value)
			{
				const BOOL b = value ? 1 : 0;
				(m_device->getDevice()->*SetConstantB)(reg, &b, 1);
			}
			GST void ShaderTT::SetValue(int32 reg, float value)
			{
				SharedFloatBuffer[0] = value;
				SharedFloatBuffer[1] = SharedFloatBuffer[2] = SharedFloatBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, 1);
			}
			GST void ShaderTT::SetValue(int32 reg, int32 value)
			{
				SharedIntBuffer[0] = value;
				SharedIntBuffer[1] = SharedIntBuffer[2] = SharedIntBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantI)(reg, SharedIntBuffer, 1);
			}
			
			GST void ShaderTT::SetValue(int32 reg, const bool* value, int32 count)
			{
				assert(count<=SharedBufferSize);

				// all must be converted
				for (int i=0;i<count;i++)
					SharedBoolBuffer[i] = value[i];

				int32 alignedCount = (count + 3) & ~0x03;

				for (int i=count;i<alignedCount;i++)
					SharedBoolBuffer[i] = 0;

				(m_device->getDevice()->*SetConstantB)(reg, SharedBoolBuffer, alignedCount / 4);
			}
			
			GST void ShaderTT::SetValue(int32 reg, const float* value, int32 count)
			{
				int regCount = count / 4;
				if (regCount>0)
					(m_device->getDevice()->*SetConstantF)(reg, value, regCount);

				// set the remaining
				count = count % 4;
				if (count)
				{
					value += regCount * 4;
					reg += regCount;

					for (int i=0;i<count;i++)
						SharedFloatBuffer[i] = value[i];

					for (int i=count;i<4;i++)
						SharedFloatBuffer[i] = 0;

					(m_device->getDevice()->*SetConstantF)(reg, SharedFloatBuffer, 1);
				}
			}
			
			GST void ShaderTT::SetValue(int32 reg, const int* value, int32 count)
			{
				int regCount = count/4;
				if (regCount>0)
					(m_device->getDevice()->*SetConstantI)(reg, value, regCount);

				// set the remaining
				count = count % 4;
				if (count)
				{
					reg += regCount;
					value += regCount * 4;

					for (int i=0;i<count;i++)
						SharedIntBuffer[i] = value[i];

					for (int i=count;i<4;i++)
						SharedIntBuffer[i] = 0;

					(m_device->getDevice()->*SetConstantI)(reg, SharedIntBuffer, 1);
				}
			}
			

			
			GST void ShaderTT::SetVector2(const String &paramName, const Vector2& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SharedFloatBuffer[0] = value.X;
				SharedFloatBuffer[1] = value.Y;
				SharedFloatBuffer[2] = SharedFloatBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantF)(cons.RegisterIndex, SharedFloatBuffer, 1);
			}
			
			GST void ShaderTT::SetVector3(const String &paramName, const Vector3& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SharedFloatBuffer[0] = value.X;
				SharedFloatBuffer[1] = value.Y;
				SharedFloatBuffer[2] = value.Z;
				SharedFloatBuffer[3] = 0;
				(m_device->getDevice()->*SetConstantF)(cons.RegisterIndex, SharedFloatBuffer, 1);
			}
			
			GST void ShaderTT::SetVector4(const String &paramName, const Vector4& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				(m_device->getDevice()->*SetConstantF)(cons.RegisterIndex, value, 1);
			}
			
			GST void ShaderTT::SetValue(const String &paramName, const Quaternion& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			
			GST void ShaderTT::SetValue(const String &paramName, const Matrix& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			
			GST void ShaderTT::SetValue(const String &paramName, const Color4& value) 
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			
			GST void ShaderTT::SetValue(const String &paramName, const Plane& value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}


			
			GST void ShaderTT::SetVector2(const String &paramName, const Vector2* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector2(cons.RegisterIndex, value, count);
			}
			
			GST void ShaderTT::SetVector3(const String &paramName, const Vector3* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector3(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetVector4(const String &paramName, const Vector4* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetVector4(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const Quaternion* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const Matrix* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const Plane* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const Color4* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}

			GST void ShaderTT::SetValue(const String &paramName, bool value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			GST void ShaderTT::SetValue(const String &paramName, float value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			GST void ShaderTT::SetValue(const String &paramName, int32 value)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value);
			}
			GST void ShaderTT::SetValue(const String &paramName, const bool* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const float* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}
			GST void ShaderTT::SetValue(const String &paramName, const int32* value, int32 count)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				if (!cons.RegisterCount)
				{
					ThrowKeyNotFoundEx(paramName);
				}
				SetValue(cons.RegisterIndex, value, count);
			}

#undef ShaderTT
#undef GST

		}
	}
}

#endif