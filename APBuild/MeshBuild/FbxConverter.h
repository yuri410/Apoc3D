#pragma once

#include "../APBCommon.h"

#ifndef DISABLE_FBX

#include <fbxsdk.h>

#define MAXBONES_PER_VERTEX 4

#include <vector>

namespace APBuild
{
	class FbxConverter
	{
	public:
		struct BoneWeight
		{
			List<std::pair<int, float>> BoneWeights;

			void NormalizeInPlace()
			{
				if (BoneWeights.getCount() > MAXBONES_PER_VERTEX)
					BoneWeights.RemoveEnd(MAXBONES_PER_VERTEX);

				float total = 0.0f;
				for (auto& e : BoneWeights)
				{
					total += e.second;
				}

				float fScale = 1.0f/total;
				for (auto& e : BoneWeights)
				{
					e.second *= fScale;
				}

			}

			void AddBoneWeight(const BoneWeight& boneWeights)
			{
				for( int32 i = 0; i < boneWeights.BoneWeights.getCount(); ++i)	
				{
					const std::pair<int, float> e = boneWeights.BoneWeights[i];
					AddBoneWeight(e.first, e.second);
				}
			}
			void AddBoneWeight(int boneIndex, float boneWeight)
			{
				float passed = false;
				for (int32 i = 0; i < BoneWeights.getCount(); i++)
				{
					const std::pair<int, float> e = BoneWeights[i];

					if (boneWeight > e.second)
					{
						BoneWeights.Insert(i, std::make_pair(boneIndex, boneWeight));
						passed = true;
						break;
					}
				}
				if (!passed)
					BoneWeights.Add(std::make_pair(boneIndex, boneWeight));
			}

			Vector4 GetBlendIndex() const
			{
				float v[4] = {0};
				for (int32 i=0;i<BoneWeights.getCount();i++)
				{
					v[i] = static_cast<float>( BoneWeights[i].first);
				}
				return Vector4(v);
			}
			Vector4 GetBlendWeight() const
			{
				float v[4] = {0};
				for (int32 i=0;i<BoneWeights.getCount();i++)
				{
					v[i] = BoneWeights[i].second;
				}
				return Vector4(v);
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
			List<Vector3> m_Positions;
			List<Vector3> m_Normals;
			List<Vector2> m_TexCoord0;
			List<Vector2> m_TexCoord1;
			List<uint32> m_VertexIndices;
			List<BoneWeight> m_BoneWeights;
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
				m_Positions.Add(vPosition);
				m_Normals.Add(vNormal);
				m_TexCoord0.Add(vTexCoord);
				m_BoneWeights.Add(boneWeights);

				m_VertexIndices.Add(m_VertexIndices.getCount());
				if (boneWeights.BoneWeights.getCount())
					m_bSkinnedModel = true;
			}
			void AddVertex(const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord0, const Vector2& vTexCoord1,
				const BoneWeight& boneWeights)
			{
				m_Positions.Add(vPosition);
				m_Normals.Add(vNormal);
				m_TexCoord0.Add(vTexCoord0);
				m_TexCoord1.Add(vTexCoord1);
				m_BoneWeights.Add(boneWeights);

				m_VertexIndices.Add(m_VertexIndices.getCount());
				if (boneWeights.BoneWeights.getCount())
					m_bSkinnedModel = true;
			}
			void FinishAndOptimize()
			{

			}

			bool IsSkinnedModel() const { return m_bSkinnedModel; }
			inline MaterialData* GetMaterial() const { return m_pMaterial; }
			const List<Vector3>& getPosition() const { return m_Positions; }
			const List<Vector3>& getNormal() const { return m_Normals; }
			const List<Vector2>& getTexCoord0() const { return m_TexCoord0; }
			const List<Vector2>& getTexCoord1() const { return m_TexCoord1; }
			const List<uint32>& getVertexIndices() const { return m_VertexIndices; }
			const List<BoneWeight>& getBoneWeights() const { return m_BoneWeights; }
			//void InitializeDeviceObjects(ID3D10Device* pd3dDevice);
			//void ReleaseDeviceObjects();

		};

		/**  */
		class FIMesh
		{
		private:
			friend class FbxConverter;

			List<FIMeshPart*> m_ModelParts;

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
				m_ModelParts.DeleteAndClear();

				m_AnimationKeyFrames.DeleteValuesAndClear();
			}

			void AddVertex(MaterialData* pMaterial, const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord, const BoneWeight& boneWeights)
			{
				bool bNewMaterial = true;

				for(FIMeshPart* pModelPart : m_ModelParts )
				{
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
					m_ModelParts.Add(pModelPart);
				}
			}
			void AddVertex(MaterialData* pMaterial, const Vector3& vPosition, const Vector3& vNormal, const Vector2& vTexCoord0, const Vector2& vTexCoord1, const BoneWeight& boneWeights)
			{
				bool bNewMaterial = true;

				for (FIMeshPart* pModelPart : m_ModelParts)
				{
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
					m_ModelParts.Add(pModelPart);
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
			const List<FIMeshPart*>& getParts() const { return m_ModelParts; }

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
				m_AnimationKeyFrames.DeleteValuesAndClear();
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
			List<FISkeletonBone*> m_SkeletonBones;

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
			//const HashMap<string, SkeletonBone*>& GetSkeletonBones() const { return m_SkeletonBones; }
			//Matrix* GetSkinTransforms() { return m_SkinTransforms; }
			int GetBoneCount() const { return m_SkeletonBones.getCount(); }

			void FlattenAnimation(AnimationData::ClipTable* clipTable)
			{
				HashSet<std::string> seenAnimation;
				
				for (const FISkeletonBone* bone : m_SkeletonBones)
				{
					for (FIPartialAnimation* anim : bone->m_AnimationKeyFrames.getValueAccessor()) 
					{
						const std::string& name = anim->GetAnimationName();

						if (!seenAnimation.Contains(name))
						{
							seenAnimation.Add(name);
						}
					}
				}

				for (const std::string& animName : seenAnimation)
				{
					uint frameIndex = 0;
					bool finished = false;
					List<ModelKeyframe> frames;
					
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
						clipTable->Add(StringUtils::toPlatformWideString(animName), clip);
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
					bone.Name = StringUtils::toPlatformWideString(m_SkeletonBones[i]->GetName());
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

		FbxManager* m_pFBXSdkManager;
		FbxScene* m_pFBXScene;

		/** One to one mapping to m_materials. 
			Used to find corresponding MaterialData from KFbxSurfaceMaterial or back.
		*/
		List<FbxSurfaceMaterial*> m_FBXMaterials;
		List<MaterialData*> m_materials;
		

		HashMap<std::string, FIMesh*> m_meshes;
		FISkeleton* m_pSkeleton;



		bool InitializeFBXSdk();
		bool LoadScene(const String& pFilename);

		void ProcessScene(FbxScene* pScene);
		void ProcessMaterials(FbxScene* pScene);
		void ProcessAnimations(FbxScene* pScene);

		/** Recursively process all the animation of a specified node and all its children. */
		void ProcessAnimation(FbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop);
		void ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType);
		void ProcessSkeleton(FbxNode* pNode);
		void ProcessMesh(FbxNode* pNode);

		void FixSkeletonTransform(FbxNode* pSceneRoot, FbxNode* pNode);

		void ProcessBoneWeights(FbxMesh* pFBXMesh, std::vector<BoneWeight>& meshBoneWeights);
		void ProcessBoneWeights(FbxSkin* pFBXSkin, std::vector<BoneWeight>& meshBoneWeights);


		MaterialData* GetMaterialLinkedWithPolygon(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		Vector2 GetTexCoord(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		int GetMappingIndex(FbxLayerElement::EMappingMode MappingMode, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
		MaterialData* GetMaterial(FbxSurfaceMaterial* pFBXMaterial);
		Matrix GetAbsoluteTransformFromCurrentTake(FbxNode* pNode, FbxTime time);
		Matrix GetGeometricOffset(FbxNode* pNode);
		Vector3 GetMaterialColor(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<double> FBXFactorProperty);


		bool IsSkeletonRoot(FbxNode* pNode);

		void FlattenAnimation(AnimationData::ClipTable* clipTable);
	public:
		FbxConverter();
		~FbxConverter();
		bool Initialize(const String& pFilename);
		
		void SetBakeTransform(bool v);

		static void Import(const ProjectResModel& config);
	};
}

#else

namespace APBuild
{
	class FbxConverter
	{
	public:
		static void Import(const ProjectResModel& config);
	};
}

#endif
