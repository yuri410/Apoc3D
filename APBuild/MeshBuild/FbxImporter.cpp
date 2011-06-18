#include "FbxImporter.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif

void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene)
{
	// The first thing to do is to create the FBX SDK manager which is the 
	// object allocator for almost all the classes in the SDK.
	pSdkManager = KFbxSdkManager::Create();

	if (!pSdkManager)
	{
		printf("Unable to create the FBX SDK manager\n");
		exit(0);
	}

	// create an IOSettings object
	KFbxIOSettings * ios = KFbxIOSettings::Create(pSdkManager, IOSROOT );
	pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	KString lExtension = "so";
#endif
	pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// Create the entity that will hold the scene.
	pScene = KFbxScene::Create(pSdkManager,"");
}

void DestroySdkObjects(KFbxSdkManager* pSdkManager)
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}

bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
	int lMajor, lMinor, lRevision;
	bool lStatus = true;

	// Create an exporter.
	KFbxExporter* lExporter = KFbxExporter::Create(pSdkManager, "");

	if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
	{
		// Write in fall back format in less no ASCII format found
		pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

		//Try to export in ASCII if possible
		int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

		for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
		{
			if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
			{
				KString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
				char *lASCII = "ascii";
				if (lDesc.Find(lASCII)>=0)
				{
					pFileFormat = lFormatIndex;
					break;
				}
			}
		}
	}

	// Set the export states. By default, the export states are always set to 
	// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
	// shows how to change these states.

	IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
	IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
	IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
	IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
	IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
	IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
	IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	// Initialize the exporter by providing a filename.
	if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false)
	{
		printf("Call to KFbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
		return false;
	}

	KFbxSdkManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
	printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

	// Export the scene.
	lStatus = lExporter->Export(pScene); 

	// Destroy the exporter.
	lExporter->Destroy();
	return lStatus;
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		printf("Call to KFbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		printf("\n");

		for(i = 0; i < lAnimStackCount; i++)
		{
			KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			printf("    Animation Stack %d\n", i);
			printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
	{
		printf("Please enter password: ");

		lPassword[0] = '\0';

		scanf("%s", lPassword);
		KString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}


string WStr2Str(const wstring& str)
{
	char* buffer = new char[str.length()];
	wcstombs(buffer, str.c_str(), str.length());
	string result = buffer;
	delete[] buffer;
	return result;
}

void ExportContent(KFbxScene* pScene)
{
	KFbxNode* lNode = pScene->GetRootNode();

	if(lNode)
	{
		for(int i = 0; i < lNode->GetChildCount(); i++)
		{
			DisplayContent(lNode->GetChild(i));
		}
	}
}
void ExportContent(KFbxNode* pNode)
{

}

namespace APBuild
{
	bool IsANullNode(KFbxNode* node)
	{
		KFbxNodeAttribute* attr = node->GetNodeAttribute();
		if (attr)
		{
			if (attr->GetAttributeType()!=KFbxNodeAttribute::eNULL)
			{
				return false;
			}
		}
		return true;
	}
	bool IsAnInterestingNode(KFbxNode* node)
	{
		KFbxNodeAttribute* attr = node->GetNodeAttribute();
		if (!attr)
		{
			return true;
		}
		return attr->GetAttributeType() == KFbxNodeAttribute::eNULL
			|| attr->GetAttributeType() == KFbxNodeAttribute::eSKELETON 
			|| attr->GetAttributeType() == KFbxNodeAttribute::eMESH;
	}
	bool NodesAbsoluteTransformIsIdentity(KFbxNode* node)
	{
		const KFbxMatrix matrix = node->EvaluateGlobalTransform();
		return matrix[0][0] == 1.0 && matrix[1][1] == 1.0 && matrix[2][2] == 1.0 && matrix[3][3] == 1.0 &&
			matrix[0][1] == 0.0 && matrix[0][2] == 0.0 && matrix[0][3] == 0.0 &&
			matrix[1][0] == 0.0 && matrix[1][2] == 0.0 && matrix[1][3] == 0.0 &&
			matrix[2][0] == 0.0 && matrix[2][1] == 0.0 && matrix[2][3] == 0.0 &&
			matrix[3][0] == 0.0 && matrix[3][1] == 0.0 && matrix[3][2] == 0.0;
	}
	string GetNameForFbxObject(KFbxObject* obj)
	{
		KString name = obj->GetNameWithNameSpacePrefix();
		return name.operator const char *();
	}
	bool FindIndexUsingMappingMode(KFbxLayerElementMaterial* layer, 
		KFbxMesh* mesh, int polyNum, int vertNumInPolygon, int indexForThatVert, int& result)
	{
		KFbxLayerElement::EMappingMode mappingMode = layer->GetMappingMode();
		if (mappingMode == KFbxLayerElement::eALL_SAME)
		{
			result = 0;
			return true;
		}
		if (mappingMode == KFbxLayerElement::eBY_POLYGON)
		{
			result = polyNum;
			return true;
		}
		if (mappingMode == KFbxLayerElement::eBY_CONTROL_POINT)
		{
			result = indexForThatVert;
			return true;
		}
		if (mappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			result = polyNum * 3 + vertNumInPolygon;
			return true;
		}
		
		return false;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	bool FbxImporter::IsSkeletonRoot(KFbxNode* node)
	{
		if (!node)
		{
			return false;
		}
		return m_skeletonRoot == node;
	}

	KFbxSurfaceMaterial* FbxImporter::GetMaterialAppliedToPoly(KFbxMesh* mesh, int layerNum, int polyNum)
	{
		KFbxSurfaceMaterial* surfaceMtrl;

		KFbxNode* node = mesh->GetNode();

		if (!node)
		{
			return 0;
		}
		KFbxLayerElementMaterial* layeredMtrl = mesh->GetLayer(layerNum)->GetMaterials();
		if (!layeredMtrl)
		{
			return 0;
		}
		try
		{
			int indexForThatVert = mesh->GetPolygonVertex(polyNum, 0);
			int index;
			bool re =
				FindIndexUsingMappingMode(layeredMtrl, 
				 mesh, polyNum, 0, indexForThatVert, index);
			if (re && index>=0)
			{
				KFbxLayerElement::EReferenceMode mode = layeredMtrl->GetReferenceMode();
				if (mode == KFbxLayerElement::eDIRECT)
				{
					if (index >= node->GetMaterialCount())
					{
						return 0;
					}
					return node->GetMaterial(index);
				}
				else if (mode == KFbxLayerElement::eINDEX_TO_DIRECT)
				{
					
					KFbxLayerElementArrayTemplate<int>* indexArr = layeredMtrl->GetIndexArray();
					if (index < indexArr->GetCount())
					{
						int num = indexArr->GetAt(index);
						if (num >=0 && num < node->GetMaterialCount())
						{
							return node->GetMaterial(num);
						}
					}
					return 0;
				}
			}
		}
		catch (CException* e)
		{
		}
		return 0;
	}
	void FbxImporter::CacheBindPose(KFbxScene* scene)
	{
		for (int i=0;i<scene->GetPoseCount();i++)
		{
			KFbxPose* pose = scene->GetPose(i);

			for (int j=0;j < pose->GetCount();j++)
			{
				KName name = pose->GetNodeName(j);
				
				string name2 = name.GetCurrentName();
				
				KFbxMatrix matrix = pose->GetMatrix(j);
				float elems[16];
				for (int k=0;k<16;k++)
				{
					elems[k] = static_cast<float>(matrix[k]);
				}
				Matrix matrix2(elems);

				m_bindPoseCache.Add(name, matrix2);
			}
		}
	}

	KFbxNode* FbxImporter::FindRootMostBone(KFbxNode* sceneRoot)
	{
		// BFS pass until the first skeleton node is found
		if (sceneRoot)
		{
			list<KFbxNode*> nodeList;
			
			nodeList.push_back(sceneRoot);

			do 
			{
				KFbxNode* nodePtr = *nodeList.begin();
				nodeList.erase(nodeList.begin());

				if (nodePtr->GetSkeleton())
				{
					return nodePtr;
				}

				if (nodePtr->GetChildCount(false)>0)
				{
					for (int i=0;i<nodePtr->GetChildCount(false);i++)
					{
						KFbxNode* subNode = nodePtr->GetChild(i);
						nodeList.push_back(subNode);
					}
				}

			} while (!ptrList.empty());
		}
		return 0;
	}
	void FbxImporter::BuildMesh(KFbxNode* node)
	{
		m_pFBXGeometryConverter->TriangulateInPlace(node);

	}

	void FbxImporter::ProcessNode(const Matrix& parentAbsTrans, KFbxNode* fbxNode, 
		bool partofMainSkeleton, bool warnIfBoneButNotChild)
	{
		KFbxObject* fbxObj = dynamic_cast<KFbxObject*>(fbxNode);
		assert(fbxObj);

		string objName = GetNameForFbxObject(fbxObj);

		KFbxNodeAttribute* attr = fbxNode->GetNodeAttribute();
		if (attr)
		{
			KFbxNodeAttribute::EAttributeType type = attr->GetAttributeType();
			switch (type)
			{
			case KFbxNodeAttribute::eSKELETON:
				if (!IsSkeletonRoot(fbxNode))
				{
					
				}
				partofMainSkeleton = true;
				break;
			case KFbxNodeAttribute::eMESH:

				break;
			
			case KFbxNodeAttribute::eUNIDENTIFIED:
			case KFbxNodeAttribute::eNULL:
			case KFbxNodeAttribute::eMARKER:
			case KFbxNodeAttribute::eNURB:
			case KFbxNodeAttribute::ePATCH:
			case KFbxNodeAttribute::eCAMERA:
			case KFbxNodeAttribute::eCAMERA_STEREO:
			case KFbxNodeAttribute::eCAMERA_SWITCHER:
			case KFbxNodeAttribute::eLIGHT:
			case KFbxNodeAttribute::eOPTICAL_REFERENCE:
			case KFbxNodeAttribute::eOPTICAL_MARKER:

				break;
			default:
				{

				}
			}
			
		}

		for (int i=0;i<fbxNode->GetChildCount(false);i++)
		{
			KFbxNode* subNode = fbxNode->GetChild(i);

			ProcessNode(subNode);
			
		}
	}

	FbxImporter::FbxImporter()
	{
		
	}

	void FbxImporter::Import(const string& fileName)
	{
		KFbxSdkManager* lSdkManager = NULL;
		KFbxScene* lScene = NULL;

		InitializeSdkObjects(lSdkManager, lScene);
		m_pFBXSdkManager = lSdkManager;

		m_pFBXGeometryConverter = new KFbxGeometryConverter(m_pFBXSdkManager);

		bool lResult = LoadScene(lSdkManager, lScene, fileName.c_str());

		if (!lResult)
		{
			cout << "An error occurred while loading the scene...";
		}
		else
		{
			KFbxAxisSystem currentAxisSystem = lScene->GetGlobalSettings().GetAxisSystem();

			KFbxAxisSystem axisSystem = KFbxAxisSystem(KFbxAxisSystem::eUpVector::YAxis, 
				KFbxAxisSystem::eFrontVector::ParityOdd, 
				KFbxAxisSystem::eCoorSystem::RightHanded);

			if (axisSystem != currentAxisSystem)
			{
				axisSystem.ConvertScene(lScene);
			}
			KFbxSystemUnit currentUnit = lScene->GetGlobalSettings().GetSystemUnit();

			if (currentUnit.GetScaleFactor() != 1.0f)
			{
				KFbxSystemUnit unit = KFbxSystemUnit(1,1);
				
				unit.ConvertScene(lScene);
			}

			KFbxNode* node = lScene->GetRootNode();

			if (node->GetChildCount(false) == 1 && 
				IsANullNode(node) && IsAnInterestingNode(node->GetChild(0)) &&
				NodesAbsoluteTransformIsIdentity(node))
			{
				node = node->GetChild(0);
			}
			m_skeletonRoot = FindRootMostBone(node);
			CacheBindPose(lScene);
			ProcessNode(Matrix::Identity, node, false, true);
			// add animation
		}

		lScene->Destroy();
		delete m_pFBXGeometryConverter;
		DestroySdkObjects(lSdkManager);
	}
}