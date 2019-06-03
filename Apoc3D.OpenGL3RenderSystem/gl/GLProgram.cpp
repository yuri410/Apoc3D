#include "GLProgram.h"
#include "../GL3Shader.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			bool GLProgramVariable::IsTextureType() const
			{
				switch (m_type)
				{
					case GL_IMAGE_1D: 
					case GL_IMAGE_2D: 
					case GL_IMAGE_3D: 
					case GL_IMAGE_2D_RECT: 
					case GL_IMAGE_CUBE: 
					case GL_IMAGE_BUFFER: 
					case GL_IMAGE_1D_ARRAY: 
					case GL_IMAGE_2D_ARRAY: 
					case GL_IMAGE_2D_MULTISAMPLE: 
					case GL_IMAGE_2D_MULTISAMPLE_ARRAY: 
					case GL_INT_IMAGE_1D: 
					case GL_INT_IMAGE_2D: 
					case GL_INT_IMAGE_3D: 
					case GL_INT_IMAGE_2D_RECT: 
					case GL_INT_IMAGE_CUBE: 
					case GL_INT_IMAGE_BUFFER: 
					case GL_INT_IMAGE_1D_ARRAY: 
					case GL_INT_IMAGE_2D_ARRAY: 
					case GL_INT_IMAGE_2D_MULTISAMPLE: 
					case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: 
					case GL_UNSIGNED_INT_IMAGE_1D: 
					case GL_UNSIGNED_INT_IMAGE_2D: 
					case GL_UNSIGNED_INT_IMAGE_3D: 
					case GL_UNSIGNED_INT_IMAGE_2D_RECT: 
					case GL_UNSIGNED_INT_IMAGE_CUBE: 
					case GL_UNSIGNED_INT_IMAGE_BUFFER: 
					case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: 
					case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: 
					case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
					case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
						return true;
				}
				return false;
			}
			
			bool GLProgramVariable::IsSamplerType() const
			{
				switch (m_type)
				{
					case GL_SAMPLER_1D:
					case GL_SAMPLER_2D:
					case GL_SAMPLER_3D:
					case GL_SAMPLER_CUBE:
					case GL_SAMPLER_1D_SHADOW:
					case GL_SAMPLER_2D_SHADOW:
					case GL_SAMPLER_1D_ARRAY:
					case GL_SAMPLER_2D_ARRAY:
					case GL_SAMPLER_1D_ARRAY_SHADOW:
					case GL_SAMPLER_2D_ARRAY_SHADOW:
					case GL_SAMPLER_2D_MULTISAMPLE:
					case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
					case GL_SAMPLER_CUBE_SHADOW:
					case GL_SAMPLER_BUFFER:
					case GL_SAMPLER_2D_RECT:
					case GL_SAMPLER_2D_RECT_SHADOW:
					case GL_INT_SAMPLER_1D:
					case GL_INT_SAMPLER_2D:
					case GL_INT_SAMPLER_3D:
					case GL_INT_SAMPLER_CUBE:
					case GL_INT_SAMPLER_1D_ARRAY:
					case GL_INT_SAMPLER_2D_ARRAY:
					case GL_INT_SAMPLER_2D_MULTISAMPLE:
					case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
					case GL_INT_SAMPLER_BUFFER:
					case GL_INT_SAMPLER_2D_RECT:
					case GL_UNSIGNED_INT_SAMPLER_1D:
					case GL_UNSIGNED_INT_SAMPLER_2D:
					case GL_UNSIGNED_INT_SAMPLER_3D:
					case GL_UNSIGNED_INT_SAMPLER_CUBE:
					case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
					case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
					case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
					case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
					case GL_UNSIGNED_INT_SAMPLER_BUFFER:
					case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
						return true;
				}
				return false;
			}


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

			GLProgram::GLProgram(GLProgram&& o)
				: m_prog(o.m_prog)
				, m_uniformTable(std::move(o.m_uniformTable))
				, m_refCount(o.m_refCount)
			{
				o.m_prog = 0;
			}

			GLProgram& GLProgram::operator=(GLProgram&& o)
			{
				if (this != &o)
				{
					this->~GLProgram();
					new (this)GLProgram(std::move(o));
				}
				return *this;
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

					GLuint slotId = 0;
					for (GLProgramVariable& var : m_uniformTable.getValueAccessor())
					{
						var.m_isSampler = var.IsSamplerType();
						var.m_isTexture = var.IsTextureType();

						if (var.m_isSampler || var.m_isTexture)
						{
							var.m_textureSlotID = slotId++;
						}
					}
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

						GLuint loc;
						if (uniform)
							loc = glGetUniformLocation(m_prog, varName);
						else
							loc = glGetAttribLocation(m_prog, varName);

						GLProgramVariable var;
						var.m_name = StringUtils::UTF8toUTF16(varName);
						var.m_type = varType;
						var.m_size = varSize;
						var.m_location = loc;

						if (var.m_location != -1)
						{
							if (uniform)
								m_uniformTable.Add(var.m_name, var);
							else
								m_attributes.Add(var);
						}
					}
				}
			}

			void GLProgram::BindSamplerSlots()
			{
				for (const GLProgramVariable& var : m_uniformTable.getValueAccessor())
				{
					if (var.m_isSampler)
						glUniform1i(var.m_location, var.m_textureSlotID);
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