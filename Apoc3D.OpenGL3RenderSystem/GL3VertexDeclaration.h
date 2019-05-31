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

			private:
				
			};
		}
	}
}