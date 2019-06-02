
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
#include "GL/GLProgram.h"
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
			}
			GL3Shader::~GL3Shader()
			{
				if (m_prog && m_prog->DecrRefCount())
				{
					delete m_prog;
				}
				m_prog = nullptr;

				if (m_shaderID)
				{
					glDeleteShader(m_shaderID);
					m_shaderID = 0;
				}
			}

			void GL3Shader::CompileShader(GLenum shaderType, const byte* byteCode)
			{
				int32 dataLength = mb_i32_le((const char*)byteCode);
				int32 sourceLength = mb_i32_le((const char*)byteCode + sizeof(int));
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

			int32 GL3Shader::GetParamIndex(const String& paramName)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv) return pv->m_location;
				KeyNotFoundError(paramName);
				return 0;
			}
			int32 GL3Shader::GetSamplerIndex(const String& paramName)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv)
					return pv->m_textureSlotID;
				KeyNotFoundError(paramName);
				return 0;
			}
			bool GL3Shader::TryGetParamIndex(const String& paramName, int32& result)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv)
				{
					result = pv->m_location;
					return true;
				}
				return false;
			}
			bool GL3Shader::TryGetSamplerIndex(const String& paramName, int32& result)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv && pv->m_isSampler)
				{
					result = pv->m_textureSlotID;
					return true;
				}
				return false;
			}

			void GL3Shader::KeyNotFoundError(const String& name)
			{
				Apoc3D::Core::ApocLog(LOG_Graphics, L"Shader parameter " + name + L" not found.", LOGLVL_Warning);
			}

			void GL3Shader::NotifyLinkage(const List<Shader*>& shaders)
			{
				GLProgram* prog = nullptr;

				int linkedShaderCount = 0;
				int unlinkedShaderCount = 0;
				for (Shader* s : shaders)
				{
					GL3Shader* gs = static_cast<GL3Shader*>(s);

					if (gs == this)
					{
						continue;
					}

					if (gs->m_prog)
					{
						linkedShaderCount++;
						prog = gs->m_prog;
					}
					else
					{
						unlinkedShaderCount++;
					}
				}

				assert(unlinkedShaderCount + linkedShaderCount + 1 == shaders.getCount());

				if (linkedShaderCount == 0)
				{
					List<GLuint> shaderIDs(shaders.getCount());
					for (Shader* s : shaders)
					{
						GL3Shader* gs = static_cast<GL3Shader*>(s);
						shaderIDs.Add(gs->m_shaderID);
					}

					prog = new GLProgram();
					prog->Link(shaderIDs);

					for (Shader* s : shaders)
					{
						GL3Shader* gs = static_cast<GL3Shader*>(s);
						gs->m_prog = prog;
						prog->IncrRefCount();
					}
				}
				else
				{
					for (Shader* s : shaders)
					{
						GL3Shader* gs = static_cast<GL3Shader*>(s);
						gs->m_prog = prog;
					}
				}
			}

			void GL3Shader::SetTexture(int slotIdx, Texture* tex)
			{
				m_device->getNativeState()->SetTexture(slotIdx, static_cast<GL3Texture*>(tex));
			}
			void GL3Shader::SetSamplerState(int slotIdx, const ShaderSamplerState& state)
			{
				m_device->getNativeState()->SetSampler(slotIdx, state);
			}

			void GL3Shader::SetTexture(const String& paramName, Texture* tex)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv)
					SetTexture(pv->m_textureSlotID, tex);
				else
					KeyNotFoundError(paramName);
			}
			void GL3Shader::SetSamplerState(const String& paramName, const ShaderSamplerState& state)
			{
				const GLProgramVariable* pv = m_prog->getUniform(paramName);
				if (pv)
					SetSamplerState(pv->m_textureSlotID, state);
				else
					KeyNotFoundError(paramName);
			}

			//////////////////////////////////////////////////////////////////////////

			GL3VertexShader::GL3VertexShader(GL3RenderDevice* device, const byte* byteCode)
				: GL3Shader(device, GL_VERTEX_SHADER, byteCode)
			{
			}
			GL3VertexShader::~GL3VertexShader()
			{
				
			}

			//////////////////////////////////////////////////////////////////////////

			GL3PixelShader::GL3PixelShader(GL3RenderDevice* device, const byte* byteCode)
				: GL3Shader(device, GL_FRAGMENT_SHADER, byteCode)
			{
			}
			GL3PixelShader::~GL3PixelShader()
			{
				
			}

		}
	}
}