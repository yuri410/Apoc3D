#include "Terrain.h"
#include "TerrainMesh.h"
#include "Core/ResourceHandle.h"

namespace SampleTerrain
{
	const float Terrain::CellLength = 2;
	const float Terrain::BlockLength = CellLength * TerrainEdgeLength;
	const float Terrain::HeightScale = 75;
	PerlinNoise Terrain::Noiser(0.42, 0.01, 1, 8, 8881);
	PerlinNoise Terrain::NoiserPlant(0.42, 1, 1, 1, 8881);
	//PerlinNoise Terrain::Noiser(0.42, 0.01, 1, 8, 8881);

	Terrain::Terrain(RenderDevice* device, int bx, int bz)
	{
		m_BoundingSphere = GetBoundingSphere(bx, bz);
		m_transformation.LoadIdentity();
		//m_transformation.SetTranslation(Vector3Utils::LDVector(bx * BlockLength, 0, bz*BlockLength));

		for (int i=0;i<4;i++)
		{
			m_terrains[i] = TerrainMeshManager::getSingleton().CreateInstance(device, bx,bz, i);
		}
	}

	Terrain::~Terrain()
	{
		for (int i=0;i<4;i++)
		{
			delete m_terrains[i];
		}
	}

	RenderOperationBuffer* Terrain::GetRenderOperation(int lod)
	{
		if (lod>=4)
			lod = 3;

		RenderOperationBuffer* opBuf = 0;
		while (!opBuf && lod<4)
		{
			ResourceHandle<TerrainMesh>& mesh = *m_terrains[lod];
			opBuf = mesh->GetRenderOperation(lod);
			if (opBuf)
				return opBuf;
			lod++;
		}
		return 0;
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
	float Terrain::GetPlantDist(float x, float z)
	{
		return (float)NoiserPlant.Noise((int)x,(int)z);
	}
	float Terrain::GetNoise(int x, int y)
	{
		return (float) Noiser.Noise(x,y);
	}
}