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

#ifndef MODELDATA_H
#define MODELDATA_H

#include "Common.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/MaterialTypes.h"
#include "Graphics/ModelTypes.h"
#include "Graphics/RenderSystem/VertexElement.h"
#include "Collections/FastList.h"
#include "Math/Color.h"
#include "Math/BoundingSphere.h"
#include "MaterialData.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI MeshData
		{
		public:
			char* VertexData;

			int32 TextureCoordCount;
			bool HasTextureCoord[MaxTextures];
			FastList<VertexElement> VertexElements;

			int32 ParentBoneID;
			BoundingSphere BoundingSphere;

			String Name;
			uint32 VertexSize;
			uint32 VertexCount;

			MeshMaterialSet<MaterialData> Materials;

			FastList<MeshFace> Faces;

			static uint32 ComputeVertexSize(const FastList<VertexElement>& elements);

			void Load(TaggedDataReader* data);
			TaggedDataWriter* Save();

		};


		class APAPI ModelData
		{
		private:
			void ReadData(TaggedDataReader* data, int32 id);
			TaggedDataWriter* WriteData() const;
		public:
			FastList<MeshData*> Entities;

			ModelData()
			{
			}
			~ModelData();
			
			void Load(const ResourceLocation* rl);
			void Save(Stream* strm) const;
		};
	}
}
#endif