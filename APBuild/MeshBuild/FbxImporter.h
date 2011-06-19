#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H
#include "APBCommon.h"

#include <fbxsdk.h>

#include "Collections/FastList.h"
#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "IOLib/MaterialData.h"
#include "Graphics/Animation/AnimationTypes.h"

#define MAXBONES_PER_VERTEX 4

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::Animation;

namespace APBuild
{
	class FbxImporter
	{
	public:
		struct BoneWeight
		{
			std::vector<std::pair<int, float>> BoneWeights;

			void NormalizeInPlace()
			{
				float total = 0.0f;
				int nIndex = 0;

				for (std::vector<std::pair<int,float>>::iterator iter = BoneWeights.begin();
					iter != BoneWeights.end(); )
				{
					if( nIndex >= MAXBONES_PER_VERTEX )
					{
						iter = BoneWeights.erase(iter);
					}
					else
					{
						total += iter->second;
						nIndex++;
						iter++;
					}
				}


				float fScale = 1.0f/total;
				for (std::vector<std::pair<int,float>>::iterator iter = BoneWeights.begin();
					iter != BoneWeights.end(); iter++)
				{
					iter->second *= fScale;
				}

			}

			void AddBoneWeight(const BoneWeight& boneWeights)
			{
				for( size_t i = 0; i < boneWeights.BoneWeights.size(); ++i)	
				{
					const std::pair<int, float> e = boneWeights.BoneWeights[i];
					AddBoneWeight(e.first, e.second);
				}
			}
			void AddBoneWeight(int boneIndex, float boneWeight)
			{
				float passed = false;
				for(std::vector<std::pair<int,float>>::iterator it = BoneWeights.begin(); 
					it != BoneWeights.end(); ++it )
				{
					if( boneWeight > it->second )
					{
						BoneWeights.insert(it, std::make_pair(boneIndex, boneWeight));
						passed = true;
						break;
					}
				}
				if( !passed )
					BoneWeights.push_back(std::make_pair(boneIndex,boneWeight));
			}
		};

		class FIMesh;
		class AnimationKeyFrames;

		class FIMeshPart
		{
		private:
			std::vector<Vector3> m_Positions;
			std::vector<Vector3> m_Normals;
			std::vector<Vector2> m_TexCoords;
			std::vector<uint32> m_VertexIndices;
			std::vector<BoneWeight> m_BoneWeights;
			MaterialData* m_pMaterial;

			FIMesh* m_pModelParent;

			bool m_bSkinnedModel;
		public:
			FIMeshPart(FIMesh* pModelParent, MaterialData* pMaterial)
				: m_pModelParent(pModelParent), m_pMaterial(pMaterial),
				m_bSkinnedModel(false)
			{

			}
			~FIMeshPart() { }

			void AddVertex(const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord, 
				const BoneWeight& boneWeights)
			{
				m_Positions.push_back(vPosition);
				m_Normals.push_back(vNormal);
				m_TexCoords.push_back(vTexCoord);
				m_BoneWeights.push_back(boneWeights);

				m_VertexIndices.push_back(m_VertexIndices.size());
				if (boneWeights.BoneWeights.size())
					m_bSkinnedModel = true;
			}
			void FinishAndOptimize()
			{

			}

			inline MaterialData* GetMaterial() { return m_pMaterial; }

			//void InitializeDeviceObjects(ID3D10Device* pd3dDevice);
			//void ReleaseDeviceObjects();

		};

		class FIMesh
		{
		private:
			std::vector<FIMeshPart*> m_ModelParts;

			FastMap<string, AnimationKeyFrames*> m_AnimationKeyFrames;

			std::string m_strName;

			Matrix m_matAbsoluteTransform;
			Matrix m_matGeometricOffset;
			Matrix m_matAnimationTransform;
		public:
			FIMesh(const std::string& strName)
				: m_strName(strName)
			{
				m_matAbsoluteTransform = Matrix::Identity;
				m_matGeometricOffset = Matrix::Identity;
				m_matAbsoluteTransform = Matrix::Identity;
			}
			~FIMesh()
			{
				for (size_t i=0;i<m_ModelParts.size();i++)
				{
					delete m_ModelParts[i];
				}
			}

			void AddVertex(MaterialData* pMaterial, const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord, const BoneWeight& boneWeights)
			{
				bool bNewMaterial = true;

				for( size_t i = 0; i < m_ModelParts.size(); ++i )
				{
					FIMeshPart* pModelPart = m_ModelParts[i];
					if( pMaterial == pModelPart->GetMaterial() )
					{
						pModelPart->AddVertex(vPosition, vNormal, vTexCoord, boneWeights);
						bNewMaterial = false;
					}
				}

				if( bNewMaterial )
				{
					FIMeshPart* pModelPart = new FIMeshPart(this, pMaterial);
					pModelPart->AddVertex(vPosition, vNormal, vTexCoord, boneWeights);
					m_ModelParts.push_back(pModelPart);
				}
			}

			void FinishAndOptimize()
			{
				//for( int i = 0; i < m_ModelParts.GetSize(); ++i )
				//{
				//	m_ModelParts.GetAt(i)->Optimize();
				//}
			}

			//void InitializeDeviceObjects(ID3D10Device* pd3dDevice);
			//void ReleaseDeviceObjects();

			void AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames)
			{
				m_AnimationKeyFrames.Add(pAnimationKeyFrames->GetAnimationName(),
					pAnimationKeyFrames);
			}
			AnimationKeyFrames* GetAnimationKeyFrames(const std::string& strAnimationName)
			{
				return m_AnimationKeyFrames[strAnimationName];
			}

			//void UpdateAnimation(CBTTAnimationController* pAnimationController);
			//void Render(ID3D10Device* pd3dDevice, CBTTEffect* pBTTEffect, bool bEnableAnimation);

			void SetAbsoluteTransform(const Matrix& matAbsoluteTransform)   { m_matAbsoluteTransform = matAbsoluteTransform; }
			void SetGeometricOffset(const Matrix& matGeometricOffset)		{ m_matGeometricOffset = matGeometricOffset; }
			const Matrix& GetAbsoluteTransform() const { return m_matAbsoluteTransform; }
			const Matrix& GetGeometricOffset() const	{ return m_matGeometricOffset; }
			const Matrix& GetAnimationTransform() const { return m_matAnimationTransform; }

			const std::string& GetName() const { return m_strName; }
		};

		class AnimationKeyFrames
		{
		protected:
			std::string m_strAnimationName;
			std::vector<Matrix> m_KeyFrames;
		public:
			AnimationKeyFrames(const std::string& strAnimationName)
				: m_strAnimationName(strAnimationName)
			{

			}
			~AnimationKeyFrames() { }

			void AddKeyFrame(const Matrix& matTransform)
			{
				m_KeyFrames.push_back(matTransform);
			}

			const std::string& GetAnimationName() const { return m_strAnimationName; }

			const Matrix& GetKeyFrameTransform(int nKeyFrame) { return m_KeyFrames[nKeyFrame]; }
			//const CBTTAnimationQuaternionKeyFrame& GetKeyFrameQuaternion(int nKeyFrame);
		};
		class SkeletonBone
		{
		private:

			std::string m_strName;

			Matrix m_matBindPoseTransform;
			Matrix m_matInvBindPoseTransform;
			Matrix m_matBoneReferenceTransform;
			Matrix m_matInvBoneReferenceTransform;

			int m_nParentBoneIndex;

			FastMap<std::string, AnimationKeyFrames*> m_AnimationKeyFrames;
		public:
			SkeletonBone(std::string strName, int nParentBoneIndex)
				: m_strName(strName), m_nParentBoneIndex(nParentBoneIndex)
			{
				m_matBindPoseTransform = Matrix::Identity;
				m_matInvBindPoseTransform = Matrix::Identity;
			}
			~SkeletonBone()
			{
				m_AnimationKeyFrames.Clear();
			}


			void AddAnimationKeyFrames(AnimationKeyFrames* pAnimationKeyFrames)
			{
				m_AnimationKeyFrames.Add(pAnimationKeyFrames->GetAnimationName(), pAnimationKeyFrames);
			}

			void SetBindPoseTransform(const Matrix& matBindPoseTransform)
			{
				m_matBindPoseTransform = matBindPoseTransform;
				Matrix::Inverse(m_matInvBindPoseTransform, matBindPoseTransform);
			}
			void SetBoneReferenceTransform(const Matrix& matBoneReferenceTransform)
			{
				m_matBoneReferenceTransform = matBoneReferenceTransform;
				Matrix::Inverse(m_matInvBoneReferenceTransform, matBoneReferenceTransform);
			}

			AnimationKeyFrames* GetAnimationKeyFrames(const std::string strAnimationName)
			{
				AnimationKeyFrames* result;
				if (m_AnimationKeyFrames.TryGetValue(strAnimationName, result))
					return result;
				return 0;
			}

			const Matrix& GetBindPoseTransform()			{ return m_matBindPoseTransform; }
			const Matrix& GetInvBindPoseTransform()			{ return m_matInvBindPoseTransform; }
			const Matrix& GetBoneReferenceTransform()		{ return m_matBoneReferenceTransform; }
			const Matrix& GetInvBoneReferenceTransform()	{ return m_matInvBoneReferenceTransform; }

			const std::string& GetName() const				{ return m_strName; }
			int GetParentBoneIndex() const					{ return m_nParentBoneIndex; }
		};
		class Skeleton
		{
		private:
			//void BuildBoneTransforms(CBTTAnimationController* pAnimationController, const std::string& strAnimationName);
			//void BuildBoneTransforms();

			FastList<SkeletonBone*> m_SkeletonBones;
			Matrix* m_SkinTransforms;
			Matrix* m_BoneAnimationTransforms;
			int m_nBoneCount;
		public:

			Skeleton()
				: m_SkinTransforms(0), m_BoneAnimationTransforms(0), m_nBoneCount(0)
			{

			}
			~Skeleton()
			{
				if (m_BoneAnimationTransforms)
					delete[] m_SkinTransforms;
				if (m_BoneAnimationTransforms)
					delete[] m_BoneAnimationTransforms;
			}

			void AddSkeletonBone(SkeletonBone* pSkeletonBone)
			{
				m_SkeletonBones.Add(pSkeletonBone);
				m_nBoneCount++;
			}
			SkeletonBone* FindBone(const std::string& strBoneName)
			{
				for (int i=0;i<m_SkeletonBones.getCount();i++)
				{
					if (m_SkeletonBones[i]->GetName() == strBoneName)
					{
						return m_SkeletonBones[i];
					}
				}

				return 0;
			}
			int FindBoneIndex(const std::string& strBoneName)
			{
				for (int i=0;i<m_SkeletonBones.getCount();i++)
				{
					if (m_SkeletonBones[i]->GetName() == strBoneName)
					{
						return i;
					}
				}

				return -1;
			}

			SkeletonBone* GetSkeletonBone(int nIndex)
			{
				return m_SkeletonBones[nIndex];
			}
			//const FastMap<string, SkeletonBone*>& GetSkeletonBones() const { return m_SkeletonBones; }
			Matrix* GetSkinTransforms() { return m_SkinTransforms; }
			int GetBoneCount() const 	{ return m_nBoneCount; }

			//void UpdateAnimation(CBTTAnimationController* pAnimationController);
		};
	private:
		KFbxSdkManager* m_pFBXSdkManager;
		KFbxScene* m_pFBXScene;

		FastList<KFbxSurfaceMaterial*> m_FBXMaterials;
		FastList<MaterialData*> m_materials;
		//FastList<MeshData*> m_meshes;
		FastMap<string, FIMesh*> m_meshes;
		Skeleton* m_pSkeleton;

		bool InitializeFBXSdk();
		bool LoadScene(const String& pFilename);

		void ProcessScene(KFbxScene* pScene);
		void ProcessMaterials(KFbxScene* pScene);
		void ProcessAnimations(KFbxScene* pScene);
		void ProcessAnimation(KFbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop);
		void ProcessNode(KFbxNode* pNode, KFbxNodeAttribute::EAttributeType attributeType);
		void ProcessSkeleton(KFbxNode* pNode);
		void ProcessMesh(KFbxNode* pNode);

		void ProcessBoneWeights(KFbxMesh* pFBXMesh, std::vector<BoneWeight>& meshBoneWeights);
		void ProcessBoneWeights(KFbxSkin* pFBXSkin, std::vector<BoneWeight>& meshBoneWeights);


		MaterialData* GetMaterialLinkedWithPolygon(KFbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		Vector2 GetTexCoord(KFbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		int GetMappingIndex(KFbxLayerElement::EMappingMode MappingMode, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		MaterialData* GetMaterial(KFbxSurfaceMaterial* pFBXMaterial);
		Matrix GetAbsoluteTransformFromCurrentTake(KFbxNode* pNode, KTime time);
		Matrix GetGeometricOffset(KFbxNode* pNode);
		Vector3 GetMaterialColor(KFbxPropertyDouble3 FBXColorProperty, KFbxPropertyDouble1 FBXFactorProperty);


		bool IsSkeletonRoot(KFbxNode* pNode);


	public:
		bool Initialize(const String& pFilename);
		
	
	};
}
#endif