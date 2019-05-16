
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "GL3Shader.h"
#include "GL3RenderStateManager.h"
#include "GL3RenderDevice.h"
#include "GL3Texture.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Library/lz4.h"
#include "Apoc3D/IOLib/IOUtils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3Shader::GL3Shader(GL3RenderDevice* device, GLenum shaderType, const byte* byteCode)
				: Shader(device), m_device(device)
			{
				CompileShader(shaderType, byteCode);
				m_constantTable = new ConstantTable(byteCode);
			}
			GL3Shader::GL3Shader(GL3RenderDevice* device, GLenum shaderType, const ResourceLocation& rl)
				: Shader(device), m_device(device)
			{
				BinaryReader br(rl);
				int64 len = br.getBaseStream()->getLength();
				char* buffer = new char[(uint)len];
				br.ReadBytes(buffer, len);

				CompileShader(shaderType, (byte*)buffer);

				m_constantTable = new ConstantTable(reinterpret_cast<const byte*>(buffer));

				delete[] buffer;
			}
			GL3Shader::~GL3Shader()
			{
				delete m_constantTable;
			}

			void GL3Shader::CompileShader(GLenum shaderType, const byte* byteCode)
			{
				int32 dataLength = ci32_le((const char*)byteCode);
				int32 sourceLength = ci32_le((const char*)byteCode + sizeof(int));
				const GLchar* sourceCode = new GLchar[sourceLength + 1]();

				int32 ret = LZ4_decompress_safe((const char*)byteCode + sizeof(int)*2, (char*)sourceCode, dataLength, sourceLength);
				assert(ret == sourceLength);

				m_shaderID = glCreateShader(shaderType);
				glShaderSource(m_shaderID, 1, &sourceCode, nullptr);
				glCompileShader(m_shaderID);

				GLint res;
				glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &res);
				if (res == GL_FALSE)
				{
					GLint res;
					glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &res);

					std::string errMsg;
					if (res > 0)
					{
						std::string infoLog(res + 1, '0');
						glGetShaderInfoLog(m_shaderID, res, &res, &infoLog[0]);
						errMsg = infoLog;
					}

					AP_EXCEPTION(ErrorID::Default, "Shader compilation failed: " + errMsg);
				}
			}

			// TODO: attributes and uniforms are only available after program linking

			int32 GL3Shader::GetParamIndex(const String& paramName)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc && sc->RegisterCount)
				{
					return sc->RegisterIndex;
				}
				KeyNotFoundError(paramName);
				return 0;
			}
			int32 GL3Shader::GetSamplerIndex(const String& paramName)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					return sc->SamplerIndex;
				KeyNotFoundError(paramName);
				return 0;
			}
			bool GL3Shader::TryGetParamIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc && !sc->IsSampler)
				{
					result = sc->RegisterIndex;
					return true;
				}
				return false;
			}
			bool GL3Shader::TryGetSamplerIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc && sc->IsSampler)
				{
					result = sc->SamplerIndex;
					return true;
				}
				return false;
			}

			void GL3Shader::KeyNotFoundError(const String& name)
			{
				Apoc3D::Core::ApocLog(LOG_Graphics, L"Shader parameter " + name + L" not found.", LOGLVL_Warning);
			}

			//////////////////////////////////////////////////////////////////////////

			GL3VertexShader::GL3VertexShader(GL3RenderDevice* device, const byte* byteCode)
				: GL3Shader(device, GL_VERTEX_SHADER, byteCode)
			{
			}
			GL3VertexShader::GL3VertexShader(GL3RenderDevice* device, const ResourceLocation& rl)
				: GL3Shader(device, GL_VERTEX_SHADER, rl)
			{

			}
			GL3VertexShader::~GL3VertexShader()
			{
				if (m_shaderID)
				{
					glDeleteShader(m_shaderID);
					m_shaderID = 0;
				}
			}

			void GL3VertexShader::SetTexture(int samIndex, Texture* tex)
			{

			}
			void GL3VertexShader::SetSamplerState(int samIndex, const ShaderSamplerState &state)
			{
				m_device->getNativeStateManager()->SetVertexSampler(samIndex, state);

			}

			void GL3VertexShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetTexture(sc->SamplerIndex, tex);
				else
					KeyNotFoundError(paramName);
			}
			void GL3VertexShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetSamplerState(sc->SamplerIndex, state);
				else
					KeyNotFoundError(paramName);
			}

			//////////////////////////////////////////////////////////////////////////

			GL3PixelShader::GL3PixelShader(GL3RenderDevice* device, const byte* byteCode)
				: GL3Shader(device, GL_FRAGMENT_SHADER, byteCode)
			{
			}
			GL3PixelShader::GL3PixelShader(GL3RenderDevice* device, const ResourceLocation& rl)
				: GL3Shader(device, GL_FRAGMENT_SHADER, rl)
			{

			}
			GL3PixelShader::~GL3PixelShader()
			{
				if (m_shaderID)
				{
					glDeleteShader(m_shaderID);
					m_shaderID = 0;
				}
			}

			void GL3PixelShader::SetTexture(int samIndex, Texture* tex)
			{
				m_device->getNativeStateManager()->SetTexture(samIndex, static_cast<NRSTexture*>(tex));
			}
			void GL3PixelShader::SetSamplerState(int samIndex, const ShaderSamplerState &state)
			{
				m_device->getNativeStateManager()->SetPixelSampler(samIndex, state);
			}

			void GL3PixelShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetTexture(sc->SamplerIndex, tex);
				else
					KeyNotFoundError(paramName);
			}
			void GL3PixelShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetSamplerState(sc->SamplerIndex, state);
				else
					KeyNotFoundError(paramName);
			}

		}
	}
}