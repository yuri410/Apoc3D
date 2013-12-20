#include "Terrain.h"
#include "TerrainMesh.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Graphics/Model.h"

namespace SampleTerrain
{
	const float Terrain::CellLength = 2;
	const float Terrain::BlockLength = CellLength * TerrainEdgeLength;
	const float Terrain::HeightScale = 75;
	PerlinNoise Terrain::Noiser(0.42, 0.01, 1, 8, 8881);
	RenderOperationBuffer Terrain::m_opBuffer;
	

	Terrain::Terrain(RenderDevice* device, int bx, int bz)
		: m_treeROPDirty(true), m_bufferedLevel(0), m_isPushing(false)
	{
		m_BoundingSphere = GetBoundingSphere(bx, bz);
		m_transformation.LoadIdentity();
		
		// create TerrainMesh instances from the manager for different LODs.
		for (int i=0;i<4;i++)
		{
			m_terrains[i] = TerrainMeshManager::getSingleton().CreateInstance(device, bx,bz, i);
		}

		// Now generate the trees' position, rotation, height one by one.

		// reserve the list's space to an average amount of trees; so resizing will be limited.
		m_trees.ResizeDiscard(300);

		// the cellLength here is use to for trees' distribution only.
		// The chunk is divided into rows and columns of quad areas with a size of 
		// cellLength by cellLength. One tree will be spawned in the area at most dependents
		// on a probability calculated later.
		// this has nothing to do with the terrain mesh's cell in this method
		float cellLength = 8;

		int treeCellEdgeCount = (int)(Terrain::BlockLength/cellLength);
		for (int i=0;i<treeCellEdgeCount;i++)
		{
			for (int j=0;j<treeCellEdgeCount;j++)
			{
				float worldX = (i+0.5f)*cellLength + bx*Terrain::BlockLength;
				float worldZ = (j+0.5f)*cellLength + bz*Terrain::BlockLength;

				// two additional sample are required to check the gradient at current position
				// the gradient is used to modulate the chance whether a tree is spawned here.
				float height = Terrain::GetHeightAt(worldX, worldZ);
				float refheight2 = Terrain::GetHeightAt(worldX+1, worldZ+1);

				// the probability of a tree's appearance at the current position.
				// the p here is inverted, the real probability should be 1-p
				//
				// the first half of the expression calculate the inv-chance by elevation, a elevation closer to 0.0
				// is better place to put trees. The second part take the slope into consideration, 
				// a higher different in height results higher inv-chance, which in turn make the chance smaller
				float p = Math::Saturate(fabs(height - ( 0.00f)) / (0.25f) + 
					powf(fabs(refheight2-height)*1.414f * HeightScale * 1.5f,8));

				// some offset in position to make the tree's are not planted in rows or lines.
				float ofX = Terrain::GetNoise((int)worldX, (int)worldZ);
				float ofZ = Terrain::GetNoise((int)worldX+65535, (int)worldZ);

				worldX += ofX*cellLength*0.5f; worldZ += ofZ*cellLength*0.5f;

				// compare the probability. A lower P is easier to meet.
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
		m_opBuffer.Clear();
		if (lod>=4)
			lod = 3;

		// do a render operation request from the TerrainMeshes, with an order
		// from higher lod number to lower. 
		// (The LOD index or LOD number means a more rough when the number is higher, and when 0 it mean the most detailed model.)
		// 
		// Now, if a resource is not loaded, according to the protocol, the returned RenderOperationBuffer
		// should be 0. As the matter of fact that rough model(higher LOD number) is usually loaded before the detailed one, 
		// and the processing takes less time. The strategy trying from higher LOD to lower ones ensures the terrain can be
		// showed up as soon as possible if not loaded yet. And minimize the waiting time during which may cause the terrain chunk
		// translucent.
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


			// when the lod changes from frame to frame, the pre-built RenderOperations need to be
			// rebuilt as the tree models used is different.
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
				// release the pre-built RenderOperations when lod is far( equals to 3)
				if (m_treeOPBuffer.getCount() > 10)
				{
					m_treeOPBuffer.Clear();
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
			// do the tree falling when the camera is inside

			const float thres = m_isFastPushing ? 256.0f : 64.0f;

			for (int i=0;i<m_trees.getCount();i++)
			{
				if (m_trees[i].FallState == 0)
				{
					float d2 = Vector3::DistanceSquared(m_trees[i].Position, m_pushingPos);
					if (d2 < thres)
					{
						Vector3 d = Vector3::Subtract(m_trees[i].Position, m_pushingPos);
						d.NormalizeInPlace();

						m_trees[i].FallAxis = Vector3::Cross(d, Vector3::UnitY);
						m_trees[i].FallAxis = Vector3::Normalize(m_trees[i].FallAxis);

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
			m_treeOPBuffer.Clear();

			Model* tree = TerrainMeshManager::getSingleton().getTreeModelByLOD(lod);
			for (int i=0;i<m_trees.getCount();i++)
			{
				const TreeInfo& info = m_trees[i];

				RenderOperationBuffer* opbuf = tree->GetRenderOperation(0);
				if (opbuf && opbuf->getCount())
				{
					Matrix trans;
					info.CalculateTransform(trans);

					m_treeOPBuffer.AddBuffer(opbuf, trans);
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

		// the pre-built RenderOperations will rebuild every frame when pushing, to animate the 
		// trees' falling
		m_treeROPDirty = true;
	}

	void Terrain::MakeTree(float x, float y, float z)
	{
		TreeInfo info;
		info.Height = Randomizer::NextFloat() * 0.1f + 0.9f;
		info.Rot = Randomizer::NextFloat() * Math::PI * 2;
		info.Position = Vector3(x,y,z);
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