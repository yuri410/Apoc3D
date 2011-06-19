#include "FbxImporter.h"

#include "CompileLog.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(m_pFBXSdkManager->GetIOSettings()))
#endif

namespace APBuild
{
	string toString(const String& str)
	{
		char* buffer = new char[str.length()];
		wcstombs(buffer, str.c_str(), str.length());
		string result = buffer;
		delete[] buffer;
		return result;
	}
	String toWString(const string& str)
	{
		wchar_t* buffer = new wchar_t[str.length()];
		mbstowcs(buffer, str.c_str(), str.length());
		String result = buffer;
		delete[] buffer;
		return result;
	}


	Matrix ConvertMatrix(const KFbxMatrix& m)
	{
		const double* mptr = m.operator const double *();
		float buffer[16];
		for (int i=0;i<16;i++)
		{
			buffer[i] = static_cast<float>(mptr[i]);
		}
		return Matrix(buffer);
	}

	bool FbxImporter::InitializeFBXSdk()
	{
		m_pFBXSdkManager = KFbxSdkManager::Create();
		if (!m_pFBXSdkManager) return false;


		// create an IOSettings object
		KFbxIOSettings * ios = KFbxIOSettings::Create(m_pFBXSdkManager, IOSROOT );
		m_pFBXSdkManager->SetIOSettings(ios);

		// Load plugins from the executable directory
		KString lPath = KFbxGetApplicationDirectory();
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		KString lExtension = "dll";
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_MAC
		KString lExtension = "dylib";
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_LINUX
		KString lExtension = "so";
#elif
#pragma error "FBX SDK does not not support the platform."
#endif
		m_pFBXSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

		m_pFBXScene = KFbxScene::Create(m_pFBXSdkManager,"");
		if( !m_pFBXScene ) return false;

		return true;
	}

	bool FbxImporter::Initialize(const String& pFilename)
	{
		if( !InitializeFBXSdk() )	return false;
		if( !LoadScene(pFilename) )	return false;

		if( m_pFBXScene )		m_pFBXScene->Destroy();
		if(m_pFBXSdkManager)	m_pFBXSdkManager->Destroy();

		m_pFBXScene = NULL;
		m_pFBXSdkManager = NULL;

		return true;
	}

	bool FbxImporter::LoadScene(const String& pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		//int lFileFormat = -1;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[256];

		string mbFilename = toString(pFilename);

		// Get the file version number generate by the FBX SDK.
		KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		KFbxImporter* lImporter = KFbxImporter::Create(m_pFBXSdkManager,"");


		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(
			mbFilename.c_str(), -1, m_pFBXSdkManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( !lImportStatus )
		{
			CompileLog::getSingleton().WriteError(L"Unable to initialize FBX", pFilename);
			CompileLog::getSingleton().WriteError(toWString(lImporter->GetLastErrorString()), pFilename);

			if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
			{
				CompileLog::getSingleton().WriteError(
					L"File version is probably newer than the version supported.", pFilename);
			}

			return false;
		}


		if (lImporter->IsFBX())
		{
			
			//printf("Animation Stack Information\n");

			//lAnimStackCount = lImporter->GetAnimStackCount();

			//printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
			//printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			//printf("\n");

			//for(i = 0; i < lAnimStackCount; i++)
			//{
			//	KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			//	printf("    Animation Stack %d\n", i);
			//	printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			//	printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			//	// Change the value of the import name if the animation stack should be imported 
			//	// under a different name.
			//	printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			//	// Set the value of the import state to false if the animation stack should be not
			//	// be imported. 
			//	printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			//	printf("\n");
			//}

			//// Set the import states. By default, the import states are always set to 
			//// true. The code below shows how to change these states.
			//IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
			//IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
			//IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
			//IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
			//IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
			//IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
			//IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(m_pFBXScene);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			printf("Please enter password: ");

			lPassword[0] = '\0';

			scanf("%s", lPassword);
			KString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(m_pFBXScene);

			if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
			{
				printf("\nPassword is wrong, import aborted.\n");
			}
		}

		ProcessScene(m_pFBXScene);

		// Destroy the importer.
		lImporter->Destroy();



		//KFbxImporter* pFBXImporter = KFbxImporter::Create(m_pFBXSdkManager,"");

		//
		//if( !pFBXImporter->Initialize(pFilename) )	{ pFBXImporter->Destroy(); return false; }	// Initialize importer
		//if( !pFBXImporter->Import(m_pFBXScene) )	{ pFBXImporter->Destroy(); return false; }	// Import the scene

		//m_strFileName = pFBXImporter->GetFileName().Buffer();

		//pFBXImporter->Destroy();

		//ProcessScene(m_pFBXScene);

		return true;
	}

	void FbxImporter::ProcessScene(KFbxScene* pScene)
	{
		
	}

	void FbxImporter::ProcessMaterials(KFbxScene* pScene)
	{

	}
	void FbxImporter::ProcessNode(KFbxNode* pNode, KFbxNodeAttribute::EAttributeType attributeType)
	{
		if( !pNode )
			return;

		KFbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if( pNodeAttribute->GetAttributeType() == attributeType )
			{
				switch(pNodeAttribute->GetAttributeType())
				{
				case KFbxNodeAttribute::eSKELETON:
					ProcessSkeleton(pNode);
					break;
				case KFbxNodeAttribute::eMESH:
					ProcessMesh(pNode);
					break;

				case KFbxNodeAttribute::eMARKER:
				case KFbxNodeAttribute::eNURB:
				case KFbxNodeAttribute::ePATCH:
				case KFbxNodeAttribute::eCAMERA:
				case KFbxNodeAttribute::eLIGHT:
				case KFbxNodeAttribute::eNULL:
					break;
				};
			}
		}

		for( int i = 0; i < pNode->GetChildCount(); ++i )
		{
			ProcessNode(pNode->GetChild(i), attributeType);
		}
	}
	void FbxImporter::ProcessSkeleton(KFbxNode* pNode)
	{

	}
	void FbxImporter::ProcessMesh(KFbxNode* pNode)
	{
		bool isAnimated = pNode->IsAnimated();

		KFbxGeometryConverter GeometryConverter(m_pFBXSdkManager);
		if( !GeometryConverter.TriangulateInPlace( pNode ) )
			return;

		KFbxMesh* pFBXMesh = pNode->GetMesh();
		if( !pFBXMesh )
			return;

		int nVertexCount = pFBXMesh->GetControlPointsCount();
		if( nVertexCount <= 0 )
			return;

		std::vector<CBTTBoneWeights> boneWeights(nVertexCount, CBTTBoneWeights());
		ProcessBoneWeights(pFBXMesh, boneWeights);

		//CBTTModel* pModel = new CBTTModel(pNode->GetName());
		//KFbxVector4* aControlPoints = pFBXMesh->GetControlPoints();
		//for( int pi = 0; pi < pFBXMesh->GetPolygonCount(); ++pi )
		//{
		//	CBTTMaterial* pMaterial = NULL;

		//	for( int pvi = 0; pvi < 3; ++pvi )
		//	{
		//		int nVertexIndex = pFBXMesh->GetPolygonVertex(pi, pvi);

		//		if( nVertexIndex < 0 || nVertexIndex >= nVertexCount )
		//			continue;

		//		if( pMaterial == NULL )
		//			pMaterial = GetMaterialLinkedWithPolygon(pFBXMesh, 0, pi, 0, nVertexIndex);

		//		KFbxVector4 fbxPosition = aControlPoints[nVertexIndex];
		//		KFbxVector4 fbxNormal;
		//		pFBXMesh->GetPolygonVertexNormal(pi, pvi, fbxNormal);
		//		fbxNormal.Normalize();

		//		pModel->AddVertex(pMaterial, KFbxVector4ToD3DXVECTOR3(fbxPosition),
		//			KFbxVector4ToD3DXVECTOR3(fbxNormal),
		//			GetTexCoord(pFBXMesh, 0, pi, pvi, nVertexIndex),
		//			boneWeights[nVertexIndex]);
		//	}
		//}

		//pModel->SetAbsoluteTransform(GetAbsoluteTransformFromCurrentTake(pNode, KTime(0)));
		//pModel->SetGeometricOffset(GetGeometricOffset(pNode));
		//pModel->Optimize();
		//m_Models.Add(pModel->GetName(), pModel);
	}

	void FbxImporter::ProcessBoneWeights(KFbxSkin* pFBXSkin, std::vector<CBTTBoneWeights>& meshBoneWeights)
	{
		KFbxCluster::ELinkMode linkMode = KFbxCluster::eNORMALIZE; //Default link mode

		switch(linkMode)
		{
		case KFbxCluster::eNORMALIZE:	//Normalize so weight sum is 1.0.
			for( int i = 0; i < (int)skinBoneWeights.size(); ++i )
			{
				skinBoneWeights[i].Normalize();
			}
			break;

		case KFbxCluster::eADDITIVE:	//Not supported yet. Do nothing
			break;

		case KFbxCluster::eTOTAL1:		//The weight sum should already be 1.0. Do nothing.
			break;
		}

		for( int i = 0; i < (int)meshBoneWeights.size(); ++i )
		{
			meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
		}	
	}
	void FbxImporter::ProcessBoneWeights(KFbxMesh* pFBXMesh, std::vector<CBTTBoneWeights>& meshBoneWeights)
	{

	}

	int FbxImporter::GetMappingIndex(KFbxLayerElement::EMappingMode MappingMode, 
		int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		switch(MappingMode)
		{
		case KFbxLayerElement::eALL_SAME:
			return 0;
			break;

		case KFbxLayerElement::eBY_CONTROL_POINT:
			return nVertexIndex;
			break;

		case KFbxLayerElement::eBY_POLYGON_VERTEX:
			return nPolygonIndex*3 + nPolygonVertexIndex;
			break;

		case KFbxLayerElement::eBY_POLYGON:
			return nPolygonIndex;
			break;

		case KFbxLayerElement::eNONE:
		case KFbxLayerElement::eBY_EDGE:
			break;
		}
		return -1;
	}

	MaterialData* FbxImporter::GetMaterial(KFbxSurfaceMaterial* pFBXMaterial)
	{
		int index = m_FBXMaterials.IndexOf(pFBXMaterial);
		if (index != -1)
			return m_materials[index];
		return 0;
	}
	Matrix FbxImporter::GetAbsoluteTransformFromCurrentTake(KFbxNode* pNode, KTime time)
	{
		if (!pNode)
		{
			return Matrix::Identity;
		}
		return ConvertMatrix(pNode->EvaluateGlobalTransform(time));
	}
	Matrix FbxImporter::GetGeometricOffset(KFbxNode* pNode)
	{
		if( !pNode )
		{
			return Matrix::Identity;
		}

		KFbxVector4 T = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
		KFbxVector4 R = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
		KFbxVector4 S = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

		KFbxXMatrix matFBXGeometryOffset;
		matFBXGeometryOffset.SetIdentity();
		matFBXGeometryOffset.SetT(T);
		matFBXGeometryOffset.SetR(R);
		matFBXGeometryOffset.SetS(S);

		return ConvertMatrix(matFBXGeometryOffset);
	}

	bool FbxImporter::IsSkeletonRoot(KFbxNode* pNode)
	{
		bool bNodeIsSkeleton = false;
		bool bParentNodeIsSkeleton = false;

		KFbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if( pNodeAttribute )	
			bNodeIsSkeleton = (pNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON);

		KFbxNode* pParentNode = pNode->GetParent();
		if( pParentNode )
		{
			KFbxNodeAttribute* pParentNodeAttribute = pParentNode->GetNodeAttribute();
			if( pParentNodeAttribute )
				bParentNodeIsSkeleton = (pParentNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON);
		}

		return (!bParentNodeIsSkeleton && bNodeIsSkeleton);
	}

	Vector3 FbxImporter::GetMaterialColor(KFbxPropertyDouble3 FBXColorProperty, KFbxPropertyDouble1 FBXFactorProperty)
	{
		Vector3 Color;

		if( FBXColorProperty.IsValid() )
		{
			fbxDouble3 FBXColor = FBXColorProperty.Get();
			Color = Vector3Utils::LDVector( FBXColor[0], FBXColor[1],FBXColor[2] );

			if( FBXFactorProperty.IsValid() )
			{
				double FBXFactor = FBXFactorProperty.Get();
				Vector3Utils::Multiply(Color, static_cast<float>(FBXFactor));
			}
		}
		return Color;
	}
}