#ifndef TERRAINMESH_H
#define TERRAINMESH_H

#include "TerrainCommon.h"
#include "Scene/SceneObject.h"
#include "Core/Resource.h"
#include "Core/ResourceManager.h"
#include "Core/Singleton.h"
#include "Graphics/Renderable.h"
#include "Graphics/Material.h"
#include "Graphics/GeometryData.h"
#include "Graphics/RenderOperationBuffer.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Scene;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace SampleTerrain
{
	/** The terrain's mesh for each terrain chunk, considered as Resources.
	*/
	class TerrainMesh : public Resource, public Renderable
	{
	public:
		TerrainMesh(RenderDevice* rd, int bx, int bz, int size);
		~TerrainMesh() { }

		virtual uint32 getSize();

		virtual RenderOperationBuffer* GetRenderOperation(int lod);
		
		static String GetHashString(int x, int z, int size);

	protected:

		virtual void load();
		virtual void unload();
	private:
		RenderDevice* m_device;


		IndexBuffer* m_sharedIndex;

		VertexBuffer* m_vertexBuffer;
		VertexDeclaration* m_vtxDecl;
		Material m_material;

		GeometryData m_geoData;

		RenderOperationBuffer m_opBuffer;

		int m_edgeVertexCount;
		int m_primitiveCount;

		int m_bx;
		int m_bz;

	};

	class TerrainMeshManager : public ResourceManager, public Singleton<TerrainMeshManager>
	{
	public:
		TerrainMeshManager();
		~TerrainMeshManager();

		void InitializeResources(RenderDevice* device);
		void FinalizeResources();

		ResourceHandle<TerrainMesh>* CreateInstance(RenderDevice* rd, int bx, int bz, int lod);

		Model* getTreeModelByLOD(int lod) const;
		SharedIndexData* getIndexData(int size) const;
		ResourceHandle<Texture>* getTexture(int id) const { return m_textures[id]; }

		SINGLETON_DECL_HEARDER(TerrainMeshManager);

	private:
		Model* m_tree[4];

		SharedIndexData* m_idxLod0;
		SharedIndexData* m_idxLod1;
		SharedIndexData* m_idxLod2;
		SharedIndexData* m_idxLod3;

		ResourceHandle<Texture>* m_textures[6];
	};

	/** The mesh in one LOD share the same index data as the topology is unchanged.
	*/
	class SharedIndexData
	{
	public:
		SharedIndexData(RenderDevice* rd, int terrSize);
		~SharedIndexData();

		IndexBuffer* getIndexBuffer() const { return m_indexBuffer; }
	private:
		IndexBuffer* m_indexBuffer;
		int m_terrainSize;
	};

	
}

#endif