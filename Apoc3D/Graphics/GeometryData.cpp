#include "GeometryData.h"
#include "RenderSystem/VertexDeclaration.h"
#include "RenderSystem/Buffer/HardwareBuffer.h"

namespace Apoc3D
{
	namespace Graphics
	{
		void GeometryData::Setup(RenderSystem::VertexBuffer* vb, RenderSystem::IndexBuffer* ib, 
			RenderSystem::VertexDeclaration* decl,
			RenderSystem::PrimitiveType pt)
		{
			VertexBuffer = vb;
			IndexBuffer = ib;
			VertexDecl = decl;
			VertexSize = decl->GetVertexSize();
			VertexCount = vb->getSize() / VertexSize;

			PrimitiveType = pt;

			int32 count = ib ? ib->getIndexCount() : VertexCount;

			switch (pt)
			{
				case PT_PointList:
					PrimitiveCount = count;
					break;

				case PT_LineList:
					PrimitiveCount = count / 2;
					break;

				case PT_LineStrip:
					PrimitiveCount = count - 1;
					break;

				case PT_TriangleList:
					PrimitiveCount = count / 3;
					break;

				case PT_TriangleStrip:
				case PT_TriangleFan:
					PrimitiveCount = count - 2;
					break;
			}

			if (PrimitiveCount < 0)
				PrimitiveCount = 0;
		}
	}
}