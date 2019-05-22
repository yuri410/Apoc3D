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

		}
	}
}