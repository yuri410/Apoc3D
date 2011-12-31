#include "Terrain.h"
#include "TerrainMesh.h"
#include "Core/ResourceHandle.h"

namespace SampleTerrain
{
	const float Terrain::CellLength = 2;
	const float Terrain::BlockLength = CellLength * TerrainEdgeLength;
	const float Terrain::HeightScale = 75;
	PerlinNoise Terrain::Noiser(0.4, 0.01, 1, 8, 8881);
	//PerlinNoise Terrain::Noiser(0.42, 0.015, 1, 8, 8881);

	Terrain::Terrain(RenderDevice* device, int bx, int bz)
	{
		m_BoundingSphere = GetBoundingSphere(bx, bz);
		m_transformation.LoadIdentity();
		//m_transformation.SetTranslation(Vector3Utils::LDVector(bx * BlockLength, 0, bz*BlockLength));

		for (int i=0;i<1;i++)
		{
			m_terrains[i] = TerrainMeshManager::getSingleton().CreateInstance(device, bx,bz, i);
		}
	}

	Terrain::~Terrain()
	{
		for (int i=0;i<3;i++)
		{
			delete m_terrains[i];
		}
	}

	RenderOperationBuffer* Terrain::GetRenderOperation(int lod)
	{
		//return 0;
		ResourceHandle<TerrainMesh>& mesh = *m_terrains[0];
		return mesh->GetRenderOperation(0);
	}

	void Terrain::Update(const GameTime* const time)
	{

	}

	float Terrain::GetHeightAt(float x, float z)
	{
		float h = (float)Noiser.GetHeight(x,z);
		if (h<-0.5f)
			h=-0.5f;
		return h;
	}
}