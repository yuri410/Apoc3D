#include "GLProgram.h"
#include "../GL3Shader.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GLProgram::GLProgram()
			{
				m_prog = glCreateProgram();
			}

			GLProgram::~GLProgram()
			{
				if (m_prog)
				{
					glDeleteProgram(m_prog);
					m_prog = 0;
				}
			}

			void GLProgram::Link(const List<GLuint>& shaders)
			{
				for (GLuint s : shaders)
				{
					glAttachShader(m_prog, s);
				}

				glLinkProgram(m_prog);

				GLint res;
				glGetProgramiv(m_prog, GL_LINK_STATUS, &res);

				if (res == GL_FALSE)
				{
					glGetProgramiv(m_prog, GL_INFO_LOG_LENGTH, &res);

					std::string errMsg;
					if (res > 0)
					{
						std::string infoLog(res + 1, '0');
						glGetProgramInfoLog(m_prog, res, &res, &infoLog[0]);
						errMsg = infoLog;
					}

					AP_EXCEPTION(ErrorID::Default, "Program link failed: " + errMsg);
				}
				else
				{
					RetireveVariables(true);
					RetireveVariables(false);
				}
			}

			void GLProgram::RetireveVariables(bool uniform)
			{
				GLint varCount = 0;
				glGetProgramiv(m_prog, uniform ? GL_ACTIVE_UNIFORMS : GL_ACTIVE_ATTRIBUTES, &varCount);

				for (GLint i = 0; i < varCount; i++)
				{
					GLchar varName[256];
					GLint  varNameLength = 0;
					GLint  varSize;
					GLenum varType;

					if (uniform)
						glGetActiveUniform(m_prog, i, sizeof(varName), &varNameLength, &varSize, &varType, varName);
					else
						glGetActiveAttrib(m_prog, i, sizeof(varName), &varNameLength, &varSize, &varType, varName);

					if (varNameLength > 0 && varNameLength < sizeof(varName))
					{
						varName[varNameLength] = 0;

						glGetUniformLocation(m_prog, varName);

						GLProgramVariable var;
						var.m_name = StringUtils::UTF8toUTF16(varName);
						var.m_type = varType;
						var.m_size = varSize;
						var.m_location = glGetUniformLocation(m_prog, varName);

						if (var.m_location != -1)
						{
							m_uniformTable.Add(var.m_name, var);
						}
					}
				}
			}

			void GLProgram::IncrRefCount()
			{
				m_refCount++;
			}

			bool GLProgram::DecrRefCount()
			{
				m_refCount--;
				return m_refCount <= 0;
			}
		}
	}
}