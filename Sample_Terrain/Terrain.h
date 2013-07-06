#ifndef TERRAIN_H
#define TERRAIN_H

#include "TerrainCommon.h"
#include "apoc3d/Scene/SceneObject.h"
#include "apoc3d/Core/Resource.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/PerlinNoise.h"
#include "apoc3d/Graphics/RenderOperationBuffer.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Scene;
using namespace Apoc3D::Math;

namespace SampleTerrain
{
	/** A terrain block/chunk with a size of BlockLength by BlockLength.
	 *  A Terrain object always has a mesh and a list of trees' info with it.
	 *  
	 * *Trees vs TerrainMesh:
	 *  Tree's info is generated once the terrain object is
	 *  constructed, while the terrain mesh is treated
	 *  as a resources, which is handle by TerrainMeshManager.
	 *  As the whole world have around more than 1 million 
	 *  tree, the majority memory consumption is caused by
	 *  these pre-generated trees. And such pre-generation is for the easy
	 *  implementation.
	 *  
	 *  TerrainMesh are loaded around the time been seen if 
	 *  it is not loaded, and unloaded when not seen for
	 *  a long time.
	 *
	 * *Level of Detail:
	 *  A terrain chunk has 4 terrain mesh of the same place 
	 *  but with different level of details. The level of details are
	 *  controlled by the OctreeSceneManager's decision, 
	 *  by giving an index. The distant mesh is very rough. Two benefits comes. 
	 *  Reducing primitives and when it is used as an async 
	 *  resources, the processing can be done instantly.
	 *  The LOD index or LOD number means a more rough when 
	 *  the number is higher, and when 0 it mean the most detailed model.
	 *
	 * *Trivia:
	 *  RenderOperation buffer for trees are pre-calculated 
	 *  once the chunk is has higher LOD, and releases when the LOD drops too low.
	 *
	 *  The coordinates of a terrain chunk is measured by 
	 *  terrain chunk space coords. Terrain::GetBlockCoordinate does 
	 *  the conversion from this coords to the world space.
	 */
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

		/** Notifies the terrain block that the camera is currently in side the chunk that tree pushing should be considered.
		*/
		void Push(Vector3 pos, bool isFastPushing);

		/** Generates a new seed for the Perlin Noise.
		*/
		static void NewSeed();
		/** Calculates the world coordinate for the given terrain chunk space coords.
		*/
		static void GetBlockCoordinate(const Vector3 pos, int& bx, int& bz)
		{
			bx = (int)floor(pos.X/BlockLength);
			bz = (int)floor(pos.Z/BlockLength);
		}
		/** Calculates the bounding sphere in world space for a terrain chunk at given terrain chunk space coords.
		*/
		static BoundingSphere GetBoundingSphere(int bx, int bz)
		{
			return BoundingSphere(Vector3((bx+0.5f) * BlockLength, 0, (bz+0.5f)*BlockLength), BlockLength * Math::Root2 * 0.5f);
		}
		/** Gets a noise value based on world coordinates
		*/
		static float GetPlantDist(float x, float z);
		/** Gets the height generated by Perlin Noise at any given world coordinate.
		*/
		static float GetHeightAt(float x, float z);
		/** Gets a noise value based on coordinates.
		*/
		static float GetNoise(int x, int y);

		static PerlinNoise& GetNoiseGenerator() { return Noiser; }
	private:
		struct TreeInfo
		{
			/** Per-instance random Y-rotation angle, to make things 
			 *  natural.
			 */
			float Rot;
			/** Per-instance random height scale, to make things 
			 *  natural
			 */
			float Height;
			Vector3 Position;

			/** A number representing the tree's current falling
			 *  angle or the distance of "down-moving" into the earth.
			 * 
			 *  This is converted mathematically as a param to the transform.
			 */

			float FallState;
			/** The rotation axis for the tree's falling animation
			*/
			Vector3 FallAxis;
			/** Indicates whether the tree is falling fast.
			 *  This is set to true when sprinting.
			 */
			bool IsFastFalling;

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
		/** Randomly create a new tree near the given location
		*/
		void MakeTree(float x, float y, float z);


		static RenderOperationBuffer m_opBuffer;
		static PerlinNoise Noiser;
		//static PerlinNoise NoiserPlant;
	};
}

#endif