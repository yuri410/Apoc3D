#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLVertexArray final
			{
			public:
				GLVertexArray();
				~GLVertexArray();

				GLVertexArray(GLVertexArray&& o);
				GLVertexArray& operator=(GLVertexArray&& o);

				GLVertexArray(const GLVertexArray&) = delete;
				GLVertexArray& operator=(const GLVertexArray&) = delete;

				void Bind();
				void BindAttribute(GLuint vbo, GLint attrib, GLuint elementCount, GLenum elementType, GLboolean normalized, GLuint stride, GLuint offset);
			private:
				GLuint m_vao;
			};
		}
	}
}