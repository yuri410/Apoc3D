#include "TerrainMesh.h"

#include "Core/Logging.h"
#include "Core/ResourceHandle.h"
#include "Collections/FastList.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/Buffer/HardwareBuffer.h"
#include "Graphics/RenderSystem/VertexDeclaration.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/TextureManager.h"
#include "Graphics/ModelManager.h"
#include "Graphics/Model.h"
#include "Vfs/FileSystem.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/FileLocateRule.h"
#include "Math/RandomUtils.h"
#include "Math/PerlinNoise.h"
#include "Utility/StringUtils.h"

#include "Terrain.h"

SINGLETON_DECL(SampleTerrain::TerrainMeshManager);

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace SampleTerrain
{
	struct TerrainVertex
	{
		float Position[3];
		float Normal[3];
	};

	static VertexElement Elements[2] =
	{
		VertexElement(0, VEF_Vector3, VEU_Position, 0),
		VertexElement(12, VEF_Vector3, VEU_Normal, 0)
		//VertexElement(24, VEF_Vector2, VEU_TextureCoordinate, 0)
	};


	String TerrainMesh::GetHashString(int x, int z, int size)
	{
		return L"TM" + StringUtils::ToString(x) + L" " + StringUtils::ToString(z) + L" " + StringUtils::ToString(size);
	}


	TerrainMesh::TerrainMesh(RenderDevice* rd, int bx, int bz, int size)
		: Resource(TerrainMeshManager::getSingletonPtr(), TerrainMesh::GetHashString(bx,bz,size)), m_device(rd),
		m_edgeVertexCount(size+1),
		m_vertexBuffer(0), m_vtxDecl(0),
		m_bx(bx), m_bz(bz), m_material(rd)
	{

		m_material.Cull = CULL_Clockwise;
		m_material.Ambient = Color4(0.35f,0.35f,0.35f);
		m_material.Diffuse = Color4(1.0f, 1.0f, 1.0f);
		m_material.Specular = Color4(0.0f, 0.0f, 0.0f);
		m_material.Power = 1;
		m_material.setPriority(5);
		m_material.setPassFlags(1);
		m_material.setPassEffect(0, EffectManager::getSingleton().getEffect(L"Terrain"));
		m_material.setTexture(0, TerrainMeshManager::getSingleton().getTexture(0));
		m_material.setTexture(1, TerrainMeshManager::getSingleton().getTexture(1));
		m_material.setTexture(2, TerrainMeshManager::getSingleton().getTexture(2));
		m_material.setTexture(3, TerrainMeshManager::getSingleton().getTexture(3));
		m_material.setTexture(4, TerrainMeshManager::getSingleton().getTexture(4));
		m_material.setTexture(5, TerrainMeshManager::getSingleton().getTexture(5));

		m_primitiveCount = size * size*2;

		m_sharedIndex = TerrainMeshManager::getSingleton().getIndexData(size)->getIndexBuffer();
	}

	RenderOperationBuffer* TerrainMesh::GetRenderOperation(int lod)
	{
		if (getState() == RS_Loaded)
		{
			m_opBuffer.FastClear();

			RenderOperation op;
			op.Material = &m_material;
			op.GeometryData = &m_geoData;
			Matrix::CreateTranslation(op.RootTransform, m_bx*Terrain::BlockLength,0,m_bz*Terrain::BlockLength);
			m_opBuffer.Add(op);


			return &m_opBuffer;
		}
		return 0;
	}

	uint32 TerrainMesh::getSize()
	{
		int size = 0;

		size += sizeof(TerrainVertex) * m_edgeVertexCount * m_edgeVertexCount;
		//size += sizeof(int) * 6 * m_primitiveCount;

		return size;
	}
	void TerrainMesh::load()
	{
		//m_opBuffer.FastClear();
		
		ObjectFactory* fac = m_device->getObjectFactory();

		FastList<VertexElement> elements(4);
		elements.Add(Elements[0]); 
		elements.Add(Elements[1]);
		//elements.Add(Elements[2]);

		m_vtxDecl = fac->CreateVertexDeclaration(elements);
		

		int vertexCount = m_edgeVertexCount * m_edgeVertexCount;
		m_vertexBuffer = fac->CreateVertexBuffer(vertexCount, m_vtxDecl, BU_WriteOnly);

		TerrainVertex* vtxData = reinterpret_cast<TerrainVertex*>(m_vertexBuffer->Lock(LOCK_None));

		const float HeightScale= Terrain::HeightScale;

		float cellLength = Terrain::BlockLength/(m_edgeVertexCount-1);
		
		
		for (int i=0;i<m_edgeVertexCount;i++)
		{
			for (int j=0;j<m_edgeVertexCount;j++)
			{
				float worldX = i*cellLength + m_bx*Terrain::BlockLength;
				float worldZ = j*cellLength + m_bz*Terrain::BlockLength;

				float height = Terrain::GetHeightAt(worldX, worldZ);
				
				int index = i * m_edgeVertexCount + j;

				vtxData[index].Position[0] = cellLength * i;
				vtxData[index].Position[1] = height * HeightScale;
				vtxData[index].Position[2] = cellLength * j;

				//vtxData[index].u = 16 * (float)i / m_edgeVertexCount;
				//vtxData[index].v = 16 * (float)j / m_edgeVertexCount;
			}
		}
		// normal generation is done on a separate pass
		// Calculated by cross product
		for (int i=0;i<m_edgeVertexCount;i++)
		{
			for (int j=0;j<m_edgeVertexCount;j++)
			{
				int index = i * m_edgeVertexCount + j;
				Vector3 pos = Vector3Utils::LDVectorPtr(vtxData[index].Position);
				Vector3 posB;
				if (i<m_edgeVertexCount-1)
				{
					int indexB = (i+1) * m_edgeVertexCount + j;
					posB = Vector3Utils::LDVectorPtr(vtxData[indexB].Position);
				}
				else
				{
					float height = (float)Terrain::GetHeightAt((i+1)*cellLength + m_bx*Terrain::BlockLength, j*cellLength+m_bz*Terrain::BlockLength);
					posB = Vector3Utils::LDVector(cellLength * (i+1), height*HeightScale, cellLength * j);
				}

				Vector3 posR;
				if (j<m_edgeVertexCount-1)
				{
					int indexR = i * m_edgeVertexCount + j+1;
					posR = Vector3Utils::LDVectorPtr(vtxData[indexR].Position);
				}
				else
				{
					float height = (float)Terrain::GetHeightAt(i*cellLength + m_bx*Terrain::BlockLength, (j+1)*cellLength+m_bz*Terrain::BlockLength);
					posR = Vector3Utils::LDVector(cellLength * i, height*HeightScale, cellLength * (j+1));
				}
				
				Vector3 va = Vector3Utils::Subtract(posR, pos);
				Vector3 vb = Vector3Utils::Subtract(posB, pos);
				Vector3 n = Vector3Utils::Cross(va, vb);
				n = Vector3Utils::Normalize(n);

				Vector3Utils::Store(n, vtxData[index].Normal);
			}
		}


		m_vertexBuffer->Unlock();

		m_geoData.VertexDecl = m_vtxDecl;
		m_geoData.VertexSize = sizeof(TerrainVertex);
		m_geoData.VertexBuffer = m_vertexBuffer;
		m_geoData.IndexBuffer = m_sharedIndex;
		m_geoData.PrimitiveCount = m_primitiveCount;
		m_geoData.VertexCount = vertexCount;
		m_geoData.PrimitiveType = PT_TriangleList;
		m_geoData.BaseVertex = 0;

		m_opBuffer.ReserveDiscard(100);
	}
	void TerrainMesh::unload()
	{
		delete m_vertexBuffer;
		delete m_vtxDecl;
		m_vertexBuffer = 0;
		m_vtxDecl = 0;
		m_opBuffer.Clear();
		m_opBuffer.ReserveDiscard(4);
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	TerrainMeshManager::TerrainMeshManager()
		: ResourceManager(L"TerrainMeshManager Manager ", 32 * 1048576, true)
	{
		LogManager::getSingleton().Write(LOG_System, 
			L"TerrainMeshManager initialized with a cache size 32MB, using async streaming.", 
			LOGLVL_Infomation);
	}
	TerrainMeshManager::~TerrainMeshManager()
	{

	}
	void TerrainMeshManager::InitializeResources(RenderDevice* device)
	{
		// load up shared things

		m_idxLod0 = new SharedIndexData(device, 128);
		m_idxLod1 = new SharedIndexData(device, 96);
		m_idxLod2 = new SharedIndexData(device, 64);
		m_idxLod3 = new SharedIndexData(device, 32);

		FileLocation* fl = FileSystem::getSingleton().Locate(L"TRock.tex", FileLocateRule::Textures);
		m_textures[0] = TextureManager::getSingleton().CreateInstance(device,fl, false);
		fl = FileSystem::getSingleton().Locate(L"TSand.tex", FileLocateRule::Textures);
		m_textures[1] = TextureManager::getSingleton().CreateInstance(device,fl, false);
		fl = FileSystem::getSingleton().Locate(L"TGrass.tex", FileLocateRule::Textures);
		m_textures[2] = TextureManager::getSingleton().CreateInstance(device,fl, false);
		fl = FileSystem::getSingleton().Locate(L"TSnow.tex", FileLocateRule::Textures);
		m_textures[3] = TextureManager::getSingleton().CreateInstance(device,fl, false);
		fl = FileSystem::getSingleton().Locate(L"TEdge.tex", FileLocateRule::Textures);
		m_textures[4] = TextureManager::getSingleton().CreateInstance(device,fl, false);
		fl = FileSystem::getSingleton().Locate(L"foliagecolor.tex", FileLocateRule::Textures);
		m_textures[5] = TextureManager::getSingleton().CreateInstance(device,fl, false);


		FileLocateRule rule;
		LocateCheckPoint cp;
		cp.AddPath(L"models");
		rule.AddCheckPoint(cp);

		fl = FileSystem::getSingleton().Locate(L"tree_l0.mesh", rule);
		m_tree[0] = new Model( ModelManager::getSingleton().CreateInstance(device, fl));

		fl = FileSystem::getSingleton().Locate(L"tree_l1.mesh", rule);
		m_tree[1] = new Model( ModelManager::getSingleton().CreateInstance(device, fl));

		fl = FileSystem::getSingleton().Locate(L"tree_l2.mesh", rule);
		m_tree[2] = new Model( ModelManager::getSingleton().CreateInstance(device, fl));

		fl = FileSystem::getSingleton().Locate(L"tree_l3.mesh", rule);
		m_tree[3] = new Model( ModelManager::getSingleton().CreateInstance(device, fl));


	}
	void TerrainMeshManager::FinalizeResources()
	{
		delete m_idxLod0;
		delete m_idxLod1;
		delete m_idxLod2;
		delete m_idxLod3;

		delete m_tree[0];
		delete m_tree[1];
		delete m_tree[2];
		delete m_tree[3];


		delete m_textures[0];
		delete m_textures[1];
		delete m_textures[2];
		delete m_textures[3];
		delete m_textures[4];
		delete m_textures[5];
	}
	ResourceHandle<TerrainMesh>* TerrainMeshManager::CreateInstance(RenderDevice* rd, int bx, int bz, int lod)
	{
		int size;
		if (lod == 0)
			size = 128;
		else if (lod == 1)
			size = 96;
		else if (lod == 2)
			size = 64;
		else 
			size = 32;


		Resource* retirved = Exists(TerrainMesh::GetHashString(bx,bz, size));
		if (!retirved)
		{
			TerrainMesh* mdl = new TerrainMesh(rd, bx,bz,size);
			retirved = mdl;
			NotifyNewResource(mdl);
		}
		return new ResourceHandle<TerrainMesh>((TerrainMesh*)retirved);
	}

	SharedIndexData* TerrainMeshManager::getIndexData(int size) const
	{
		switch (size)
		{
		case 128:
			return m_idxLod0;
		case 96:
			return m_idxLod1;
		case 64:
			return m_idxLod2;
		case 32:
			return m_idxLod3;
		}
		return m_idxLod2;
	}
	Model* TerrainMeshManager::getTreeModelByLOD(int lod) const
	{
		return m_tree[lod];
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	SharedIndexData::SharedIndexData(RenderDevice* rd, int terrSize)
		: m_terrainSize(terrSize)
	{
		ObjectFactory* objFac = rd->getObjectFactory();

		int primCount = terrSize * terrSize * 2;
		int indexCount = primCount * 3;
		m_indexBuffer = objFac->CreateIndexBuffer(IBT_Bit32, indexCount, BU_WriteOnly);
		
		uint* idxData = reinterpret_cast<uint*>(m_indexBuffer->Lock(LOCK_None));

		terrSize++;

		int idx = 0;
		for (int i=0;i<terrSize-1;i++)
		{
			int remi = i % 2;

			for (int j = 0; j < terrSize - 1; j++)
			{
				int remj = j % 2;
				if (remi == remj)
				{
					idxData[idx++] = i * terrSize + j;
					idxData[idx++] = (i + 1) * terrSize + j;
					idxData[idx++] = i * terrSize + (j + 1);

					idxData[idx++] = i * terrSize + (j + 1);
					idxData[idx++] = (i + 1) * terrSize + j;
					idxData[idx++] = (i + 1) * terrSize + (j + 1);
				}
				else
				{
					idxData[idx++] = i * terrSize + j;
					idxData[idx++] = (i + 1) * terrSize + (j + 1);
					idxData[idx++] = i * terrSize + (j + 1);

					idxData[idx++] = i * terrSize + j;
					idxData[idx++] = (i + 1) * terrSize + j;
					idxData[idx++] = (i + 1) * terrSize + (j + 1);
				}
			}
		}
		m_indexBuffer->Unlock();
	}
	SharedIndexData::~SharedIndexData()
	{
		delete m_indexBuffer;
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

}