
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
	class FbxImporter
	{
	private:
		KFbxSdkManager* m_pFBXSdkManager;
		KFbxScene* m_pFBXScene;

		FastList<KFbxSurfaceMaterial*> m_FBXMaterials;
		FastList<MaterialData*> m_materials;
		FastList<MeshData*> m_meshes;

		bool InitializeFBXSdk();
		bool LoadScene(const String& pFilename);

		void ProcessScene(KFbxScene* pScene);
		void ProcessMaterials(KFbxScene* pScene);
		void ProcessAnimations(KFbxScene* pScene);
		void ProcessAnimation(KFbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop);
		void ProcessNode(KFbxNode* pNode, KFbxNodeAttribute::EAttributeType attributeType);
		void ProcessSkeleton(KFbxNode* pNode);
		void ProcessMesh(KFbxNode* pNode);

		void ProcessBoneWeights(KFbxMesh* pFBXMesh, std::vector<CBTTBoneWeights>& meshBoneWeights);
		void ProcessBoneWeights(KFbxSkin* pFBXSkin, std::vector<CBTTBoneWeights>& meshBoneWeights);


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