#include "GLVertexArray.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GLVertexArray::GLVertexArray()
			{
				glGenVertexArrays(1, &m_vao);
			}

			GLVertexArray::~GLVertexArray()
			{
				if (m_vao)
				{
					glDeleteVertexArrays(1, &m_vao);
					m_vao = 0;
				}
			}

			GLVertexArray::GLVertexArray(GLVertexArray&& o)
				: m_vao(o.m_vao)
			{
				o.m_vao = 0;
			}

			GLVertexArray& GLVertexArray::operator=(GLVertexArray&& o)
			{
				if (this != &o)
				{
					this->~GLVertexArray();
					new (this)GLVertexArray(std::move(o));
				}
				return *this;
			}

			void GLVertexArray::Bind()
			{
				glBindVertexArray(m_vao);
			}
			void GLVertexArray::BindAttribute(GLuint vbo, GLint attrib, GLuint elementCount, GLenum elementType, GLboolean normalized, GLuint stride, GLuint offset)
			{
				glEnableVertexAttribArray(attrib);
				glVertexAttribPointer(attrib, elementCount, elementType, normalized, stride, (const GLvoid*)offset);
			}
		}
	}
}