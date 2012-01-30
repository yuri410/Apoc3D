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
#ifndef MESH_H
#define MESH_H

#include "Common.h"
#include "ModelTypes.h"
#include "RenderOperationBuffer.h"
#include "Math/BoundingSphere.h"
#include "Collections/FastList.h"
#include "Renderable.h"
#include "RenderSystem/VertexElement.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		/** The geometry data including vertices as well as indices and materials.
		 *
		 *  Every mesh has several materials. These materials cut the mesh into
		 *  sub parts, where each part has only one material.
		 */
		class APAPI Mesh : public Renderable
		{
		public:
			VertexBuffer* getVertexBuffer() const { return m_vertexBuffer; }
			const FastList<IndexBuffer*>& getIndexBuffers() const { return m_indexBuffers; }
			int32 getIndexCount() const { return m_primitiveCount * 3; }
			/** Copies the indices to a given buffer.
			*/
			void GetIndices(uint* dest) const;

			const FastList<VertexElement>& getVertexElement() const { return m_vertexElements; }
			const int32* getPartPrimitiveCount() const { return m_partPrimitiveCount; }
			const int32* getPartVertexCount() const { return m_partVertexCount; } 
			const int32 getPartCount() const { return m_indexBuffers.getCount(); }

			/** A set of arrays of materials.
			 *  Each array in the set is corresponding to a sub mesh. each material as the array element is a 
			 *  keyframe material if material animation is applicable.
			 *
			 *  When not using any material animation, this should be a set of arrays with a length of 1.
			 *  That is in equivalent to a set of Material.
			 */
			MeshMaterialSet<Material*>* getMaterials() { return &m_materials; }
			int32 getVertexSize() const { return m_vertexSize; }
			int32 getVertexCount() const { return m_vertexCount; }
			int32 getPrimitiveConut() const { return m_primitiveCount; }

			const String& getName() const { return m_name; }
			RenderDevice* getRenderDevice() const { return m_renderDevice; }


			void setName(const String& value) { m_name = value; }

			const BoundingSphere& getBoundingSphere() const { return m_boundingSphere; }
			void setBoundingSphere(const BoundingSphere& sphere) { m_boundingSphere = sphere; }

			/** Passes triangle primitives one by one, while call the callback for each one.
			*/
			void ProcessAllTriangles(IMeshTriangleCallBack* callback) const;

			int32 CalculateSizeInBytes() const;



			virtual RenderOperationBuffer* GetRenderOperation(int level);
			
			void Save(MeshData* data);

			Mesh(RenderDevice* device, const MeshData* data);
			~Mesh(void);

		private:
			VertexDeclaration* m_vtxDecl;
			int32 m_vertexSize;
			bool m_opBufferBuilt;
			RenderOperationBuffer m_bufferedOp;
			MeshMaterialSet<Material*> m_materials;
			VertexBuffer* m_vertexBuffer;
			FastList<IndexBuffer*> m_indexBuffers;

			/** An array indicating the primitive 
			 *  count for each sub mesh (or sub part, cut by materials; see the Mesh class)
			 *  at specific index.
			 */
			int32* m_partPrimitiveCount;
			/** Similar to m_partPrimitiveCount but the vertex count.
			*/
			int32* m_partVertexCount;

			RenderDevice* m_renderDevice;
			ObjectFactory* m_factory;

			String m_name;

			int32 m_primitiveCount;
			int32 m_vertexCount;


			BoundingSphere m_boundingSphere;

			FastList<VertexElement> m_vertexElements;

		};
	}
}
#endif