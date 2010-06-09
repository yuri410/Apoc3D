/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef GEOMETRYDATA_H
#define GEOMETRYDATA_H

#pragma once
#include "..\Common.h"
#include "..\HashHandleObject.h"

namespace Apoc3D
{
	namespace Core
	{
		/* Define buffers and information used to render a geomentry
		*/
		class _Export GeometryData : public HashHandleObject
		{
		private:
			VertexBuffer* m_vtxBuffer;
			IndexBuffer* m_idxBuffer;
			VertexDeclaration* m_vtxDecl;
			int m_primCount;
			int m_baseVertex;

			PrimitiveType m_primType;
			SceneObject* m_sender;

		public:
			/* Gets the vertex buffer of this GeomentryData object
			*/
			IDirect3DVertexBuffer9* getVertexBuffer() const { return m_vtxBuffer; }
			/* Gets the index buffer of this GeomentryData object
			*/
			IDirect3DIndexBuffer9* getIndexBuffer() const { return m_idxBuffer; }
			/* Gets the vertex declaration of this GeomentryData object
			*/
			IDirect3DVertexDeclaration9* getVertexDecl() const { return m_vtxDecl; }
			/* 
			*/
			PrimitiveType getPrimitiveType() const { return m_primType; }

			void setVertexBuffer(IDirect3DVertexBuffer9* value) { m_vtxBuffer = value; }
			void setIndexBuffer(IDirect3DIndexBuffer9* value) { m_idxBuffer = value; }
			void setVertexDecl(IDirect3DVertexDeclaration9* value) { m_vtxDecl = value; }
			void setPrimitiveType(PrimitiveType value) { m_primType = value; }


			GeometryData(void)
				: m_sender(0), m_baseVertex(0), m_primCount(0), 
				m_vtxDecl(0), m_idxBuffer(0), m_vtxBuffer(0),
				m_primType(D3DPT_POINTLIST)
			{ }
			~GeometryData(void){}
		};
	};
};
#endif