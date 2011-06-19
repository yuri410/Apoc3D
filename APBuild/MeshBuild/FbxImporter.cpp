#include "FbxImporter.h"

#include "CompileLog.h"
#include "IOLib/ModelData.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(m_pFBXSdkManager->GetIOSettings()))
#endif
#define MAXBONES_PER_VERTEX 4

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

	bool HasFBXAnimation(KFbxNode *pNode)
	{
		bool hasKeys = false;

		KFbxProperty prop = pNode->GetFirstProperty();

		while(prop.IsValid())
		{
			KFCurveNode *fcn = prop.GetCurveNode()->GetKFCurveNode();
			if(fcn && fcn->KeyGetCount() > 0)
			{
				hasKeys = true;
			}
			prop = pNode->GetNextProperty(prop);
		}

		return hasKeys;
	}

	Vector3 ConvertVector3(const KFbxVector4& v)
	{
		return Vector3Utils::LDVector( v.GetAt(0), v.GetAt(1), v.GetAt(2) );
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
			CompileLog::getSingleton().WriteError(L"Cannot import files with password.", pFilename);

			return false;
		}

		KFbxAxisSystem currentAxisSystem = m_pFBXScene->GetGlobalSettings().GetAxisSystem();

		KFbxAxisSystem axisSystem = KFbxAxisSystem(KFbxAxisSystem::eUpVector::YAxis, 
			KFbxAxisSystem::eFrontVector::ParityOdd, 
			KFbxAxisSystem::eCoorSystem::RightHanded);

		if (axisSystem != currentAxisSystem)
		{
			axisSystem.ConvertScene(m_pFBXScene);
		}
		KFbxSystemUnit currentUnit = m_pFBXScene->GetGlobalSettings().GetSystemUnit();

		if (currentUnit.GetScaleFactor() != 1.0f)
		{
			KFbxSystemUnit unit = KFbxSystemUnit(1,1);

			unit.ConvertScene(m_pFBXScene);
		}

		ProcessScene(m_pFBXScene);

		lImporter->Destroy();

		return true;
	}

	void FbxImporter::ProcessScene(KFbxScene* pScene)
	{
		ProcessMaterials(pScene);
		ProcessNode(pScene->GetRootNode(), KFbxNodeAttribute::eSKELETON);
		ProcessNode(pScene->GetRootNode(), KFbxNodeAttribute::eMESH);
		ProcessAnimations(pScene);
	}

	void FbxImporter::ProcessMaterials(KFbxScene* pScene)
	{
		for( int i = 0; i < pScene->GetMaterialCount(); ++i )
		{
			MaterialData* pMaterial = new MaterialData();

			KFbxSurfaceMaterial* pFBXMaterial = pScene->GetMaterial(i);

			KFbxProperty diffuseTextureProperty = pFBXMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
			if( diffuseTextureProperty.IsValid() )
			{
				KFbxFileTexture* pDiffuseTexture = KFbxCast<KFbxFileTexture>(diffuseTextureProperty.GetSrcObject(KFbxTexture::ClassId, 0));

				if( pDiffuseTexture )
				{
					std::string strFileName = pDiffuseTexture->GetFileName();
					if( strFileName.length() == 0 )
						strFileName = pDiffuseTexture->GetRelativeFileName();
					pMaterial->TextureName[i] = toWString( strFileName );
				}
			}

			KFbxSurfaceLambert* pLambert = dynamic_cast<KFbxSurfaceLambert*>(pFBXMaterial);
			KFbxSurfacePhong* pPhong = dynamic_cast<KFbxSurfacePhong*>(pFBXMaterial);

			Vector3 AmbientColor;
			Vector3 EmissiveColor;
			Vector3 DiffuseColor;
			Vector3 SpecularColor;
			float fSpecularPower = 1.0f;
			float fTransparency = 1.0f;

			if( pLambert )
			{
				AmbientColor = GetMaterialColor(pLambert->GetAmbientColor(), pLambert->GetAmbientFactor());
				EmissiveColor = GetMaterialColor(pLambert->GetEmissiveColor(), pLambert->GetEmissiveFactor());
				DiffuseColor = GetMaterialColor(pLambert->GetDiffuseColor(), pLambert->GetDiffuseFactor());

				KFbxPropertyDouble1 FBXTransparencyProperty = pLambert->GetTransparencyFactor();
				if( FBXTransparencyProperty.IsValid() )
					fTransparency = FBXTransparencyProperty.Get();
			}
			if( pPhong )
			{
				SpecularColor = GetMaterialColor(pPhong->GetSpecularColor(), pPhong->GetSpecularFactor());

				KFbxPropertyDouble1 FBXSpecularPowerProperty = pPhong->GetShininess();
				if( FBXSpecularPowerProperty.IsValid() )
					fSpecularPower = FBXSpecularPowerProperty.Get();
			}

			pMaterial->Ambient = Color4( AmbientColor );
			pMaterial->Diffuse = Color4( DiffuseColor );
			pMaterial->Emissive = Color4( EmissiveColor );
			pMaterial->Specular = Color4( SpecularColor );
			pMaterial->Power = fSpecularPower;
			pMaterial->Diffuse.Alpha = fTransparency;
			if (fTransparency<1-EPSILON)
			{
				pMaterial->IsBlendTransparent = true;
			}

			//pMaterial->TextureName[i]
			//pMaterial->AddTexturePath( GetFilePath(this->m_strFileName) + "/" );
			m_materials.Add(pMaterial);
			m_FBXMaterials.Add(pFBXMaterial);
		}
	}
	void FbxImporter::ProcessAnimation(KFbxNode* pNode, 
		const char* strTakeName, float fFrameRate, float fStart, float fStop)
	{
		KFbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if (pNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
			{
				if( m_pSkeleton )
				{
					SkeletonBone* pBone = m_pSkeleton->FindBone(pNode->GetName());

					if( pBone )
					{

						AnimationKeyFrames* pAnimationKeyFrames = new AnimationKeyFrames(strTakeName);

						double fTime = 0;
						while( fTime <= fStop )
						{
							KTime takeTime;
							takeTime.SetSecondDouble(fTime);

							Matrix matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, takeTime);
							Matrix matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode->GetParent(), takeTime);
							Matrix matInvParentAbsoluteTransform;
							Matrix::Inverse(matInvParentAbsoluteTransform, matParentAbsoluteTransform);
							Matrix matTransform;
							Matrix::Multiply(matTransform, matAbsoluteTransform, matInvParentAbsoluteTransform);

							pAnimationKeyFrames->AddKeyFrame(matTransform);

							fTime += 1.0f/fFrameRate;
						}

						pBone->AddAnimationKeyFrames(pAnimationKeyFrames);
					}
				}
			}
			else if (pNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
			{
				FIMesh* pModel = m_meshes[pNode->GetName()];

				if( pModel )
				{
					AnimationKeyFrames* pAnimationKeyFrames = new AnimationKeyFrames(strTakeName);

					double fTime = 0;
					while( fTime <= fStop )
					{
						KTime takeTime;
						takeTime.SetSecondDouble(fTime);

						Matrix matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, takeTime);

						pAnimationKeyFrames->AddKeyFrame(matAbsoluteTransform);

						fTime += 1.0f/fFrameRate;
					}

					pModel->AddAnimationKeyFrames(pAnimationKeyFrames);
				}
			}
		}

		for( int i = 0; i < pNode->GetChildCount(); ++i )
		{
			ProcessAnimation(pNode->GetChild(i), strTakeName, fFrameRate, fStart, fStop);
		}
	}
	void FbxImporter::ProcessAnimations(KFbxScene* pScene)
	{
		//m_pAnimationController = new CBTTAnimationController();

		KFbxNode* pRootNode = pScene->GetRootNode();
		if(!pRootNode)
			return;

		float fFrameRate = (float)KTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode());

		KArrayTemplate<KString*> takeArray;	
		KFbxDocument* pDocument = dynamic_cast<KFbxDocument*>(pScene);
		if( pDocument )
			pDocument->FillAnimStackNameArray(takeArray);


		for( int i = 0; i < takeArray.GetCount(); ++i )
		{
			KString* takeName = takeArray.GetAt(i);

			if( std::string(takeName->Buffer()) != "Default" )
			{
				KFbxTakeInfo* lCurrentTakeInfo = pScene->GetTakeInfo(*takeName);

				//pScene->SetCurrentTake(takeName->Buffer());
				
				KTime KStart;
				KTime KStop;
				if (lCurrentTakeInfo)
				{
					KStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
					KStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
				}
				else
				{
					// Take the time line value
					KTimeSpan lTimeLineTimeSpan;
					pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

					KStart = lTimeLineTimeSpan.GetStart();
					KStop  = lTimeLineTimeSpan.GetStop();
				}

				//KTime KStart = KTIME_INFINITE;
				//KTime KStop = KTIME_MINUS_INFINITE;
				//pRootNode->GetAnimationInterval(KStart, KStop);

				float fStart = KStart.GetSecondDouble();
				float fStop = KStop.GetSecondDouble();

				if( fStart < fStop )
				{
					int nKeyFrames = (fStop-fStart)*fFrameRate;

					//CBTTAnimation* pAnimation = new CBTTAnimation(takeName->Buffer(), nKeyFrames, fFrameRate);
					//m_pAnimationController->AddAnimation(pAnimation);

					ProcessAnimation(pRootNode, takeName->Buffer(), fFrameRate, fStart, fStop);

					//m_pAnimationController->SetCurrentAnimation(0);
					//m_pAnimationController->Play();
				}
			}
		}

		takeArray.Clear();

		
		
		//pScene->SetTakeInfo();
		//pScene->SetCurrentTake("Default");
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
		bool isAnimated = HasFBXAnimation(pNode);

		KFbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();
		if( !pFBXSkeleton )
			return;

		if( !m_pSkeleton )
		{
			m_pSkeleton = new Skeleton();
		}

		int nParentBoneIndex = -1;
		KFbxNode* pParentNode = pNode->GetParent();
		if( pParentNode )
			nParentBoneIndex = m_pSkeleton->FindBoneIndex(pParentNode->GetName());

		SkeletonBone* pSkeletonBone = new SkeletonBone( pNode->GetName(), nParentBoneIndex );
		m_pSkeleton->AddSkeletonBone(pSkeletonBone);
	}
	void FbxImporter::ProcessMesh(KFbxNode* pNode)
	{
		bool isAnimated = HasFBXAnimation(pNode);
		
		KFbxGeometryConverter GeometryConverter(m_pFBXSdkManager);
		if( !GeometryConverter.TriangulateInPlace( pNode ) )
			return;

		KFbxMesh* pFBXMesh = pNode->GetMesh();
		if( !pFBXMesh )
			return;

		int nVertexCount = pFBXMesh->GetControlPointsCount();
		if( nVertexCount <= 0 )
			return;

		std::vector<BoneWeight> boneWeights(nVertexCount, BoneWeight());
		ProcessBoneWeights(pFBXMesh, boneWeights);

		FIMesh* mesh = new FIMesh(pNode->GetName());
		//mesh->Name = toWString( pNode->GetName() );
		//mesh->VertexCount = nVertexCount;
		
		FastList<MaterialData*> meshMtrls;

		KFbxVector4* pControlPoints = pFBXMesh->GetControlPoints();
		for( int pi = 0; pi < pFBXMesh->GetPolygonCount(); ++pi )
		{
			MaterialData* pMaterial = 0;

			for( int pvi = 0; pvi < 3; ++pvi )
			{
				int nVertexIndex = pFBXMesh->GetPolygonVertex(pi, pvi);

				if( nVertexIndex < 0 || nVertexIndex >= nVertexCount )
					continue;

				if( pMaterial == 0 )
					pMaterial = GetMaterialLinkedWithPolygon(pFBXMesh, 0, pi, 0, nVertexIndex);


				KFbxVector4 fbxPosition = pControlPoints[nVertexIndex];
				KFbxVector4 fbxNormal;
				pFBXMesh->GetPolygonVertexNormal(pi, pvi, fbxNormal);
				fbxNormal.Normalize();

				//if (!mesh->VertexData)
				//{

				//}
				mesh->AddVertex(pMaterial, ConvertVector3(fbxPosition),
					ConvertVector3(fbxNormal),
					GetTexCoord(pFBXMesh, 0, pi, pvi, nVertexIndex),
					boneWeights[nVertexIndex]);
			}

		}
		m_meshes.Add(pNode->GetName(), mesh);
	}

	void FbxImporter::ProcessBoneWeights(KFbxSkin* pFBXSkin, std::vector<BoneWeight>& meshBoneWeights)
	{
		KFbxCluster::ELinkMode linkMode = KFbxCluster::eNORMALIZE; //Default link mode

		std::vector<BoneWeight> skinBoneWeights(meshBoneWeights.size(), BoneWeight());
		int nClusterCount = pFBXSkin->GetClusterCount();
		for( int i = 0; i < nClusterCount; ++i )
		{
			KFbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);

			if( !pFBXCluster )
				continue;

			linkMode = pFBXCluster->GetLinkMode();
			KFbxNode* pLinkNode = pFBXCluster->GetLink();

			if( !pLinkNode )
				continue;

			int nBoneIndex = m_pSkeleton->FindBoneIndex(pLinkNode->GetName());
			if( nBoneIndex < 0 )
				continue;

			SkeletonBone* pSkeletonBone = m_pSkeleton->GetSkeletonBone(nBoneIndex);

			KFbxXMatrix matClusterTransformMatrix;
			KFbxXMatrix matClusterLinkTransformMatrix;
			pFBXCluster->GetTransformMatrix(matClusterTransformMatrix);
			pFBXCluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

			pSkeletonBone->SetBindPoseTransform(ConvertMatrix(matClusterLinkTransformMatrix));
			pSkeletonBone->SetBoneReferenceTransform(ConvertMatrix(matClusterTransformMatrix));

			int* indices = pFBXCluster->GetControlPointIndices();
			double* weights = pFBXCluster->GetControlPointWeights();

			for( int j = 0; j < pFBXCluster->GetControlPointIndicesCount(); ++j )
			{
				skinBoneWeights[indices[j]].AddBoneWeight(nBoneIndex, (float)weights[j]);
			}
		}

		switch(linkMode)
		{
		case KFbxCluster::eNORMALIZE:	//Normalize so weight sum is 1.0.
			for( int i = 0; i < (int)skinBoneWeights.size(); ++i )
			{
				skinBoneWeights[i].NormalizeInPlace();
			}
			break;

		case KFbxCluster::eADDITIVE:	//Not supported yet. Do nothing
			break;

		case KFbxCluster::eTOTAL1:		//The weight sum should already be 1.0. Do nothing.
			break;
		}

		for( int i = 0; i < (int)meshBoneWeights.size(); ++i )
		{
			meshBoneWeights[i].AddBoneWeight(skinBoneWeights[i]);
		}	
	}
	void FbxImporter::ProcessBoneWeights(KFbxMesh* pFBXMesh, std::vector<BoneWeight>& meshBoneWeights)
	{
		if( !m_pSkeleton )
			return;

		for( int i = 0; i < pFBXMesh->GetDeformerCount(); ++i )
		{
			KFbxDeformer* pFBXDeformer = pFBXMesh->GetDeformer(i);

			if( !pFBXDeformer )
				continue;

			if( pFBXDeformer->GetDeformerType() == KFbxDeformer::eSKIN )
			{
				KFbxSkin* pFBXSkin = dynamic_cast<KFbxSkin*>(pFBXDeformer);

				if( !pFBXSkin )
					continue;

				ProcessBoneWeights(pFBXSkin, meshBoneWeights);
			}
		}
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
				Color = Vector3Utils::Multiply(Color, static_cast<float>(FBXFactor));
			}
		}
		return Color;
	}
}