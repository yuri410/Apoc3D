#pragma once
#ifndef APOC3D_MESH_H
#define APOC3D_MESH_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "ModelTypes.h"
#include "RenderOperationBuffer.h"
#include "Renderable.h"
#include "RenderSystem/VertexElement.h"
#include "apoc3d/Collections/CollectionsCommon.h"
#include "apoc3d/Math/BoundingSphere.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		/** 
		 *  The geometry data including vertices as well as indices and materials.
		 *
		 *  Every mesh has several materials. These materials cut the mesh into
		 *  sub parts, where each part has only one material.
		 */
		class APAPI Mesh : public Renderable
		{
			static Material*& GetItem(const Mesh* m, int32 i, int32 j) { return m->m_subParts[i].MaterialFrames[j]; }
			static int32 GetCountI(const Mesh* m) { return m->getPartCount(); }
			static int32 GetCountJ(const Mesh* m, int32 i) { return m->getMaterialFrames(i).getCount(); }

		public:

			struct MaterialIterator
			{
			public:
				using Iterator = Apoc3D::Collections::Iterator2D < Material*, Mesh, GetItem, GetCountI, GetCountJ > ;

				MaterialIterator(const Mesh* m) : m_mesh(m) { }

				Iterator begin() const { return Iterator(m_mesh); }
				Iterator end() const { return Iterator(m_mesh, -1, -1); }

			private: 
				const Mesh* m_mesh;
			};

			Mesh(RenderDevice* device, const MeshData* data);
			~Mesh();

			Mesh(const Mesh&) = delete;
			Mesh& operator=(const Mesh&) = delete;

			/** Copies the indices to a given buffer. */
			void GetIndices(uint* dest) const;

			virtual RenderOperationBuffer* GetRenderOperation(int level);

			void Save(MeshData* data);

			/** Passes triangle primitives one by one, while call the callback for each one. */
			void ProcessAllTriangles(IMeshTriangleCallBack* callback) const;

			int32 CalculateSizeInBytes() const;


			VertexBuffer* getVertexBuffer() const { return m_vertexBuffer; }
			VertexDeclaration* getVertexDecl() const { return m_vtxDecl; }

			int32 getPartCount() const { return m_subParts.getCount(); }

			int32 getPartPrimitiveCount(int32 partIdx) const { return m_subParts[partIdx].PrimitiveCount; }
			int32 getPartVertexCount(int32 partIdx) const { return m_subParts[partIdx].VertexCount; }

			IndexBuffer* getIndexBuffer(int32 partIdx) const { return m_subParts[partIdx].Indices; }
			List<Material*>& getMaterialFrames(int32 partIdx) const { return m_subParts[partIdx].MaterialFrames; }
			Material* getMaterial(int32 partIdx, int32 frameIdx) const { return m_subParts[partIdx].MaterialFrames[frameIdx]; }

			MaterialIterator getMaterials() const { return MaterialIterator(this); }

			/** 
			 *  A set of arrays of materials.
			 *  Each array in the set is corresponding to a sub mesh. each material as the array element is a 
			 *  keyframe material if material animation is applicable.
			 *
			 *  When not using any material animation, this should be a set of arrays with a length of 1.
			 *  That is in equivalent to a set of Material.
			 */
			//MeshMaterialSet<Material*>* getMaterials() { return &m_materials; }

			int32 getVertexSize() const { return m_vertexSize; }
			int32 getVertexCount() const { return m_vertexCount; }
			int32 getPrimitiveConut() const { return m_primitiveCount; }
			int32 getIndexCount() const { return m_primitiveCount * 3; }

			const String& getName() const { return m_name; }
			RenderDevice* getRenderDevice() const { return m_renderDevice; }


			void setName(const String& value) { m_name = value; }

			const BoundingSphere& getBoundingSphere() const { return m_boundingSphere; }
			void setBoundingSphere(const BoundingSphere& sphere) { m_boundingSphere = sphere; }

		private:
			// a sub part(sub mesh) divided by materials
			struct SubPart
			{
				List<Material*> MaterialFrames;
				IndexBuffer* Indices = nullptr;

				int32 PrimitiveCount = 0;
				int32 VertexCount = 0;

				int32 VertexRangeUsedStart = 0;
				int32 VertexRangeUsedCount = 0;

				SubPart(const SubPart& o) = delete;
				SubPart& operator=(const SubPart& o) = delete;

				SubPart(SubPart&& o);
				SubPart& operator=(SubPart&& o);
				SubPart(){}
				~SubPart();

				template <typename T, IndexBufferFormat IBT>
				void SetIndexData(ObjectFactory* fac, const List<uint>& pi, int32 vertexCount, bool* used);
			};

			VertexDeclaration* m_vtxDecl;
			int32 m_vertexSize;
			bool m_opBufferBuilt = false;
			RenderOperationBuffer m_bufferedOp;
			VertexBuffer* m_vertexBuffer;

			List<SubPart> m_subParts;
			
			RenderDevice* m_renderDevice;
			ObjectFactory* m_factory;

			String m_name;

			int32 m_primitiveCount;
			int32 m_vertexCount;


			BoundingSphere m_boundingSphere;


		};
	}
}
#endif