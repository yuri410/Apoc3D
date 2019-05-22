#include "GLProgram.h"
#include "../GL3Shader.h"

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

			void GLProgram::Link(const List<Shader*>& shaders)
			{
				for (Shader* s : shaders)
				{
					GL3Shader* gs = static_cast<GL3Shader*>(s);

					glAttachShader(m_prog, gs->getGLShaderID());
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