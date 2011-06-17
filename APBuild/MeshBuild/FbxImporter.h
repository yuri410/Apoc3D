#ifndef FBXIMPORTER_H
#define FBXIMPORTER_H

#include "APBCommon.h"

#include <fbxsdk.h>

#include "Collections/FastMap.h"
#include "Math/Matrix.h"


using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::Config;



namespace APBuild
{
	class FbxImporter
	{
	private:
		KFbxGeometryConverter* m_pFBXGeometryConverter;
		KFbxIOPluginRegistry* m_pFBXPluginRegistry;
		KFbxSdkManager* m_pFBXSdkManager;
		FastMap<string, Matrix> m_bindPoseCache;
		void CacheBindPose(KFbxScene* scene);
	public:
		FbxImporter();

		void Import(const string& fileName);

	};
}
#endif