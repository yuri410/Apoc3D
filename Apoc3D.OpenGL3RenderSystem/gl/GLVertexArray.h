#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLVertexArray
			{
			public:
				GLVertexArray();
				~GLVertexArray();

			private:
				GLuint m_vao;
			};
		}
	}
}