#include "GeometryData.h"
#include "RenderSystem/VertexDeclaration.h"
#include "RenderSystem/HardwareBuffer.h"

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
				case PrimitiveType::PointList:
					PrimitiveCount = count;
					break;

				case PrimitiveType::LineList:
					PrimitiveCount = count / 2;
					break;

				case PrimitiveType::LineStrip:
					PrimitiveCount = count - 1;
					break;

				case PrimitiveType::TriangleList:
					PrimitiveCount = count / 3;
					break;

				case PrimitiveType::TriangleStrip:
				case PrimitiveType::TriangleFan:
					PrimitiveCount = count - 2;
					break;
			}

			if (PrimitiveCount < 0)
				PrimitiveCount = 0;
		}
	}
}