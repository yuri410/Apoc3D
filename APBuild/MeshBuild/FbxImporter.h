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

		KFbxSurfaceMaterial* GetMaterialAppliedToPoly(KFbxMesh* mesh, int layerCount, int polyCount);
		void CacheBindPose(KFbxScene* scene);
	public:
		FbxImporter();

		void Import(const string& fileName);

	};
}
#endif