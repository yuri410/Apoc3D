#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H

#include "../APBCommon.h"

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4512)
#endif

#include <fbxsdk.h>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/ExistTable.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Graphics/Animation/AnimationTypes.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/Utility/StringUtils.h"

#define MAXBONES_PER_VERTEX 4

#include <vector>

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Utility;

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

			Vector4 GetBlendIndex() const
			{
				float v[4] = {0};
				for (size_t i=0;i<BoneWeights.size();i++)
				{
					v[i] = static_cast<float>( BoneWeights[i].first);
				}
				return Vector4Utils::LDVectorPtr(v);
			}
			Vector4 GetBlendWeight() const
			{
				float v[4] = {0};
				for (size_t i=0;i<BoneWeights.size();i++)
				{
					v[i] = BoneWeights[i].second;
				}
				return Vector4Utils::LDVectorPtr(v);
			}
		};

		class FIMesh;
		class FIPartialAnimation;
		class FISkeleton;

		/** A FBX importer mesh part has its own vertex data.
			It does not have index.
			The process of producing final result should generate indices and reduce duplicated vertices.
		*/
		class FIMeshPart
		{
		private:
			std::vector<Vector3> m_Positions;
			std::vector<Vector3> m_Normals;
			std::vector<Vector2> m_TexCoord0;
			std::vector<Vector2> m_TexCoord1;
			std::vector<uint32> m_VertexIndices;
			std::vector<BoneWeight> m_BoneWeights;
			MaterialData* m_pMaterial;

			FIMesh* m_pModelParent;

			bool m_bSkinnedModel;
		public:
			Matrix GeometricOffset;
			
			FIMeshPart(FIMesh* pModelParent, MaterialData* pMaterial)
				: m_pModelParent(pModelParent), m_pMaterial(pMaterial),
				m_bSkinnedModel(false)
			{
				assert(pMaterial);
			}
			~FIMeshPart() { }

			void AddVertex(const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord, 
				const BoneWeight& boneWeights)
			{
				m_Positions.push_back(vPosition);
				m_Normals.push_back(vNormal);
				m_TexCoord0.push_back(vTexCoord);
				m_BoneWeights.push_back(boneWeights);

				m_VertexIndices.push_back(m_VertexIndices.size());
				if (boneWeights.BoneWeights.size())
					m_bSkinnedModel = true;
			}
			void AddVertex(const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord0, const Vector2& vTexCoord1,
				const BoneWeight& boneWeights)
			{
				m_Positions.push_back(vPosition);
				m_Normals.push_back(vNormal);
				m_TexCoord0.push_back(vTexCoord0);
				m_TexCoord1.push_back(vTexCoord1);
				m_BoneWeights.push_back(boneWeights);

				m_VertexIndices.push_back(m_VertexIndices.size());
				if (boneWeights.BoneWeights.size())
					m_bSkinnedModel = true;
			}
			void FinishAndOptimize()
			{

			}

			bool IsSkinnedModel() const { return m_bSkinnedModel; }
			inline MaterialData* GetMaterial() const { return m_pMaterial; }
			const std::vector<Vector3>& getPosition() const { return m_Positions; }
			const std::vector<Vector3>& getNormal() const { return m_Normals; }
			const std::vector<Vector2>& getTexCoord0() const { return m_TexCoord0; }
			const std::vector<Vector2>& getTexCoord1() const { return m_TexCoord1; }
			const std::vector<uint32>& getVertexIndices() const { return m_VertexIndices; }
			const std::vector<BoneWeight>& getBoneWeights() const { return m_BoneWeights; }
			//void InitializeDeviceObjects(ID3D10Device* pd3dDevice);
			//void ReleaseDeviceObjects();

		};

		/** 
		*/
		class FIMesh
		{
		private:
			friend class FbxImporter;
			std::vector<FIMeshPart*> m_ModelParts;

			HashMap<std::string, FIPartialAnimation*> m_AnimationKeyFrames;

			std::string m_strName;

			Matrix m_matAbsoluteTransform;
			Matrix m_parentRelativeTransform;
			//Matrix m_matGeometricOffset;
			//Matrix m_matAnimationTransform;
		public:
			
			FIMesh(const std::string& strName)
				: m_strName(strName)
			{
				m_matAbsoluteTransform = Matrix::Identity;
				m_parentRelativeTransform = Matrix::Identity;
				//m_matAbsoluteTransform = Matrix::Identity;
			}
			~FIMesh()
			{
				for (size_t i=0;i<m_ModelParts.size();i++)
				{
					delete m_ModelParts[i];
				}
				for (HashMap<std::string, FIPartialAnimation*>::Enumerator e = m_AnimationKeyFrames.GetEnumerator();e.MoveNext();)
				{
					FIPartialAnimation* akf = *e.getCurrentValue();
					delete akf;
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
			void AddVertex(MaterialData* pMaterial, const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord0, const Vector2& vTexCoord1, const BoneWeight& boneWeights)
			{
				bool bNewMaterial = true;

				for( size_t i = 0; i < m_ModelParts.size(); ++i )
				{
					FIMeshPart* pModelPart = m_ModelParts[i];
					if( pMaterial == pModelPart->GetMaterial() )
					{
						pModelPart->AddVertex(vPosition, vNormal, vTexCoord0, vTexCoord1, boneWeights);
						bNewMaterial = false;
					}
				}

				if( bNewMaterial )
				{
					FIMeshPart* pModelPart = new FIMeshPart(this, pMaterial);
					pModelPart->AddVertex(vPosition, vNormal, vTexCoord0, vTexCoord1, boneWeights);
					m_ModelParts.push_back(pModelPart);
				}
			}

			/** Obsolete. Actually do nothing.
			*/
			void FinishAndOptimize()
			{
				//for( int i = 0; i < m_ModelParts.GetSize(); ++i )
				//{
				//	m_ModelParts.GetAt(i)->Optimize();
				//}
			}

			//void InitializeDeviceObjects(ID3D10Device* pd3dDevice);
			//void ReleaseDeviceObjects();

			void AddAnimationKeyFrames(FIPartialAnimation* pAnimationKeyFrames)
			{
				m_AnimationKeyFrames.Add(pAnimationKeyFrames->GetAnimationName(),
					pAnimationKeyFrames);
			}
			FIPartialAnimation* GetAnimationKeyFrames(const std::string& strAnimationName) const
			{
				return m_AnimationKeyFrames[strAnimationName];
			}

			//void UpdateAnimation(CBTTAnimationController* pAnimationController);
			//void Render(ID3D10Device* pd3dDevice, CBTTEffect* pBTTEffect, bool bEnableAnimation);

			void SetParentRelativeTransform(const Matrix& m) { m_parentRelativeTransform = m; }
			void SetAbsoluteTransform(const Matrix& matAbsoluteTransform)   { m_matAbsoluteTransform = matAbsoluteTransform; }
			//void SetGeometricOffset(const Matrix& matGeometricOffset)		{ m_matGeometricOffset = matGeometricOffset; }
			const Matrix& GetAbsoluteTransform() const { return m_matAbsoluteTransform; }
			const Matrix& GetParentRelativeTransform() const { return m_parentRelativeTransform; }
			//const Matrix& GetGeometricOffset() const	{ return m_matGeometricOffset; }
			//const Matrix& GetAnimationTransform() const { return m_matAnimationTransform; }
			const std::vector<FIMeshPart*>& getParts() const { return m_ModelParts; }

			const std::string& GetName() const { return m_strName; }
		};

		class FIAnimationKeyframe
		{
		public:
			float Time;
			Matrix Transform;

			FIAnimationKeyframe(const Matrix& tr, float time)
				: Transform(tr), Time(time)
			{

			}
		};

		/** Defines an period of animation for one bone or mesh part.
			If the animation is skeleton animation, the bone uses the keyframes when playing.
			Otherwise the keyframes are used as transform of a mesh part.
		*/
		class FIPartialAnimation
		{
		protected:
			std::string m_strAnimationName;
			std::vector<FIAnimationKeyframe> m_KeyFrames;
		public:
			FIPartialAnimation(const std::string& strAnimationName)
				: m_strAnimationName(strAnimationName)
			{

			}
			~FIPartialAnimation() { }

			void AddKeyFrame(const FIAnimationKeyframe& frame)
			{
				m_KeyFrames.push_back(frame);
			}

			const std::string& GetAnimationName() const { return m_strAnimationName; }
			uint getKeyFrameCount() const { return static_cast<uint>(m_KeyFrames.size()); }
			const Matrix& GetKeyFrameTransform(uint nKeyFrame) const { return m_KeyFrames[nKeyFrame].Transform; }
			float GetKeyFrameTime(uint frameIndex) const { return m_KeyFrames[frameIndex].Time; }
			//const CBTTAnimationQuaternionKeyFrame& GetKeyFrameQuaternion(int nKeyFrame);
		};
		class FISkeletonBone
		{
		private:
			friend class FISkeleton;
			std::string m_strName;

			Matrix m_matBindPoseTransform;
			Matrix m_matInvBindPoseTransform;
			Matrix m_matBoneReferenceTransform;
			Matrix m_matInvBoneReferenceTransform;

			int m_nParentBoneIndex;

			HashMap<std::string, FIPartialAnimation*> m_AnimationKeyFrames;
		public:
			FISkeletonBone(std::string strName, int nParentBoneIndex)
				: m_strName(strName), m_nParentBoneIndex(nParentBoneIndex)
			{
				m_matBindPoseTransform = Matrix::Identity;
				m_matInvBindPoseTransform = Matrix::Identity;
			}
			~FISkeletonBone()
			{
				for (HashMap<std::string, FIPartialAnimation*>::Enumerator e = m_AnimationKeyFrames.GetEnumerator();e.MoveNext();)
				{
					FIPartialAnimation* akf = *e.getCurrentValue();
					delete akf;
				}
				m_AnimationKeyFrames.Clear();
			}


			void AddAnimationKeyFrames(FIPartialAnimation* pAnimationKeyFrames)
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

			FIPartialAnimation* GetAnimationKeyFrames(const std::string strAnimationName) const
			{
				FIPartialAnimation* result;
				if (m_AnimationKeyFrames.TryGetValue(strAnimationName, result))
					return result;
				return 0;
			}

			const Matrix& GetBindPoseTransform() const { return m_matBindPoseTransform; }
			const Matrix& GetInvBindPoseTransform()	const { return m_matInvBindPoseTransform; }
			const Matrix& GetBoneReferenceTransform() const { return m_matBoneReferenceTransform; }
			const Matrix& GetInvBoneReferenceTransform() const { return m_matInvBoneReferenceTransform; }

			const std::string& GetName() const { return m_strName; }
			int GetParentBoneIndex() const { return m_nParentBoneIndex; }


		};
		class FISkeleton
		{
		private:
			FastList<FISkeletonBone*> m_SkeletonBones;

		public:
			FISkeleton()
			{

			}
			~FISkeleton()
			{
				for (int i=0;i<m_SkeletonBones.getCount();i++)
				{
					delete m_SkeletonBones[i];
				}
			}

			void AddSkeletonBone(FISkeletonBone* pSkeletonBone)
			{
				m_SkeletonBones.Add(pSkeletonBone);
			}
			FISkeletonBone* FindBone(const std::string& strBoneName)
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

			FISkeletonBone* GetSkeletonBone(int nIndex)
			{
				return m_SkeletonBones[nIndex];
			}
			//const FastMap<string, SkeletonBone*>& GetSkeletonBones() const { return m_SkeletonBones; }
			//Matrix* GetSkinTransforms() { return m_SkinTransforms; }
			int GetBoneCount() const { return m_SkeletonBones.getCount(); }

			void FlattenAnimation(AnimationData::ClipTable* clipTable)
			{
				ExistTable<std::string> seenAnimation;
				
				for (int i=0;i<m_SkeletonBones.getCount();i++)
				{
					const FISkeletonBone* bone = m_SkeletonBones[i];
					for (HashMap<std::string, FIPartialAnimation*>::Enumerator j=bone->m_AnimationKeyFrames.GetEnumerator();j.MoveNext();)
					{
						FIPartialAnimation* anim = *j.getCurrentValue();

						const std::string& name = anim->GetAnimationName();

						if (!seenAnimation.Exists(name))
						{
							seenAnimation.Add(name);
						}
					}
				}

				for (ExistTable<std::string>::Enumerator e = seenAnimation.GetEnumerator();e.MoveNext();)
				{
					const std::string& animName = *e.getCurrent();
					
					uint frameIndex = 0;
					bool finished = false;
					FastList<ModelKeyframe> frames;
					
					while (!finished)
					{
						finished = true;
						for (int i=0;i<m_SkeletonBones.getCount();i++)
						{
							const FISkeletonBone* bone = m_SkeletonBones[i];
							FIPartialAnimation* anim = bone->GetAnimationKeyFrames(animName);

							if (frameIndex < anim->getKeyFrameCount())
							{
								finished = false;
								const Matrix& trans = anim->GetKeyFrameTransform(frameIndex);
								float time = anim->GetKeyFrameTime(frameIndex);

								frames.Add(ModelKeyframe(i,time, trans));
							}
						}
						frameIndex++;
					}
					// find next frames
					for (int i=0;i<frames.getCount();i++)
					{
						frames[i].setNextFrameIndex(-1);
						for (int j=0;j<frames.getCount();j++)
						{
							int nextFrameIdx = (j + i + 1) % frames.getCount();
							if (frames[nextFrameIdx].getObjectIndex() == frames[i].getObjectIndex())
							{
								frames[i].setNextFrameIndex(nextFrameIdx);
								break;
							}
						}
					}
					if (frames.getCount())
					{
						ModelAnimationClip* clip = new ModelAnimationClip(frames[frames.getCount()-1].getTime(), frames);
						clipTable->Add(StringUtils::toWString(animName), clip);
					}					
				}
			}
			void FlattenBones(List<Bone>& bones)
			{
				for (int i=0;i<m_SkeletonBones.getCount();i++)
				{
					Bone bone(i);
					bone.Parent = m_SkeletonBones[i]->GetParentBoneIndex();
					bone.setBindPoseTransform(m_SkeletonBones[i]->GetBindPoseTransform());
					bone.setBoneReferenceTransform(m_SkeletonBones[i]->GetBoneReferenceTransform());
					bone.Name = StringUtils::toWString(m_SkeletonBones[i]->GetName());
					bones.Add(bone);
				}

				for (int i=0;i<bones.getCount();i++)
				{
					if (bones[i].Parent != -1)
					{
						bones[bones[i].Parent].Children.Add(i);
					}
				}
			}

			//void UpdateAnimation(CBTTAnimationController* pAnimationController);
		};
	private:
		String m_sourceFile;
		bool m_bakeTransform;

		KFbxSdkManager* m_pFBXSdkManager;
		KFbxScene* m_pFBXScene;

		/** One to one mapping to m_materials. 
			Used to find corresponding MaterialData from KFbxSurfaceMaterial or back.
		*/
		FastList<KFbxSurfaceMaterial*> m_FBXMaterials;
		FastList<MaterialData*> m_materials;
		

		HashMap<std::string, FIMesh*> m_meshes;
		FISkeleton* m_pSkeleton;



		bool InitializeFBXSdk();
		bool LoadScene(const String& pFilename);

		void ProcessScene(KFbxScene* pScene);
		void ProcessMaterials(KFbxScene* pScene);
		void ProcessAnimations(KFbxScene* pScene);
		/** Recursively process all the animation of a specified node and all its children. 
		*/
		void ProcessAnimation(KFbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop);
		void ProcessNode(KFbxNode* pNode, KFbxNodeAttribute::EAttributeType attributeType);
		void ProcessSkeleton(KFbxNode* pNode);
		void ProcessMesh(KFbxNode* pNode);

		void FixSkeletonTransform(KFbxNode* pSceneRoot, KFbxNode* pNode);

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

		void FlattenAnimation(AnimationData::ClipTable* clipTable)
		{
			const FIMesh** meshList = new const FIMesh*[m_meshes.getCount()];
			ExistTable<std::string> seenAnimation;
			int idxCounter = 0;
			for (HashMap<std::string, FIMesh*>::Enumerator i=m_meshes.GetEnumerator();i.MoveNext();)
			{
				const FIMesh* mesh = *i.getCurrentValue();
				for (HashMap<std::string, FIPartialAnimation*>::Enumerator j=mesh->m_AnimationKeyFrames.GetEnumerator();j.MoveNext();)
				{
					FIPartialAnimation* anim = *j.getCurrentValue();

					const std::string& name = anim->GetAnimationName();

					if (!seenAnimation.Exists(name))
					{
						seenAnimation.Add(name);
					}
				}
				meshList[idxCounter++] = mesh;
			}

			for (ExistTable<std::string>::Enumerator e = seenAnimation.GetEnumerator();e.MoveNext();)
			{
				const std::string& animName = *e.getCurrent();

				uint frameIndex = 0;
				bool finished = false;
				FastList<ModelKeyframe> frames;

				while (!finished)
				{
					finished = true;
					for (int i=0;i<m_meshes.getCount();i++)
					{
						const FIMesh* mesh = meshList[i];
						FIPartialAnimation* anim = mesh->GetAnimationKeyFrames(animName);

						if (frameIndex < anim->getKeyFrameCount())
						{
							finished = false;
							const Matrix& trans = anim->GetKeyFrameTransform(frameIndex);
							float time = anim->GetKeyFrameTime(frameIndex);
							
							frames.Add(ModelKeyframe(i,time, trans));
						}
					}
					frameIndex++;
				}

				// find next frames
				for (int i=0;i<frames.getCount();i++)
				{
					frames[i].setNextFrameIndex(-1);
					for (int j=0;j<frames.getCount();j++)
					{
						int nextFrameIdx = (j + i + 1) % frames.getCount();
						if (frames[nextFrameIdx].getObjectIndex() == frames[i].getObjectIndex())
						{
							frames[i].setNextFrameIndex(nextFrameIdx);
							break;
						}
					}
				}

				if (frames.getCount())
				{
					ModelAnimationClip* clip = new ModelAnimationClip(frames[frames.getCount()-1].getTime(), frames);
					clipTable->Add(StringUtils::toWString(animName), clip);
				}					
			}
			

			delete[] meshList;
		}
	public:
		FbxImporter()
			: m_pSkeleton(0), m_FBXMaterials(0), m_pFBXScene(0), m_bakeTransform(false)
		{

		}
		~FbxImporter();
		bool Initialize(const String& pFilename);
		
		void SetBakeTransform(bool v);

		static void Import(const MeshBuildConfig& config);
	};
}
#endif