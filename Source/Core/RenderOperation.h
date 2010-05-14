#pragma once

#include "..\Common.h"

class RenderOperation
{
private:
	IDirect3DVertexBuffer9* m_vtxBuffer;
	IDirect3DIndexBuffer9* m_idxBuffer;
	IDirect3DVertexDeclaration9* m_vtxDecl;
	int primCount;
	int baseVertex;

public:
	const IDirect3DVertexBuffer9* getVertexBuffer() { return m_vtxBuffer; }
	const IDirect3DIndexBuffer9* getIndexBuffer() { return m_idxBuffer; }
	const IDirect3DVertexDeclaration9* getVertexDecl() { return m_vtxDecl; }

	void setVertexBuffer(IDirect3DVertexBuffer9* value) { m_vtxBuffer = value; }
	void setIndexBuffer(IDirect3DIndexBuffer9* value) { m_idxBuffer = value; }
	void setVertexDecl(IDirect3DVertexDeclaration9* value) { m_vtxDecl = value; }


	RenderOperation(void) { }
	~RenderOperation(void) { }
};

