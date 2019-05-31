#pragma once

#include "../GL3Common.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			struct GLProgramVariable
			{
				String m_name;
				GLuint m_location;
				GLint  m_size;
				GLenum m_type;

				GLuint m_textureSlotID = 0;
				bool   m_isSampler = false;
				bool   m_isTexture = false;

				bool IsTextureType() const;
				bool IsSamplerType() const;
			};

			class GLProgram
			{
			public:
				GLProgram();
				~GLProgram();

				void Link(const List<GLuint>& shaders);

				const GLProgramVariable* getUniform(const String& name) const { return m_uniformTable.TryGetValue(name); }

				void IncrRefCount();
				bool DecrRefCount();

			private:
				void RetireveVariables(bool uniform);

				GLuint m_prog;
				
				int32 m_refCount = 1;

				HashMap<String, GLProgramVariable> m_uniformTable;

			};
		}
	}
}