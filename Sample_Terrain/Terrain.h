#ifndef TERRAIN_H
#define TERRAIN_H

#include "TerrainCommon.h"
#include "Scene/SceneObject.h"
#include "Core/Resource.h"
#include "Core/ResourceManager.h"
#include "Math/BoundingSphere.h"
#include "Math/PerlinNoise.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Scene;
using namespace Apoc3D::Math;

namespace SampleTerrain
{
	class Terrain : public SceneObject
	{
	public:
		static const float CellLength;

		static const int TerrainEdgeLength = 128;
		static const float BlockLength;
		static const float HeightScale;
		
		Terrain(RenderDevice* device, int bx, int bz);
		~Terrain();

		virtual RenderOperationBuffer* GetRenderOperation(int level);
		virtual void Update(const GameTime* const time);
		static BoundingSphere GetBoundingSphere(int bx, int bz)
		{
			return BoundingSphere(Vector3Utils::LDVector((bx+0.5f) * BlockLength, 0, (bz+0.5f)*BlockLength), BlockLength * Math::Root2 * 0.5f);
		}
		static float GetPlantDist(float x, float z);
		static float GetHeightAt(float x, float z);
		static float GetNoise(int x, int y);
	private:
		ResourceHandle<TerrainMesh>* m_terrains[3];
		
		static PerlinNoise Noiser;
		static PerlinNoise NoiserPlant;
	};
}

#endif