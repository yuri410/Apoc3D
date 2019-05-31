#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLSampler
			{
			public:
				GLSampler();
				~GLSampler();

				void Bind(GLuint unit);

				void SetAddressModeS(GLenum mode);
				void SetAddressModeT(GLenum mode);
				void SetAddressModeR(GLenum mode);
				void SetMinFilter(GLenum mode);
				void SetMagFilter(GLenum mode);
				void SetBorderColor(const GLfloat (&color)[4]);
				void SetMipmapBias(GLint bias);
				void SetMaxMip(GLint maxMipLevel);

				GLenum GetAddressModeS();
				GLenum GetAddressModeT();
				GLenum GetAddressModeR();
				GLenum GetMinFilter();
				GLenum GetMagFilter();
				void GetBorderColor(GLfloat(&color)[4]);
				GLint GetMipmapBias();
				GLint GetMaxMip();

			private:
				GLuint	m_sampler;
				GLuint	m_unit = 0;
				bool	m_bound = false;
			};
		}
	}
}