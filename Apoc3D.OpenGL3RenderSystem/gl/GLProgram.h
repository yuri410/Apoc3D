#pragma once

#include "../GL3Common.h"
#include "Apoc3D/Collections/HashMap.h"

using namespace Apoc3D::Collections;
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

			class GLProgram final
			{
			public:
				GLProgram();
				~GLProgram();

				GLProgram(GLProgram&& o);
				GLProgram& operator=(GLProgram&& o);

				GLProgram(const GLProgram&) = delete;
				GLProgram& operator=(const GLProgram&) = delete;
				
				void Link(const List<GLuint>& shaders);

				const GLProgramVariable* getUniform(const String& name) const { return m_uniformTable.TryGetValue(name); }
				const List<GLProgramVariable>& getAttributes() const { return m_attributes; }

				GLuint getGLProgID() const { return m_prog; }

				void IncrRefCount();
				bool DecrRefCount();

			private:
				void RetireveVariables(bool uniform);

				GLuint m_prog;
				
				int32 m_refCount = 1;

				HashMap<String, GLProgramVariable> m_uniformTable;
				List<GLProgramVariable> m_attributes;
			};
		}
	}
}