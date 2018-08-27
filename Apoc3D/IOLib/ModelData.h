#pragma once
#ifndef APOC3D_MODELDATA_H
#define APOC3D_MODELDATA_H

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

#include "apoc3d/ApocCommon.h"

#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/MaterialTypes.h"
#include "apoc3d/Graphics/ModelTypes.h"
#include "apoc3d/Graphics/RenderSystem/VertexElement.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/BoundingSphere.h"

#include "MaterialData.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		/**
		 *  Defines one entire mesh's data stored in binary form and procedures to load/save them.
		 */
		class APAPI MeshData
		{
		public:
			/**
			 *  Vertex data, no matter what exactly the vertex elements are, this is in a binary buffer form defined in VertexElements.
			 */
			char* VertexData = nullptr;

			List<VertexElement> VertexElements;

			BoundingSphere BoundingSphere;

			String Name;
			uint32 VertexSize = 0;
			uint32 VertexCount = 0;

			/** 
			 *  A set of arrays of material data.
			 *  Each array in the set is corresponding to a sub mesh. each material data as the array element is a 
			 *  keyframe material if material animation is applicable.
			 *
			 *  When not using any material animation, this should be a set of arrays with a length of 1.
			 *  That is in equivalent to a set of MaterialData.
			 */
			MeshMaterialSet<MaterialData*> Materials;

			/** A list of triangle faces. Only triangle list is supported as primitive at this stage. */
			List<MeshFace> Faces;

			static uint32 ComputeVertexSize(const List<VertexElement>& elements);

			void LoadData(TaggedDataReader* data);
			void SaveData(TaggedDataWriter* data) const;
			void SaveLite(BinaryWriter& bw) const;

			MeshData();
			~MeshData();

			MeshData(const MeshData&) = delete;
			MeshData& operator=(const MeshData&) = delete;

		};

		/** 
		 *  Defines one entire model's data stored in binary form and procedures to load/save them.
		 */
		class APAPI ModelData
		{
		public:
			/** A list of meshes that this model has. */
			List<MeshData*> Entities;

			ModelData() { }
			~ModelData();
			
			ModelData(const ModelData&) = delete;
			ModelData& operator=(const ModelData&) = delete;

			void Load(const ResourceLocation& rl);
			void Save(Stream& strm) const;
			void SaveLite(Stream& strm) const;
		private:
			void ReadData(TaggedDataReader* data);
			void WriteData(TaggedDataWriter*) const;
		};
	}
}
#endif