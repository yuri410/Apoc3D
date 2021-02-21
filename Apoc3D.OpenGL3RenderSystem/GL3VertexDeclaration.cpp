#include "GL3VertexDeclaration.h"
#include "GL3Buffers.h"
#include "GL3Utils.h"

#include "GL/GLProgram.h"
#include "GL/GLVertexArray.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3VertexDeclaration::GL3VertexDeclaration(const List<VertexElement>& e)
				: VertexDeclaration(e)
			{

			}

			GL3VertexDeclaration::~GL3VertexDeclaration()
			{

			}

			void GL3VertexDeclaration::Bind(GLProgram* prog, GL3VertexBuffer* vb)
			{
				vb->Bind();
				
				MappingKey k;
				k.m_prog = prog ? prog->getGLProgID() : 0;
				k.m_vbo = vb->getGLBufferID();

				GLVertexArray* vao = m_cachedVaos.TryGetValue(k);
				if (vao == nullptr)
				{
					GLVertexArray newVAO;

					for (const GLProgramVariable& att : prog->getAttributes())
					{
						const VertexElement* foundElem = nullptr;
						for (const VertexElement& ve : m_elements)
						{
							// attributes are identified with names
							if (StringUtils::EqualsNoCase(att.m_name, GLUtils::VertexElementUsageConverter[ve.getUsage()]))
							{
								foundElem = &ve;
								break;
							}
						}

						GLenum elementType;
						GLuint elementCount;
						GLboolean normalized;

						if (foundElem && GLUtils::ConvertVertexElementFormat(foundElem->getType(), elementType, elementCount, normalized))
						{
							newVAO.BindAttribute(k.m_vbo, att.m_location,
												 elementCount, elementType, normalized,
												 foundElem->getSize(), foundElem->getOffset() );
						}
					}

					m_cachedVaos.Add(k, std::move(newVAO));

					vao = &m_cachedVaos[k];
				}

				vao->Bind();
			}
		}
	}
}