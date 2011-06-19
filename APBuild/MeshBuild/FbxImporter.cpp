#include "FbxImporter.h"

#include "CompileLog.h"

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

	void GetGeomentryOffset(KFbxMatrix* matrix, KFbxNode* node)
	{
		KFbxVector4 transl = node->GetGeometricTranslation(eSOURCE_SET);
		KFbxVector4 rotation = node->GetGeometricRotation(eSOURCE_SET);
		KFbxVector4 scaling = node->GetGeometricScaling(eSOURCE_SET);

		matrix->SetTRS(transl, rotation, scaling);
	}
	Matrix GetAbsoluteTransform(KFbxNode* fbxNode, const KTime& time)
	{
		KFbxMatrix globalTrans = fbxNode->EvaluateGlobalTransform(time);
		KFbxMatrix m;
		GetGeomentryOffset(&m, fbxNode);
		m = globalTrans * m;
		return ConvertMatrix(m);
	}
	CalculateNodeTransformResult CalculateNodeTransformsAtTime(const Matrix& parentAbsTransform, KFbxNode* node, const KTime& time)
	{
		CalculateNodeTransformResult result;
		result.AbsoluteTransform = GetAbsoluteTransform(node, time);
		Matrix invParent;
		Matrix::Inverse(invParent, parentAbsTransform);
		Matrix::Multiply(result.LocalTransform, result.AbsoluteTransform, invParent);
		return result;
	}
	CalculateNodeTransformResult CalculateNodeTransformsAtTime(KFbxNode* fbxNode, const KTime& time)
	{
		if (!fbxNode->GetParent())
		{

		}
		return CalculateNodeTransformsAtTime(Matrix::Identity, fbxNode, time);
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void AddAnimationChannel(AnimationContentDictionary* animationDictionary, string animationName, string boneName, 
		AnimationTake* animationTake, double duration)
	{
		AnimationContent* content = 0;
		if (!animationDictionary->TryGetValue(animationName, content))
		{
			content = new AnimationContent(animationName);

			content->Duration = 0;
			animationDictionary->Add(animationName, content);
		}

		AnimationTake* channel = 0;
		if (!content->Takes.TryGetValue(boneName, channel))
		{
			channel = new AnimationTake();
			content->Takes.Add(boneName, channel);
		}
		for (int i=0;i<animationTake->Keyframes.getCount();i++)
		{
			channel->Keyframes.Add(keyframe);
		}
		double span = duration;
		double span2 = content->Duration;
		if (span > span2)
		{
			content->Duration = span;
		}
	}

	KFbxNode* AnimationConverter::FindRootMostBone(KFbxNode* sceneRoot)
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

			} while (!nodeList.empty());
		}
		return 0;
	}
	void AnimationConverter::AddAnimationToNodesRecursive(NodeContent* node, const string& takeName)
	{
		KFbxNode* fbxNode = node->FBXNode;

		KTime start;
		KTime end;
		
		fbxNode->GetAnimationInterval(start, end);
		double dend = start.GetSecondDouble();
		double dstart = end.GetSecondDouble();
		double invFrameRate = 1/60.0;

		AnimationTake* take = new AnimationTake();
		
		bool flag = false;
		for (double t = dstart; t<dend; t+=invFrameRate)
		{
			KTime time;
			time.SetSecondDouble(t);

			CalculateNodeTransformResult trs;

			if (!flag && node->Transform != trs.LocalTransform)
			{
				flag = true;
			}

			AnimationKeyframe keyframe;
			take->Keyframes.Add(keyframe);
		}
		if (flag)
		{
			string nameForFbxObject = GetNameForFbxObject(fbxNode);
			string animationName = takeName;
			AddAnimationChannel(!(node->PartOfMainSkeleton) ? (&node->Animations) : (&m_SkeletonRoot->Animations), 
				animationName, nameForFbxObject, take, dend);
		}

		for (int i=0;i<node->Children.getCount();i++)
		{
			AddAnimationToNodesRecursive(node->Children[i], takeName);
		}
	}

	void AnimationConverter::HierarchyFixup(NodeContent* sceneRoot)
	{
		AnimationTake* channel = 0;
		
		KFbxNode* nodePtr = m_SkeletonRoot->FBXNode;
		//**(((int*) nodePtr))[0xa8](nodePtr, 0);
		nodePtr->SetName(0);

		for (int i=0;i<nodePtr->GetChildCount();i++)
		{
			SetCurrentTakeRecursive(nodePtr->GetChild(i), 0);
		}
		KTime time;

		Matrix invSceneTrans;
		Matrix::Inverse(invSceneTrans, sceneRoot->GetAbsoluteTransfrom());
		Matrix m1;
		Matrix::Multiply(m1, GetAbsoluteTransform(nodePtr, time), invSceneTrans);
		Matrix m2;
		Matrix::Inverse(invSceneTrans, m_SkeletonRoot->Transform);
		Matrix m3;
		Matrix::Multiply(m3, m1, m2);
		m_SkeletonRoot->Transform = m1;

		AnimationContentDictionary::Enumerator e2 = m_SkeletonRoot->Animations.GetEnumerator();
		while (1)
		{
			do
			{
				if (!e2.MoveNext())
				{
					return;
				}
				channel = 0;
			}
			while (!e2.getCurrentValue()->Takes.TryGetValue(m_SkeletonRoot->Name, channel));

			for (int i=0;i<channel->Keyframes.getCount();i++)
			{
				AnimationKeyframe& current = channel->Keyframes[i];
				Matrix matrix;
				Matrix.Multiply(matrix, current.Transform, m3);
				current.Transform = matrix;
			}
			
		}


	}
	void AnimationConverter::SetCurrentTakeRecursive(KFbxNode* node, const string& takeName)
	{
		node->SetName(takeName.c_str());
		
		//if (takeName != null)
		//{
		//	
		//	**(((int*) node))[0xa4](node, takeName);
		//}
		//else
		//{
		//	**(((int*) node))[0xa8](node, 0);
		//}
		for (int i=0;i<node->GetChildCount();i++)
		{
			SetCurrentTakeRecursive(node->GetChild(i), takeName);
		}
		//int num = 0;
		//if (0 < fbxsdk_200901a.KFbxNode.GetChildCount((KFbxNode modopt(IsConst)* modopt(IsConst) modopt(IsConst)) node, false))
		//{
		//	do
		//	{
		//		SetCurrentTakeRecursive(fbxsdk_200901a.KFbxNode.GetChild(node, num), takeName);
		//		num++;
		//	}
		//	while (num < fbxsdk_200901a.KFbxNode.GetChildCount((KFbxNode modopt(IsConst)* modopt(IsConst) modopt(IsConst)) node, false));
		//}

	}
	void AnimationConverter::WarnAboutAnimatedNodes(NodeContent* node)
	{
		AnimationTake* channel = 0;
		
		//foreach (KeyValuePair<string, AnimationContent> pair in node.Animations)
		//{
		//	if (pair.Value.Channels.TryGetValue(node.Name, out channel) && (channel.Count > 0))
		//	{
		//		ContentIdentity contentIdentity = new ContentIdentity(this.m_ContentIdentity.SourceFilename, this.m_ContentIdentity.SourceTool, node.Name);
		//		object[] messageArgs = new object[] { this.m_SkeletonRoot.Name, node.Name, pair.Key };
		//		this.m_ImporterContext.Logger.LogWarning("FbxImporter_PulledOutSkeletonRootButParentIsAnimated", contentIdentity, ResourceStrings.PulledOutSkeletonRootButParentIsAnimated, messageArgs);
		//	}
		//}
		
		if (!node->Parent)
		{
			WarnAboutAnimatedNodes(node->Parent);
		}

	}

	void AnimationConverter::AddAnimationInformationToScene(KFbxScene* fbxScene, NodeContent* sceneRoot)
	{
		KArrayTemplate<KString*> takeNames;
		NodeContent* skeletonRoot = m_SkeletonRoot;
		if (!skeletonRoot && (sceneRoot != skeletonRoot))
		{
			sceneRoot->Children.Add(skeletonRoot);
		}

		fbxScene->FillAnimStackNameArray(takeNames);

		for (int i=0;i<takeNames.GetCount();i++)
		{
			KString* takeName = takeNames[i];
			SetCurrentTakeRecursive(fbxScene->GetRootNode(), takeName->operator const char *());
		}

		if (m_SkeletonRoot)
		{
			HierarchyFixup(sceneRoot);
		}

		NodeContent* skeletonOldParent = m_SkeletonOldParent;
		if (skeletonOldParent)
		{
			WarnAboutAnimatedNodes(skeletonOldParent);
		}

	}


	void AnimationConverter::HierarchyFixup(NodeContent* sceneRoot)
	{
		AnimationTake* channel = 0;
		KTime time;
		//FbxNodeInfo info = this.m_NodeContentToFbxNodeInfo[this.m_SkeletonRoot];
		//IntPtr fbxNode = info.fbxNode;
		KFbxNode* nodePtr = m_SkeletonRoot->FBXNode;

		**(((int*) nodePtr))[0xa8](nodePtr, 0);
		for (int i=0;i<nodePtr->GetChildCount();i++)
		{
			//SetCurrentTakeRecursive(nodePtr->GetChild(i), );
		}
		//int num = 0;
		//if (0 < fbxsdk_200901a.KFbxNode.GetChildCount((KFbxNode modopt(IsConst)* modopt(IsConst) modopt(IsConst)) nodePtr, false))
		//{
		//	do
		//	{
		//		SetCurrentTakeRecursive(fbxsdk_200901a.KFbxNode.GetChild(nodePtr, num), null);
		//		num++;
		//	}
		//	while (num < fbxsdk_200901a.KFbxNode.GetChildCount((KFbxNode modopt(IsConst)* modopt(IsConst) modopt(IsConst)) nodePtr, false));
		//}
		//*((long*) &time) = 0;
		//Matrix matrix3 = Matrix.Invert(sceneRoot.AbsoluteTransform);
		//ValueType modopt(Matrix) modopt(IsBoxed) type = Matrix.Multiply((Matrix) HelperFunctions.GetAbsoluteTransform(nodePtr, time), matrix3);
		//Matrix matrix2 = Matrix.Invert(this.m_SkeletonRoot.Transform);
		//ValueType modopt(Matrix) modopt(IsBoxed) type2 = Matrix.Multiply((Matrix) type, matrix2);
		//this.m_SkeletonRoot.Transform = (Matrix) type;
		//using (IEnumerator<AnimationContent> enumerator2 = this.m_SkeletonRoot.Animations.Values.GetEnumerator())
		//{
		//	while (true)
		//	{
		//		do
		//		{
		//			if (!enumerator2.MoveNext())
		//			{
		//				return;
		//			}
		//			channel = null;
		//		}
		//		while (!enumerator2.Current.Channels.TryGetValue(this.m_SkeletonRoot.Name, out channel));
		//		IEnumerator<AnimationKeyframe> enumerator = channel.GetEnumerator();
		//		try
		//		{
		//			while (enumerator.MoveNext())
		//			{
		//				AnimationKeyframe current = enumerator.Current;
		//				Matrix matrix = Matrix.Multiply(current.Transform, (Matrix) type2);
		//				current.Transform = matrix;
		//			}
		//		}
		//		finally
		//		{
		//			IEnumerator<AnimationKeyframe> enumerator3 = enumerator;
		//			IDisposable disposable = enumerator;
		//			if (enumerator != null)
		//			{
		//				enumerator.Dispose();
		//			}
		//		}
		//	}
		//}

	}
	void AnimationConverter::NodeWasCreated(KFbxNode* fbxNode, NodeContent* node, NodeContent* potentialParent, bool partOfMainSkeleton)
	{

	}




	/************************************************************************/
	/*                                                                      */
	/************************************************************************/



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
				Matrix matrix2 = ConvertMatrix(matrix);

				m_bindPoseCache.Add(name2, matrix2);
			}
		}
	}

	CalculateNodeTransformResult FbxImporter::CalculateNodeTransformBindPose(const Matrix& parentAbsTransform, KFbxNode* node)
	{
		KString name = node->GetName();
		string key = name.operator const char *();

		Matrix trans;
		if (m_bindPoseCache.TryGetValue(key, trans))
		{
			CalculateNodeTransformResult result;
			result.AbsoluteTransform = trans;
			Matrix invParent;
			Matrix::Inverse(invParent, parentAbsTransform);
			Matrix::Multiply(result.LocalTransform, trans, invParent);
			return result;
		}
		KTime time;
		return CalculateNodeTransformsAtTime(parentAbsTransform, node, time);
	}


	//void FbxImporter::BuildMesh(KFbxNode* node)
	//{
	//	KFbxGeometryConverter converter(m_pFBXSdkManager);
	//	converter.TriangulateInPlace(node);
	//}

	NodeContent* FbxImporter::ProcessNode(const Matrix& parentAbsTrans, KFbxNode* fbxNode, 
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

		if (m_skeletonRoot != fbxNode)
		{

		}
		
		CalculateNodeTransformResult transform = CalculateNodeTransformBindPose(parentAbsTrans, node);

		for (int i=0;i<fbxNode->GetChildCount(false);i++)
		{
			KFbxNode* subNode = fbxNode->GetChild(i);
			//IsSkeletonRoot(subNode)
			ProcessNode(transform.AbsoluteTransform, subNode, partofMainSkeleton, warnIfBoneButNotChild);;
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

		//m_pFBXGeometryConvertser = new KFbxGeometryConverter(m_pFBXSdkManager);

		bool lResult = LoadScene(lSdkManager, lScene, fileName.c_str());

		if (!lResult)
		{
			CompileLog::getSingleton().WriteError(L"Cannot open model file.", fileName);
			//cout << "An error occurred while loading the scene...";
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
		DestroySdkObjects(lSdkManager);
	}
}