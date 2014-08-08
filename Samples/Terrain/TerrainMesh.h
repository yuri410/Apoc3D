#ifndef TERRAINMESH_H
#define TERRAINMESH_H

#include "TerrainCommon.h"

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
		
		/** Gets a string that represent this resource
		*/
		static String GetHashString(int x, int z, int size);

	protected:
		/** This is called by the background thread to load the actual content of the mesh.
		*/
		virtual void load();
		/** This is call by the background thread to unload the actual content of the mesh.
		 *  In this demo, vertex buffers should be released and deleted to 
		 *  make effect that the memory being released.
		 */
		virtual void unload();
	private:
		RenderDevice* m_device;


		IndexBuffer* m_sharedIndex;

		VertexBuffer* m_vertexBuffer;
		VertexDeclaration* m_vtxDecl;
		Material m_material;

		GeometryData m_geoData;

		RenderOperationBuffer m_opBuffer;

		/** The number of vertices on one edge of the terrain mesh quad.
		*/
		int m_edgeVertexCount;
		/** The total number of triangles as primitives in the terrain mesh.
		*/
		int m_primitiveCount;

		/** The X coordinate in the terrain chunk space.
		*/
		int m_bx;
		/** The Z coordinate in the terrain chunk space.
		*/
		int m_bz;

	};

	/** The resource manager for TerrainMeshes. This type of use of ResourceManager enables
	 *  independent background resource management for customized resources. In this case it is 
	 *  TerrainMesh. 
	 *
	 *  TerrainMesh for different level of details are all managed in this class. To distinguish
	 *  different TerrainMesh at different chunk space and LODs, a "HashString", which is the unique
	 *  id for each chunk at certain LOD, is used as the name of resource(see TerrainMesh::GetHashString). 
	 *  This will prevent the same terrain mesh loaded twice. Though duplication seems impossible in a chunk world,
	 *  as a universal model for resource management, it is required by the resource manager.
	 *  HashString could be more useful in other cases like ModelManager loading from files; it
	 *  will help the ModelManager from loading from the same file multiple times.
	 *  
	 *  Index data for the LODs are prepared for using as a shared resources. So did the tree models,
	 *  which does not have artist-created animations to necessarily load 1 model per-instance.
	 */
	class TerrainMeshManager : public ResourceManager
	{
		SINGLETON_DECL(TerrainMeshManager);

	public:
		TerrainMeshManager();
		~TerrainMeshManager();

		void InitializeResources(RenderDevice* device);
		void FinalizeResources();

		ResourceHandle<TerrainMesh>* CreateInstance(RenderDevice* rd, int bx, int bz, int lod);

		Model* getTreeModelByLOD(int lod) const;
		SharedIndexData* getIndexData(int size) const;
		ResourceHandle<Texture>* getTexture(int id) const { return m_textures[id]; }

	private:
		Model* m_tree[4];

		SharedIndexData* m_idxLod0;
		SharedIndexData* m_idxLod1;
		SharedIndexData* m_idxLod2;
		SharedIndexData* m_idxLod3;

		ResourceHandle<Texture>* m_textures[6];
	};

	/** Stores the index data for each size of the terrain tile at a specific LOD, 
	 *  the mesh in one LOD share the same index data as the topology is unchanged.
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