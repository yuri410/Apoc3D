#pragma once

#include "GL3Common.h"
#include "Apoc3d/Graphics/RenderSystem/VertexDeclaration.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics 
	{
		namespace GL3RenderSystem
		{
			class GL3VertexDeclaration : public VertexDeclaration
			{
			public:
				GL3VertexDeclaration(const List<VertexElement>& e);
				~GL3VertexDeclaration();

				void Bind(GLProgram* prog, GL3VertexBuffer* vb);

			private:
				struct MappingKey
				{
					GLuint m_prog;
					GLuint m_vbo;
				};

				struct MappingKeyComparer
				{
					static bool Equals( const MappingKey& x,  const MappingKey& y) { return x.m_prog == y.m_prog && x.m_vbo == y.m_vbo; }

					static int32 GetHashCode(const MappingKey& obj) { return (int32)((obj.m_prog << 16) ^ obj.m_vbo); }
				};

				HashMap<MappingKey, GLVertexArray, MappingKeyComparer> m_cachedVaos;
			};
		}
	}
}