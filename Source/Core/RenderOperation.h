#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/*
		  Represents an operation to render a mesh part in the scene.
		  RenderOperation is used by the engine to manage the scene rendering pipeline.
		*/
		class _Export RenderOperation
		{
		private:
			IDirect3DVertexBuffer9* m_vtxBuffer;
			IDirect3DIndexBuffer9* m_idxBuffer;
			IDirect3DVertexDeclaration9* m_vtxDecl;
			int primCount;
			int baseVertex;

		public:
			/*
			  Gets the vertex buffer of this RenderOperation
			*/
			const IDirect3DVertexBuffer9* getVertexBuffer() { return m_vtxBuffer; }
			/*
			  Gets the index buffer of this RenderOperation
			*/
			const IDirect3DIndexBuffer9* getIndexBuffer() { return m_idxBuffer; }
			/*
			  Gets the vertex declaration of this RenderOperation
			*/
			const IDirect3DVertexDeclaration9* getVertexDecl() { return m_vtxDecl; }

			void setVertexBuffer(IDirect3DVertexBuffer9* value) { m_vtxBuffer = value; }
			void setIndexBuffer(IDirect3DIndexBuffer9* value) { m_idxBuffer = value; }
			void setVertexDecl(IDirect3DVertexDeclaration9* value) { m_vtxDecl = value; }


			RenderOperation(void) { }
			~RenderOperation(void) { }
		};
	}
}
