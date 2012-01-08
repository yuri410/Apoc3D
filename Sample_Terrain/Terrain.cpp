#include "Terrain.h"
#include "TerrainMesh.h"
#include "Core/ResourceHandle.h"
#include "Core/GameTime.h"
#include "Math/RandomUtils.h"
#include "Graphics/Model.h"

namespace SampleTerrain
{
	const float Terrain::CellLength = 2;
	const float Terrain::BlockLength = CellLength * TerrainEdgeLength;
	const float Terrain::HeightScale = 75;
	PerlinNoise Terrain::Noiser(0.42, 0.01, 1, 8, 8881);
	RenderOperationBuffer Terrain::m_opBuffer;
	//PerlinNoise Terrain::Noiser(0.42, 0.01, 1, 8, 8881);

	Terrain::Terrain(RenderDevice* device, int bx, int bz)
		: m_treeROPDirty(true), m_bufferedLevel(0), m_isPushing(false)
	{
		m_BoundingSphere = GetBoundingSphere(bx, bz);
		m_transformation.LoadIdentity();
		//m_transformation.SetTranslation(Vector3Utils::LDVector(bx * BlockLength, 0, bz*BlockLength));

		//m_treeOPBuffer.ReserveDiscard(300);

		for (int i=0;i<4;i++)
		{
			m_terrains[i] = TerrainMeshManager::getSingleton().CreateInstance(device, bx,bz, i);
		}

		m_trees.ResizeDiscard(300);
		float cellLength = 8;

		int treeCellEdgeCount = (int)(Terrain::BlockLength/cellLength);
		for (int i=0;i<treeCellEdgeCount;i++)
		{
			for (int j=0;j<treeCellEdgeCount;j++)
			{
				float worldX = (i+0.5f)*cellLength + bx*Terrain::BlockLength;
				float worldZ = (j+0.5f)*cellLength + bz*Terrain::BlockLength;

				float height = Terrain::GetHeightAt(worldX, worldZ);
				//float refheight1 = Terrain::GetHeightAt(worldX+1, worldZ);
				float refheight2 = Terrain::GetHeightAt(worldX+1, worldZ+1);

				//float p = Math::Saturate(fabs(height - ( 0.00f)) / (0.25f) + 
				//	powf((fabs(refheight1-height) + fabs(refheight2-height)) * HeightScale * 1.5f,8));
				float p = Math::Saturate(fabs(height - ( 0.00f)) / (0.25f) + 
					powf(fabs(refheight2-height)*1.414f * HeightScale * 1.5f,8));

				float ofX = Terrain::GetNoise((int)worldX, (int)worldZ);
				float ofZ = Terrain::GetNoise((int)worldX+65535, (int)worldZ);

				worldX += ofX*cellLength*0.5f; worldZ += ofZ*cellLength*0.5f;

				if (Terrain::GetPlantDist(worldX, worldZ) > p)
				{
					MakeTree(worldX, height * HeightScale-0.5f, worldZ);
				}
			}
		}
		m_trees.Trim();
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
		m_opBuffer.FastClear();
		if (lod>=4)
			lod = 3;

		RenderOperationBuffer* opBuf = 0;
		while (!opBuf && lod<4)
		{
			ResourceHandle<TerrainMesh>& mesh = *m_terrains[lod];
			opBuf = mesh->GetRenderOperation(lod);
			if (opBuf)
				break;
			lod++;
		}


		if (opBuf)
		{
			if (opBuf->getCount())
				m_opBuffer.Add(&opBuf->operator[](0), opBuf->getCount());


			
			if (lod != 3 && m_bufferedLevel != lod)
			{
				m_treeROPDirty = true;
			}

			if (lod != 3)
			{
				if (m_treeROPDirty)
				{
					if (RebuildROPBuffer(lod))
						m_treeROPDirty = false;
				}

				if (m_treeOPBuffer.getCount())
				{
					m_opBuffer.Add(&m_treeOPBuffer[0], m_treeOPBuffer.getCount());
				}
			}
			else
			{
				if (m_treeOPBuffer.getCount() > 10)
				{
					m_treeOPBuffer.FastClear();
					m_treeOPBuffer.ReserveDiscard(4);
					m_treeROPDirty = true;
				}
			}
		}

		return &m_opBuffer;
	}

	
	void Terrain::Update(const GameTime* const time)
	{
		if (m_isPushing)
		{
			const float thres = m_isFastPushing ? 256.0f : 64.0f;

			for (int i=0;i<m_trees.getCount();i++)
			{
				if (m_trees[i].FallState == 0)
				{
					float d2 = Vector3Utils::DistanceSquared(m_trees[i].Position, m_pushingPos);
					if (d2 < thres)
					{
						Vector3 d = Vector3Utils::Subtract(m_trees[i].Position, m_pushingPos);
						d = Vector3Utils::Normalize(d);

						m_trees[i].FallAxis = Vector3Utils::Cross(d, Vector3Utils::UnitY);
						m_trees[i].FallAxis = Vector3Utils::Normalize(m_trees[i].FallAxis);

						//Vector3 bi = Vector3Utils::Cross(Vector3Utils::UnitY, m_trees[i].FallAxis);
						//m_trees[i].FallDirection = Vector3Utils::Dot(bi, d);
						m_trees[i].IsFastFalling = m_isFastPushing;
						m_trees[i].FallState = 0.01f;
					}
				}
				else
				{
					if (m_trees[i].IsFastFalling)
					{
						m_trees[i].FallState += time->getElapsedTime() * 9;	
					}
					else
					{
						m_trees[i].FallState += time->getElapsedTime() * 3;	
					}
					
				}
			}
			m_isPushing = false;
		}

	}
	bool Terrain::RebuildROPBuffer(int lod)
	{
		bool passed = true;

		{
			m_treeOPBuffer.FastClear();

			Model* tree = TerrainMeshManager::getSingleton().getTreeModelByLOD(lod);
			for (int i=0;i<m_trees.getCount();i++)
			{
				const TreeInfo& info = m_trees[i];

				RenderOperationBuffer* opbuf = tree->GetRenderOperation(0);
				if (opbuf && opbuf->getCount())
				{
					Matrix trans;
					info.CalculateTransform(trans);

					m_treeOPBuffer.Add(&opbuf->operator[](0), opbuf->getCount(), trans);
				}
				else
				{
					passed = false;
				}
			}
		}
		m_bufferedLevel = lod;
		return passed;
	}

	void Terrain::Push(Vector3 pos, bool isFastPushing)
	{
		m_isFastPushing = isFastPushing;
		m_isPushing = true;
		m_pushingPos = pos;
		m_treeROPDirty = true;
	}

	void Terrain::MakeTree(float x, float y, float z)
	{
		TreeInfo info;
		info.Height = Randomizer::NextFloat() * 0.1f + 0.9f;
		info.Rot = Randomizer::NextFloat() * Math::PI * 2;
		info.Position = Vector3Utils::LDVector(x,y,z);
		info.FallState = 0;
		m_trees.Add(info);
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
		return (float)Noiser.Noise((int)x,(int)z);
	}
	float Terrain::GetNoise(int x, int y)
	{
		return (float) Noiser.Noise(x,y);
	}
	void Terrain::NewSeed()
	{
		Noiser.SetRandomSeed(Randomizer::Next());
	}
}