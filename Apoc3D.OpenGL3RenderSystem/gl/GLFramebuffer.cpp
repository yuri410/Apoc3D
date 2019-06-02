#include "GLFramebuffer.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GLFramebuffer::GLFramebuffer(const List<GLuint>& colorTargets, GLuint depthTarget)
			{
				glCreateFramebuffers(1, &m_fbo);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

				for (int i = 0; i < colorTargets.getCount(); i++)
				{
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTargets[i], 0);
				}

				if (depthTarget)
				{
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTarget, 0);
				}

				if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{

				}
			}

			GLFramebuffer::~GLFramebuffer()
			{
				if (m_fbo)
				{
					glDeleteFramebuffers(1, &m_fbo);
					m_fbo = 0;
				}
			}


			GLFramebuffer::GLFramebuffer(GLFramebuffer&& o)
				: m_fbo(o.m_fbo)
			{
				o.m_fbo = 0;
			}
			GLFramebuffer& GLFramebuffer::operator=(GLFramebuffer&& o)
			{
				if (this != &o)
				{
					this->~GLFramebuffer();
					new (this)GLFramebuffer(std::move(o));
				}
				return *this;
			}

		}
	}
}