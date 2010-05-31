#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		/* Define buffers and information used to render a geomentry
		*/
		class _Export GeomentryData
		{
		private:
			VertexBuffer* m_vtxBuffer;
			IndexBuffer* m_idxBuffer;
			VertexDeclaration* m_vtxDecl;
			int m_primCount;
			int m_baseVertex;

		public:
			/*
			  Gets the vertex buffer of this GeomentryData object
			*/
			const IDirect3DVertexBuffer9* getVertexBuffer() { return m_vtxBuffer; }
			/*
			  Gets the index buffer of this GeomentryData object
			*/
			const IDirect3DIndexBuffer9* getIndexBuffer() { return m_idxBuffer; }
			/*
			  Gets the vertex declaration of this GeomentryData object
			*/
			const IDirect3DVertexDeclaration9* getVertexDecl() { return m_vtxDecl; }

			void setVertexBuffer(IDirect3DVertexBuffer9* value) { m_vtxBuffer = value; }
			void setIndexBuffer(IDirect3DIndexBuffer9* value) { m_idxBuffer = value; }
			void setVertexDecl(IDirect3DVertexDeclaration9* value) { m_vtxDecl = value; }


			GeomentryData(void);
			~GeomentryData(void);
		};
	};
};