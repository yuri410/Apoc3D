#ifndef TERRAIN_H
#define TERRAIN_H

#include "TerrainCommon.h"
#include "Scene/SceneObject.h"
#include "Core/Resource.h"
#include "Core/ResourceManager.h"
#include "Math/BoundingSphere.h"
#include "Math/PerlinNoise.h"
#include "Graphics/RenderOperationBuffer.h"

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

		void Push(Vector3 pos, bool isFastPushing);

		static void NewSeed();
		static void GetBlockCoordinate(const Vector3 pos, int& bx, int& bz)
		{
			bx = (int)floor(_V3X(pos)/BlockLength);
			bz = (int)floor(_V3Z(pos)/BlockLength);
		}
		static BoundingSphere GetBoundingSphere(int bx, int bz)
		{
			return BoundingSphere(Vector3Utils::LDVector((bx+0.5f) * BlockLength, 0, (bz+0.5f)*BlockLength), BlockLength * Math::Root2 * 0.5f);
		}
		static float GetPlantDist(float x, float z);
		static float GetHeightAt(float x, float z);
		static float GetNoise(int x, int y);
	private:
		struct TreeInfo
		{
			float Rot;
			float Height;
			Vector3 Position;
			float FallState;
			Vector3 FallAxis;
			bool IsFastFalling;
			//float FallDirection;

			void CalculateTransform(Matrix& result) const
			{
				Matrix rotation;
				Matrix::CreateRotationY(rotation, Rot);

				float yOffset = 0;
				if (FallState > 0)
				{
					float rot = FallState;
					if (rot > Math::Half_PI)
					{
						yOffset += rot - Math::Half_PI;
						if (yOffset>150)
							yOffset = 150;

						rot = Math::Half_PI;
					}

					Matrix fallRot;
					Matrix::CreateRotationAxis(fallRot, FallAxis, -rot);
					Matrix temp = rotation;
					Matrix::Multiply(rotation, temp, fallRot);
				}

				
				Matrix scaling;
				Matrix::CreateScale(scaling, 1, Height, 1);

				Matrix::Multiply(result, rotation, scaling);

				result.SetTranslation(Position);
				result.Elements[4*3+2-1] -= yOffset;
			}
		};


		FastList<TreeInfo> m_trees;

		ResourceHandle<TerrainMesh>* m_terrains[4];
		
		int m_bufferedLevel;
		RenderOperationBuffer m_treeOPBuffer;
		bool m_treeROPDirty;

		bool m_isPushing;
		bool m_isFastPushing;
		Vector3 m_pushingPos;

		bool RebuildROPBuffer(int lod);
		void MakeTree(float x, float y, float z);

		static RenderOperationBuffer m_opBuffer;
		static PerlinNoise Noiser;
		//static PerlinNoise NoiserPlant;
	};
}

#endif