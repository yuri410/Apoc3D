#include "GLSampler.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GLint glGetSamplerParameter(GLuint unit, GLenum pname)
			{
				GLint r;
				glGetSamplerParameteriv(unit, pname, &r);
				return r;
			}

			GLSampler::GLSampler()
			{
				glGenSamplers(1, &m_sampler);
			}

			GLSampler::~GLSampler()
			{
				if (m_bound)
				{
					glBindSampler(m_unit, 0);
				}

				if (m_sampler)
				{
					glDeleteSamplers(1, &m_sampler);
					m_sampler = 0;
				}
			}

			void GLSampler::Bind(GLuint unit)
			{
				m_unit = unit;
				m_bound = true;
				glBindSampler(unit, m_sampler);
			}

			//////////////////////////////////////////////////////////////////////////

			void GLSampler::SetAddressModeS(GLenum mode)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_WRAP_S, mode);
			}

			void GLSampler::SetAddressModeT(GLenum mode)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_WRAP_T, mode);
			}

			void GLSampler::SetAddressModeR(GLenum mode)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_WRAP_R, mode);
			}
			
			void GLSampler::SetMinFilter(GLenum mode)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_MIN_FILTER, mode);
			}

			void GLSampler::SetMagFilter(GLenum mode)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_MAG_FILTER, mode);
			}

			void GLSampler::SetBorderColor(const GLfloat(&color)[4])
			{
				glSamplerParameterfv(m_unit, GL_TEXTURE_LOD_BIAS, color);
			}

			void GLSampler::SetMipmapBias(GLint bias)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_LOD_BIAS, bias);
			}

			void GLSampler::SetMaxMip(GLint maxMipLevel)
			{
				glSamplerParameteri(m_unit, GL_TEXTURE_MAX_LOD, maxMipLevel);
			}

			//////////////////////////////////////////////////////////////////////////

			GLenum GLSampler::GetAddressModeS()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_WRAP_S);
			}

			GLenum GLSampler::GetAddressModeT()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_WRAP_T);
			}

			GLenum GLSampler::GetAddressModeR()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_WRAP_R);
			}

			GLenum GLSampler::GetMinFilter()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_MIN_FILTER);
			}

			GLenum GLSampler::GetMagFilter()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_MAG_FILTER);
			}

			void GLSampler::GetBorderColor(GLfloat (&color)[4])
			{
				glGetSamplerParameterfv(m_unit, GL_TEXTURE_LOD_BIAS, color);
			}

			GLint GLSampler::GetMipmapBias()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_LOD_BIAS);
			}

			GLint GLSampler::GetMaxMip()
			{
				return glGetSamplerParameter(m_unit, GL_TEXTURE_MAX_LOD);
			}

		}
	}
}