#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H

#include "APBCommon.h"

#include <fbxsdk.h>

#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "IOLib/MaterialData.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;


namespace APBuild
{
	class FbxImporter
	{
	private:
		KFbxGeometryConverter* m_pFBXGeometryConverter;
		KFbxIOPluginRegistry* m_pFBXPluginRegistry;
		KFbxSdkManager* m_pFBXSdkManager;
		FastMap<string, Matrix> m_bindPoseCache;
		KFbxNode* m_skeletonRoot;

		bool IsSkeletonRoot(KFbxNode* node);

		KFbxNode* FindRootMostBone(KFbxNode* sceneRoot);
		void BuildMesh(KFbxNode* node);

		void ProcessNode(const Matrix& parentAbsTrans, KFbxNode* fbxNode, 
			bool partofMainSkeleton, bool warnIfBoneButNotChild);
		KFbxSurfaceMaterial* GetMaterialAppliedToPoly(KFbxMesh* mesh, int layerCount, int polyCount);
		void CacheBindPose(KFbxScene* scene);
	public:
		FbxImporter();

		void Import(const string& fileName);

	};
}
#endif