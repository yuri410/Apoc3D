#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLFramebuffer final
			{
			public:
				GLFramebuffer(const List<GLuint>& colorTargets, GLuint depthTarget);
				~GLFramebuffer();

				GLFramebuffer(GLFramebuffer&& o);
				GLFramebuffer& operator=(GLFramebuffer&& o);

				GLFramebuffer(const GLFramebuffer&) = delete;
				GLFramebuffer& operator=(const GLFramebuffer&) = delete;

			private:
				GLuint m_fbo;
				
			};
		}
	}
}