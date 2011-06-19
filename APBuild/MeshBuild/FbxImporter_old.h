#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H

#include "APBCommon.h"

#include <fbxsdk.h>

#include "Collections/FastList.h"
#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "IOLib/MaterialData.h"
#include "Graphics/Animation/AnimationTypes.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::Animation;

namespace APBuild
{
	class AnimationContent;
	typedef FastMap<string, AnimationContent*> AnimationContentDictionary;

	class AnimationKeyframe
	{
	public:
		float Time;
		Matrix Transform;

	};
	class AnimationTake
	{
	public:
		FastList<AnimationKeyframe> Keyframes;
	};
	class AnimationContent
	{
	public:
		FastMap<string, AnimationTake*> Takes;
		float Duration;
		string Name;

		AnimationContent(const string& name)
			: Name(name)
		{

		}
	};
	class NodeContent
	{
	public:
		AnimationContentDictionary Animations;

		string Name;
		FastList<NodeContent*> Children;
		NodeContent* Parent;
		Matrix Transform;
		
		KFbxNode* FBXNode;
		bool PartOfMainSkeleton;

		NodeContent()
			: Parent(0)
		{
		}

		Matrix GetAbsoluteTransfrom() const
		{
			if (!Parent)
			{
				return Transform;
			}
			Matrix result;
			Matrix::Multiply(result, Transform, Parent->GetAbsoluteTransfrom());
			return result;
		}
	};
	class BoneContent
	{
	public:
		string Name;
	};

	class CalculateNodeTransformResult
	{
	public:
		Matrix AbsoluteTransform;
		Matrix LocalTransform;
	};
	class AnimationConverter
	{
	private:
		//struct FbxNodeInfo
		//{
		//	KFbxNode* fbxNode;
		//	bool partOfMainSkeleton;
		//};

		//FastMap<NodeContent*, FbxNodeInfo> m_NodeContentToFbxNodeInfo;

		
		KFbxNode* m_FBXSkeletonRoot;

		KFbxNode* FindRootMostBone(KFbxNode* sceneRoot);
		void AddAnimationToNodesRecursive(NodeContent* node, const string& takeName);

		void HierarchyFixup(NodeContent* sceneRoot);
		void SetCurrentTakeRecursive(KFbxNode* node, const string& takeName);
		void WarnAboutAnimatedNodes(NodeContent* node);

	public:
		NodeContent* m_SkeletonRoot;
		NodeContent* m_SkeletonOldParent;

		void AddAnimationInformationToScene(KFbxScene* fbxScene, NodeContent* sceneRoot);

		bool IsSkeletonRoot(KFbxNode* node)
		{
			if (!node)
				return false;
			return m_FBXSkeletonRoot == node;
		}
		void FindSkeletonRoot(KFbxNode* sceneRoot)
		{
			m_FBXSkeletonRoot = FindRootMostBone(sceneRoot);
		}
		void HierarchyFixup(NodeContent* sceneRoot);
		void NodeWasCreated(KFbxNode* fbxNode, NodeContent* node, NodeContent* potentialParent, bool partOfMainSkeleton);


	};
	class MeshConverter
	{
	public:
		NodeContent* FillNodeWithInfoFromMesh(KFbxNode* fbxNode, const string& name);

	};
	class UVSetTracker
	{

	};

	class FbxImporter
	{
	private:
		AnimationConverter m_animConverter;
		FastMap<string, Matrix> m_bindPoseCache;
		MeshConverter m_meshConverter;

		KFbxIOPluginRegistry* m_pFBXPluginRegistry;
		KFbxSdkManager* m_pFBXSdkManager;
		
		UVSetTracker* m_UVSetTracker;


		void CacheBindPose(KFbxScene* scene);
		CalculateNodeTransformResult CalculateNodeTransformBindPose(const Matrix& parentAbsTransform, KFbxNode* node);

		NodeContent* ProcessNode(const Matrix& parentAbsTrans, KFbxNode* fbxNode, 
			bool& partofMainSkeleton, bool& warnIfBoneButNotChild);
		NodeContent* ProcessNode(const Matrix& parentAbsTrans, KFbxNode* fbxNode, 
			bool partofMainSkeleton, bool warnIfBoneButNotChild);
		KFbxSurfaceMaterial* GetMaterialAppliedToPoly(KFbxMesh* mesh, int layerCount, int polyCount);
	
	public:
		FbxImporter();

		void Import(const string& fileName);

	};
}
#endif