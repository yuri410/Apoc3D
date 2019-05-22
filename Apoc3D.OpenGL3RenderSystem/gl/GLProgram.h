#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLProgram
			{
			public:
				GLProgram();
				~GLProgram();

				void Link(const List<GLuint>& shaders);

				void IncrRefCount();
				bool DecrRefCount();
			private:
				GLuint m_prog;
				int32 m_refCount = 1;
			};
		}
	}
}