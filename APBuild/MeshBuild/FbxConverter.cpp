#include "FbxConverter.h"

#include "BuildConfig.h"
#include "BuildSystem.h"

#include "MeshBuild.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(m_pFBXSdkManager->GetIOSettings()))
#endif

namespace APBuild
{

	bool HasFBXAnimation(FbxNode *pNode)
	{
		//pNode->GetAnimationInterval();
		bool hasKeys = false;

		FbxProperty prop = pNode->GetFirstProperty();

		while(prop.IsValid())
		{
			FbxAnimCurveNode* cn = prop.GetCurveNode();
			if (cn)
			{
				hasKeys = true;
				break;
				//KFCurveNode *fcn = cn->GetKFCurveNode();
				//if(fcn && fcn->KeyGetCount() > 0)
				//{
				//	hasKeys = true;
				//	break;
				//}
			}
			
			prop = pNode->GetNextProperty(prop);
		}

		return hasKeys;
	}

	Vector3 ConvertVector3(const FbxVector4& v)
	{
		return Vector3( 
			static_cast<float>(v[0]),
			static_cast<float>(v[1]),
			static_cast<float>(v[2]) );
	}
	Vector2 ConvertVector2(const FbxVector2& v)
	{
		return Vector2(
			static_cast<float>(v[0]),
			static_cast<float>(v[1]) );
	}
	Vector2 ConvertTexCoord(const FbxVector2& v)
	{
		return Vector2(
			static_cast<float>(v[0]), 
			static_cast<float>(1-v[1]) );
	}
	Matrix ConvertMatrix(const FbxMatrix& m)
	{
		FbxVector4 r1 = m.GetRow(0);
		FbxVector4 r2 = m.GetRow(1);
		FbxVector4 r3 = m.GetRow(2);
		FbxVector4 r4 = m.GetRow(3);

		return Matrix( (float)r1[0], (float)r1[1], (float)r1[2], (float)r1[3],
			(float)r2[0], (float)r2[1], (float)r2[2], (float)r2[3], 
			(float)r3[0], (float)r3[1], (float)r3[2], (float)r3[3],
			(float)r4[0], (float)r4[1], (float)r4[2], (float)r4[3]);
	}


	FbxConverter::FbxConverter()
		: m_pSkeleton(0), m_FBXMaterials(0), m_pFBXScene(0), m_bakeTransform(false)
	{

	}
	FbxConverter::~FbxConverter()
	{
		for (int i=0;i<m_materials.getCount();i++)
		{
			delete m_materials[i];
		}
		for (HashMap<std::string, FIMesh*>::Enumerator i=m_meshes.GetEnumerator();i.MoveNext();)
		{
			delete *i.getCurrentValue();
		}
		delete m_pSkeleton;
	}


	bool FbxConverter::InitializeFBXSdk()
	{
		m_pFBXSdkManager = FbxManager::Create();
		if (!m_pFBXSdkManager) return false;


		// create an IOSettings object
		FbxIOSettings * ios = FbxIOSettings::Create(m_pFBXSdkManager, IOSROOT );
		m_pFBXSdkManager->SetIOSettings(ios);

		// Load plugins from the executable directory
		FbxString lPath = FbxGetApplicationDirectory();
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
		FbxString lExtension = "dll";
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_MAC
		FbxString lExtension = "dylib";
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_LINUX
		FbxString lExtension = "so";
#elif
#pragma error "FBX SDK does not not support this platform."
#endif
		m_pFBXSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

		m_pFBXScene = FbxScene::Create(m_pFBXSdkManager,"");
		if( !m_pFBXScene ) return false;

		return true;
	}

	bool FbxConverter::Initialize(const String& pFilename)
	{
		m_sourceFile = pFilename;

		if( !InitializeFBXSdk() )	return false;
		if( !LoadScene(pFilename) )	return false;

		if( m_pFBXScene )		m_pFBXScene->Destroy();
		if(m_pFBXSdkManager)	m_pFBXSdkManager->Destroy();

		m_pFBXScene = NULL;
		m_pFBXSdkManager = NULL;

		return true;
	}

	bool FbxConverter::LoadScene(const String& pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		//int lFileFormat = -1;
		//int i, lAnimStackCount;
		bool lStatus;
		//char lPassword[256];

		std::string mbFilename = StringUtils::toPlatformNarrowString(pFilename);

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(m_pFBXSdkManager,"");
		

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(
			mbFilename.c_str(), -1, m_pFBXSdkManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( !lImportStatus )
		{
			BuildSystem::LogError(L"Unable to initialize FBX", pFilename);
			BuildSystem::LogError(StringUtils::toPlatformWideString(lImporter->GetStatus().GetErrorString()), pFilename);
			
			if (lImporter->GetStatus() == FbxStatus::eInvalidFile ||
				lImporter->GetStatus() == FbxStatus::eInvalidFileVersion)
			{
				BuildSystem::LogError(
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
		
		if(lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
		{
			BuildSystem::LogError(L"Cannot import files with password.", pFilename);
			
			return false;
		}

		FbxAxisSystem currentAxisSystem = m_pFBXScene->GetGlobalSettings().GetAxisSystem();
		
		FbxAxisSystem axisSystem = FbxAxisSystem::DirectX;//KFbxAxisSystem(KFbxAxisSystem::YAxis, 
			//KFbxAxisSystem::ParityOdd, 
			//KFbxAxisSystem::RightHanded);

		if (axisSystem != currentAxisSystem)
		{
			axisSystem.ConvertScene(m_pFBXScene);
		}


		//KFbxSystemUnit currentUnit = m_pFBXScene->GetGlobalSettings().GetSystemUnit();

		//if (currentUnit.GetScaleFactor() != 1.0f)
		//{
		//	KFbxSystemUnit unit = KFbxSystemUnit(1,1);

		//	unit.ConvertScene(m_pFBXScene);
		//}

		ProcessScene(m_pFBXScene);

		lImporter->Destroy();

		return true;
	}

	void FbxConverter::ProcessScene(FbxScene* pScene)
	{
		ProcessMaterials(pScene);
		ProcessNode(pScene->GetRootNode(), FbxNodeAttribute::eSkeleton);
		ProcessNode(pScene->GetRootNode(), FbxNodeAttribute::eMesh);
		ProcessAnimations(pScene);
	}

	void FbxConverter::ProcessMaterials(FbxScene* pScene)
	{
		for( int i = 0; i < pScene->GetMaterialCount(); ++i )
		{
			MaterialData* pMaterial = new MaterialData();

			FbxSurfaceMaterial* pFBXMaterial = pScene->GetMaterial(i);

			FbxProperty diffuseTextureProperty = pFBXMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if( diffuseTextureProperty.IsValid() )
			{
				FbxFileTexture* pDiffuseTexture = diffuseTextureProperty.GetSrcObject<FbxFileTexture>(0);

				if( pDiffuseTexture )
				{
					std::string strFileName = pDiffuseTexture->GetFileName();
					if( strFileName.length() == 0 )
						strFileName = pDiffuseTexture->GetRelativeFileName();
					pMaterial->TextureName[0] = StringUtils::toPlatformWideString( strFileName );
				}
			}
			
			FbxSurfaceLambert* pLambert = FbxCast<FbxSurfaceLambert>(pFBXMaterial);
			FbxSurfacePhong* pPhong = FbxCast<FbxSurfacePhong>(pFBXMaterial);

			Vector3 AmbientColor;
			Vector3 EmissiveColor;
			Vector3 DiffuseColor;
			Vector3 SpecularColor;
			float fSpecularPower = 1.0f;
			float fTransparency = 1.0f;

			if( pLambert )
			{
				AmbientColor = GetMaterialColor(pLambert->Ambient, pLambert->AmbientFactor);
				EmissiveColor = GetMaterialColor(pLambert->Emissive, pLambert->EmissiveFactor);
				DiffuseColor = GetMaterialColor(pLambert->Diffuse, pLambert->DiffuseFactor);

				FbxPropertyT<double> FBXTransparencyProperty = pLambert->TransparencyFactor;
				if( FBXTransparencyProperty.IsValid() )
					fTransparency = static_cast<float>( FBXTransparencyProperty.Get() );
			}
			if( pPhong )
			{
				SpecularColor = GetMaterialColor(pPhong->Specular, pPhong->SpecularFactor);
				
				FbxPropertyT<double> FBXSpecularPowerProperty = pPhong->Shininess;
				if( FBXSpecularPowerProperty.IsValid() )
					fSpecularPower = static_cast<float>( FBXSpecularPowerProperty.Get() );
			}

			pMaterial->SetDefaults();
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
			

			m_materials.Add(pMaterial);
			m_FBXMaterials.Add(pFBXMaterial);
		}
	}
	void FbxConverter::ProcessAnimation(FbxNode* pNode, 
		const char* strTakeName, float fFrameRate, float fStart, float fStop)
	{
		FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			{
				if( m_pSkeleton )
				{
					FISkeletonBone* pBone = m_pSkeleton->FindBone(pNode->GetName());

					if( pBone )
					{

						FIPartialAnimation* pAnimationKeyFrames = new FIPartialAnimation(strTakeName);

						double fTime = 0;
						while( fTime <= fStop )
						{
							FbxTime takeTime;
							takeTime.SetSecondDouble(fTime);

							Matrix matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, takeTime);
							Matrix matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode->GetParent(), takeTime);
							Matrix matInvParentAbsoluteTransform;
							Matrix::Inverse(matInvParentAbsoluteTransform, matParentAbsoluteTransform);
							Matrix matTransform;
							Matrix::Multiply(matTransform, matAbsoluteTransform, matInvParentAbsoluteTransform);

							pAnimationKeyFrames->AddKeyFrame(FIAnimationKeyframe(matTransform, static_cast<float>(fTime)));

							fTime += 1.0f/fFrameRate;
						}

						pBone->AddAnimationKeyFrames(pAnimationKeyFrames);
					}
				}
			}
			else if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FIMesh* pModel = m_meshes[pNode->GetName()];

				if( pModel )
				{
					FIPartialAnimation* pAnimationKeyFrames = new FIPartialAnimation(strTakeName);

					double fTime = 0;
					while( fTime <= fStop )
					{
						FbxTime takeTime;
						takeTime.SetSecondDouble(fTime);

						Matrix matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, takeTime);

						pAnimationKeyFrames->AddKeyFrame(FIAnimationKeyframe(matAbsoluteTransform, static_cast<float>(fTime)));

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
	void FbxConverter::ProcessAnimations(FbxScene* pScene)
	{
		FbxNode* pRootNode = pScene->GetRootNode();
		if(!pRootNode)
			return;

		float fFrameRate = (float)FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode());
		
		FbxArray<FbxString*> takeArray;	
		FbxDocument* pDocument = FbxCast<FbxDocument>(pScene);
		if( pDocument )
			pDocument->FillAnimStackNameArray(takeArray);


		for( int i = 0; i < takeArray.GetCount(); ++i )
		{
			FbxString* takeName = takeArray.GetAt(i);
			

			if( std::string(takeName->Buffer()) != "Default" )
			{
				//KFbxTakeInfo* lCurrentTakeInfo = pScene->GetTakeInfo(*takeName);

				//pScene->SetCurrentTake(takeName->Buffer());
				pScene->ActiveAnimStackName.Set(*takeName);

				FbxTime KStart;
				FbxTime KStop;
				//if (lCurrentTakeInfo)
				//{
				//	KStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
				//	KStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
				//}
				//else
				{
					// Take the time line value
					FbxTimeSpan lTimeLineTimeSpan;
					pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

					KStart = lTimeLineTimeSpan.GetStart();
					KStop  = lTimeLineTimeSpan.GetStop();
				}

				//KTime KStart = KTIME_INFINITE;
				//KTime KStop = KTIME_MINUS_INFINITE;
				//pRootNode->GetAnimationInterval(KStart, KStop);

				float fStart = static_cast<float>(KStart.GetSecondDouble());
				float fStop = static_cast<float>(KStop.GetSecondDouble());

				if( fStart < fStop )
				{
					//int nKeyFrames = static_cast<int>((fStop-fStart)*fFrameRate);

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
	void FbxConverter::ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType)
	{
		if( !pNode )
			return;

		FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if( pNodeAttribute->GetAttributeType() == attributeType )
			{
				switch(pNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eSkeleton:
					ProcessSkeleton(pNode);
					break;
				case FbxNodeAttribute::eMesh:
					ProcessMesh(pNode);
					break;

				case FbxNodeAttribute::eMarker:
				case FbxNodeAttribute::eTrimNurbsSurface:
				case FbxNodeAttribute::ePatch:
				case FbxNodeAttribute::eCamera:
				case FbxNodeAttribute::eLight:
				case FbxNodeAttribute::eNull:
					break;
				};
			}
		}

		for( int i = 0; i < pNode->GetChildCount(); ++i )
		{
			ProcessNode(pNode->GetChild(i), attributeType);
		}
	}
	void FbxConverter::ProcessSkeleton(FbxNode* pNode)
	{
		//bool isAnimated = HasFBXAnimation(pNode);

		FbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();
		if( !pFBXSkeleton )
			return;

		if( !m_pSkeleton )
		{
			m_pSkeleton = new FISkeleton();
		}

		int nParentBoneIndex = -1;
		FbxNode* pParentNode = pNode->GetParent();
		if( pParentNode )
			nParentBoneIndex = m_pSkeleton->FindBoneIndex(pParentNode->GetName());

		FISkeletonBone* pSkeletonBone = new FISkeletonBone( pNode->GetName(), nParentBoneIndex );
		if (m_pSkeleton->FindBone(pNode->GetName()))
		{
			BuildSystem::LogError(String(L"There are more than one bone named ") + StringUtils::toPlatformWideString(pNode->GetName()) + String(L" in the FBX file."), m_sourceFile);
			return;
		}
		m_pSkeleton->AddSkeletonBone(pSkeletonBone);
	}
	void FbxConverter::ProcessMesh(FbxNode* pNode)
	{
		//bool isAnimated = HasFBXAnimation(pNode);
		
		FbxMesh* pFBXMesh = pNode->GetMesh();
		if( !pFBXMesh )
			return;

		FbxGeometryConverter GeometryConverter(m_pFBXSdkManager);
		if( !GeometryConverter.Triangulate( pFBXMesh, true ) )
			return;


		int nVertexCount = pFBXMesh->GetControlPointsCount();
		if( nVertexCount <= 0 )
			return;

		std::vector<BoneWeight> boneWeights(nVertexCount, BoneWeight());
		ProcessBoneWeights(pFBXMesh, boneWeights);

		FIMesh* mesh = new FIMesh(pNode->GetName());

		Matrix matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, 0);
		
		Matrix matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode->GetParent(), 0);
		Matrix matInvParentAbsoluteTransform;
		Matrix::Inverse(matInvParentAbsoluteTransform, matParentAbsoluteTransform);
		Matrix matTransform;
		Matrix::Multiply(matTransform, matAbsoluteTransform, matInvParentAbsoluteTransform);

		mesh->SetAbsoluteTransform(matAbsoluteTransform);
		mesh->SetParentRelativeTransform(matTransform);

		//mesh->Name = toWString( pNode->GetName() );
		//mesh->VertexCount = nVertexCount;
		
		FastList<MaterialData*> meshMtrls;

		FbxVector4* pControlPoints = pFBXMesh->GetControlPoints();
		int nLayerCount = pFBXMesh->GetLayerCount();
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


				FbxVector4 fbxPosition = pControlPoints[nVertexIndex];
				FbxVector4 fbxNormal;
				pFBXMesh->GetPolygonVertexNormal(pi, pvi, fbxNormal);
				fbxNormal.Normalize();

				//if (!mesh->VertexData)
				//{

				//}
				if (nLayerCount>1)
				{
					mesh->AddVertex(pMaterial, ConvertVector3(fbxPosition),
						ConvertVector3(fbxNormal),
						GetTexCoord(pFBXMesh, 0, pi, pvi, nVertexIndex),
						GetTexCoord(pFBXMesh, 1, pi, pvi, nVertexIndex),
						boneWeights[nVertexIndex]
					);
				}
				else
				{
					mesh->AddVertex(pMaterial, ConvertVector3(fbxPosition),
						ConvertVector3(fbxNormal),
						GetTexCoord(pFBXMesh, 0, pi, pvi, nVertexIndex),
						boneWeights[nVertexIndex]
					);
				}
				
			}

		}
		mesh->FinishAndOptimize();
		if (m_meshes.Contains(pNode->GetName()))
		{
			BuildSystem::LogError(String(L"There are more than one mesh named ") + StringUtils::toPlatformWideString(pNode->GetName()) + String(L" in the FBX file."), m_sourceFile);
			return;
		}
		else
		{
			m_meshes.Add(pNode->GetName(), mesh);
		}
		
	}

	void FbxConverter::FixSkeletonTransform(FbxNode* pSceneRoot,FbxNode* pNode)
	{
		FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			{
				if( m_pSkeleton )
				{
					FISkeletonBone* pBone = m_pSkeleton->FindBone(pNode->GetName());

					if( pBone )
					{
						FbxTime time = 0;

						// frame *= AbsoluteTransform * invRootTrans * invSkeleRootTrans
						// SkeleRootTrans = AbsoluteTransform * invRootTrans

						Matrix absoluteTransform = GetAbsoluteTransformFromCurrentTake(pNode, time);
						//Matrix invRootTrans = pSceneRoot;

					}
				}
			}
			else
			{
				for( int i = 0; i < pNode->GetChildCount(); ++i )
				{
					FixSkeletonTransform(pSceneRoot, pNode->GetChild(i));
				}
			}
		}
		
	}

	void FbxConverter::ProcessBoneWeights(FbxSkin* pFBXSkin, std::vector<BoneWeight>& meshBoneWeights)
	{
		FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize; //Default link mode

		std::vector<BoneWeight> skinBoneWeights(meshBoneWeights.size(), BoneWeight());
		int nClusterCount = pFBXSkin->GetClusterCount();
		for( int i = 0; i < nClusterCount; ++i )
		{
			FbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);

			if( !pFBXCluster )
				continue;

			linkMode = pFBXCluster->GetLinkMode();
			FbxNode* pLinkNode = pFBXCluster->GetLink();

			if( !pLinkNode )
				continue;

			int nBoneIndex = m_pSkeleton->FindBoneIndex(pLinkNode->GetName());
			if( nBoneIndex < 0 )
				continue;

			FISkeletonBone* pSkeletonBone = m_pSkeleton->GetSkeletonBone(nBoneIndex);

			FbxAMatrix matClusterTransformMatrix;
			FbxAMatrix matClusterLinkTransformMatrix;
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
		case FbxCluster::eNormalize:	//Normalize so weight sum is 1.0.
			for( int i = 0; i < (int)skinBoneWeights.size(); ++i )
			{
				skinBoneWeights[i].NormalizeInPlace();
			}
			break;

		case FbxCluster::eAdditive:	//Not supported yet. Do nothing
			break;

		case FbxCluster::eTotalOne:		//The weight sum should already be 1.0. Do nothing.
			break;
		}

		for( int i = 0; i < (int)meshBoneWeights.size(); ++i )
		{
			meshBoneWeights[i].AddBoneWeight(skinBoneWeights[i]);
		}	
	}
	void FbxConverter::ProcessBoneWeights(FbxMesh* pFBXMesh, std::vector<BoneWeight>& meshBoneWeights)
	{
		if( !m_pSkeleton )
			return;

		for( int i = 0; i < pFBXMesh->GetDeformerCount(); ++i )
		{
			FbxDeformer* pFBXDeformer = pFBXMesh->GetDeformer(i);

			if( !pFBXDeformer )
				continue;

			if( pFBXDeformer->GetDeformerType() == FbxDeformer::eSkin )
			{
				FbxSkin* pFBXSkin = FbxCast<FbxSkin>(pFBXDeformer);

				if( !pFBXSkin )
					continue;

				ProcessBoneWeights(pFBXSkin, meshBoneWeights);
			}
		}
	}

	int FbxConverter::GetMappingIndex(FbxLayerElement::EMappingMode MappingMode, 
		int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		switch(MappingMode)
		{
		case FbxLayerElement::eAllSame:
			return 0;

		case FbxLayerElement::eByControlPoint:
			return nVertexIndex;

		case FbxLayerElement::eByPolygonVertex:
			return nPolygonIndex*3 + nPolygonVertexIndex;

		case FbxLayerElement::eByPolygon:
			return nPolygonIndex;

		case FbxLayerElement::eNone:
		case FbxLayerElement::eByEdge:
			break;
		}
		return -1;
	}
	MaterialData* FbxConverter::GetMaterialLinkedWithPolygon(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		if( nLayerIndex < 0 || nLayerIndex > pFBXMesh->GetLayerCount() )
			return 0;

		FbxNode* pNode = pFBXMesh->GetNode();

		if( !pNode )
			return 0;

		FbxLayerElementMaterial* pFBXMaterial = pFBXMesh->GetLayer(nLayerIndex)->GetMaterials();

		if( pFBXMaterial )
		{
			int nMappingIndex = GetMappingIndex( pFBXMaterial->GetMappingMode(), nPolygonIndex, 0, nVertexIndex );

			if( nMappingIndex < 0 )
				return 0;

			FbxLayerElement::EReferenceMode referenceMode = pFBXMaterial->GetReferenceMode();


			if( referenceMode == FbxLayerElement::eDirect )
			{
				if( nMappingIndex < pNode->GetMaterialCount() )
				{
					return GetMaterial(pNode->GetMaterial(nMappingIndex));
				}
			}
			else if( referenceMode == FbxLayerElement::eIndexToDirect )
			{
				const FbxLayerElementArrayTemplate<int>& pMaterialIndexArray = pFBXMaterial->GetIndexArray();

				if( nMappingIndex < pMaterialIndexArray.GetCount() )
				{
					int nIndex = pMaterialIndexArray.GetAt(nMappingIndex);
					if( nIndex < pNode->GetMaterialCount() )
					{
						return GetMaterial(pNode->GetMaterial(nIndex));
					}
				}
			}
		}

		return 0;
	}

	Vector2 FbxConverter::GetTexCoord(FbxMesh* pFBXMesh, 
		int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		int nLayerCount = pFBXMesh->GetLayerCount();
		if( nLayerIndex < nLayerCount )//for( int i = 0; i < nLayerCount; ++i )
		{
			FbxLayer* pFBXLayer = pFBXMesh->GetLayer(nLayerIndex);

			if( pFBXLayer )
			{
				FbxLayerElementUV* pUVs = pFBXLayer->GetUVs(FbxLayerElement::eTextureDiffuse);
				if( pUVs )
				{
					FbxLayerElement::EMappingMode mappingMode = pUVs->GetMappingMode();
					FbxLayerElement::EReferenceMode referenceMode = pUVs->GetReferenceMode();

					int index = GetMappingIndex(mappingMode, nPolygonIndex, nPolygonVertexIndex, nVertexIndex);
					if (index<0)
						return Vector2::Zero;
					

					const FbxLayerElementArrayTemplate<FbxVector2>& pUVArray = pUVs->GetDirectArray();
					const FbxLayerElementArrayTemplate<int>& pUVIndexArray = pUVs->GetIndexArray();

					int nMappingIndex = index;
					switch(referenceMode)
					{
					case FbxLayerElement::eDirect:
						if( nMappingIndex < pUVArray.GetCount() )
						{
							return ConvertTexCoord( pUVArray.GetAt(nMappingIndex) ) ;
						}
						break;
					case FbxLayerElement::eIndexToDirect:
						if( nMappingIndex < pUVIndexArray.GetCount() )
						{
							int nIndex = pUVIndexArray.GetAt(nMappingIndex);
							if( nIndex < pUVArray.GetCount() )
							{
								return ConvertTexCoord( pUVArray.GetAt(nIndex) );
							}
						}
						break;
					};

					//switch(mappingMode)
					//{
					//case KFbxLayerElement::eBY_CONTROL_POINT:
					//	{
					//		int nMappingIndex = nVertexIndex;
					//		switch(referenceMode)
					//		{
					//		case KFbxLayerElement::eDIRECT:
					//			if( nMappingIndex < pUVArray.GetCount() )
					//			{
					//				return ConvertTexCoord( pUVArray.GetAt(nMappingIndex) ) ;
					//			}
					//			break;
					//		case KFbxLayerElement::eINDEX_TO_DIRECT:
					//			if( nMappingIndex < pUVIndexArray.GetCount() )
					//			{
					//				int nIndex = pUVIndexArray.GetAt(nMappingIndex);
					//				if( nIndex < pUVArray.GetCount() )
					//				{
					//					return ConvertTexCoord( pUVArray.GetAt(nIndex) );
					//				}
					//			}
					//			break;
					//		};
					//	}
					//	break;

					//case KFbxLayerElement::eBY_POLYGON_VERTEX:
					//	{
					//		int nMappingIndex = pFBXMesh->GetTextureUVIndex(nPolygonIndex, nPolygonVertexIndex, KFbxLayerElement::eDIFFUSE_TEXTURES);
					//		switch(referenceMode)
					//		{
					//		case KFbxLayerElement::eDIRECT:
					//		case KFbxLayerElement::eINDEX_TO_DIRECT: //I have no idea why the index array is not used in this case.
					//			if( nMappingIndex < pUVArray.GetCount() )
					//			{
					//				return ConvertTexCoord(  pUVArray.GetAt(nMappingIndex)  );
					//			}
					//			break;
					//		};
					//	}
					//	break;
					//};
				}
			}
		}
		return Vector2::Zero;
	}

	MaterialData* FbxConverter::GetMaterial(FbxSurfaceMaterial* pFBXMaterial)
	{
		int index = m_FBXMaterials.IndexOf(pFBXMaterial);
		if (index != -1)
			return m_materials[index];
		return 0;
	}
	Matrix FbxConverter::GetAbsoluteTransformFromCurrentTake(FbxNode* pNode, FbxTime time)
	{
		if (!pNode)
		{
			return Matrix::Identity;
		}
		
		Matrix a,b,c;
		a = ConvertMatrix(pNode->EvaluateGlobalTransform(time));
		b = GetGeometricOffset(pNode);

		Matrix::Multiply(c, b,a);
		return c;
	}
	Matrix FbxConverter::GetGeometricOffset(FbxNode* pNode)
	{
		if( !pNode )
		{
			return Matrix::Identity;
		}

		FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);// pNode->GetGeometricTranslation(FbxNode::eSOURCE_SET);
		FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);// pNode->GetGeometricRotation(FbxNode::eSOURCE_SET);
		FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);// pNode->GetGeometricScaling(FbxNode::eSOURCE_SET);

		FbxAMatrix matFBXGeometryOffset;
		matFBXGeometryOffset.SetIdentity();
		matFBXGeometryOffset.SetT(T);
		matFBXGeometryOffset.SetR(R);
		matFBXGeometryOffset.SetS(S);

		return ConvertMatrix(matFBXGeometryOffset);
	}

	bool FbxConverter::IsSkeletonRoot(FbxNode* pNode)
	{
		bool bNodeIsSkeleton = false;
		bool bParentNodeIsSkeleton = false;

		FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if( pNodeAttribute )	
			bNodeIsSkeleton = (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton);

		FbxNode* pParentNode = pNode->GetParent();
		if( pParentNode )
		{
			FbxNodeAttribute* pParentNodeAttribute = pParentNode->GetNodeAttribute();
			if( pParentNodeAttribute )
				bParentNodeIsSkeleton = (pParentNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton);
		}

		return (!bParentNodeIsSkeleton && bNodeIsSkeleton);
	}

	void FbxConverter::FlattenAnimation(AnimationData::ClipTable* clipTable)
	{
		const FIMesh** meshList = new const FIMesh*[m_meshes.getCount()];
		ExistTable<std::string> seenAnimation;
		int idxCounter = 0;
		for (HashMap<std::string, FIMesh*>::Enumerator i=m_meshes.GetEnumerator();i.MoveNext();)
		{
			const FIMesh* mesh = *i.getCurrentValue();
			for (HashMap<std::string, FIPartialAnimation*>::Enumerator j=mesh->m_AnimationKeyFrames.GetEnumerator();j.MoveNext();)
			{
				FIPartialAnimation* anim = *j.getCurrentValue();

				const std::string& name = anim->GetAnimationName();

				if (!seenAnimation.Exists(name))
				{
					seenAnimation.Add(name);
				}
			}
			meshList[idxCounter++] = mesh;
		}

		for (ExistTable<std::string>::Enumerator e = seenAnimation.GetEnumerator();e.MoveNext();)
		{
			const std::string& animName = *e.getCurrent();

			uint frameIndex = 0;
			bool finished = false;
			FastList<ModelKeyframe> frames;

			while (!finished)
			{
				finished = true;
				for (int i=0;i<m_meshes.getCount();i++)
				{
					const FIMesh* mesh = meshList[i];
					FIPartialAnimation* anim = mesh->GetAnimationKeyFrames(animName);

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


		delete[] meshList;
	}

	Vector3 FbxConverter::GetMaterialColor(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<double> FBXFactorProperty)
	{
		Vector3 Color;
		
		if( FBXColorProperty.IsValid() )
		{
			FbxDouble3 FBXColor = FBXColorProperty.Get();
			Color = Vector3( 
				static_cast<float>(FBXColor[0]),
				static_cast<float>(FBXColor[1]),
				static_cast<float>(FBXColor[2]) );

			if( FBXFactorProperty.IsValid() )
			{
				double FBXFactor = FBXFactorProperty.Get();
				Color = Vector3::Multiply(Color, static_cast<float>(FBXFactor));
			}
		}
		return Color;
	}
	//
	struct FIVertex
	{
		float Position[3];
		float Normal[3];
		float TexCoords[4];
		float BlendIndex[4];
		float BlendWeight[4];

		FIVertex(){}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1)
		{
			Position[0] = pos.X;
			Position[1] = pos.Y;
			Position[2] = pos.Z;

			Normal[0] = n.X; Normal[1] = n.Y; Normal[2] = n.Z;

			TexCoords[0] = t1.X;
			TexCoords[1] = t1.Y;
			TexCoords[2] = TexCoords[3] = 0;

			BlendIndex[0] = BlendIndex[1] = BlendIndex[2] = BlendIndex[3] = 0;
			BlendWeight[0] = BlendWeight[1] = BlendWeight[2] = BlendWeight[3] = 0;
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector2 t2)
		{
			Position[0] = pos.X;
			Position[1] = pos.Y;
			Position[2] = pos.Z;

			Normal[0] = n.X; Normal[1] = n.Y; Normal[2] = n.Z;

			TexCoords[0] = t1.X;
			TexCoords[1] = t1.Y;
			TexCoords[2] = t2.X;
			TexCoords[3] = t2.Y;


			BlendIndex[0] = BlendIndex[1] = BlendIndex[2] = BlendIndex[3] = 0;
			BlendWeight[0] = BlendWeight[1] = BlendWeight[2] = BlendWeight[3] = 0;
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector2 t2, Vector4 bi, Vector4 bw)
		{
			Position[0] = pos.X;
			Position[1] = pos.Y;
			Position[2] = pos.Z;

			Normal[0] = n.X; Normal[1] = n.Y; Normal[2] = n.Z;

			TexCoords[0] = t1.X;
			TexCoords[1] = t1.Y;
			TexCoords[2] = t2.X;
			TexCoords[3] = t2.Y;

			BlendIndex[0] = bi.X;
			BlendIndex[1] = bi.Y;
			BlendIndex[2] = bi.Z;
			BlendIndex[3] = bi.W;

			BlendWeight[0] = bw.X;
			BlendWeight[1] = bw.Y;
			BlendWeight[2] = bw.Z;
			BlendWeight[3] = bw.W;
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector4 bi, Vector4 bw)
		{
			Position[0] = pos.X;
			Position[1] = pos.Y;
			Position[2] = pos.Z;

			Normal[0] = n.X; Normal[1] = n.Y; Normal[2] = n.Z;

			TexCoords[0] = t1.X;
			TexCoords[1] = t1.Y;
			TexCoords[2] = TexCoords[3] = 0;

			BlendIndex[0] = bi.X;
			BlendIndex[1] = bi.Y;
			BlendIndex[2] = bi.Z;
			BlendIndex[3] = bi.W;

			BlendWeight[0] = bw.X;
			BlendWeight[1] = bw.Y;
			BlendWeight[2] = bw.Z;
			BlendWeight[3] = bw.W;
		}
	};

	class FIVertexEqualityComparer : public IEqualityComparer<FIVertex>
	{
	public:
		virtual bool Equals(const FIVertex& x, const FIVertex& y) const
		{
			if (x.Position[0] != y.Position[0])
				return false;
			if (x.Position[1] != y.Position[1])
				return false;
			if (x.Position[2] != y.Position[2])
				return false;

			if (x.Normal[0] != y.Normal[0])
				return false;
			if (x.Normal[1] != y.Normal[1])
				return false;
			if (x.Normal[2] != y.Normal[2])
				return false;

			if (x.TexCoords[0] != y.TexCoords[0])
				return false;
			if (x.TexCoords[1] != y.TexCoords[1])
				return false;
			if (x.TexCoords[2] != y.TexCoords[2])
				return false;
			if (x.TexCoords[3] != y.TexCoords[3])
				return false;


			if (x.BlendIndex[0] != y.BlendIndex[0])
				return false;
			if (x.BlendIndex[1] != y.BlendIndex[1])
				return false;
			if (x.BlendIndex[2] != y.BlendIndex[2])
				return false;
			if (x.BlendIndex[3] != y.BlendIndex[3])
				return false;

			if (x.BlendWeight[0] != y.BlendWeight[0])
				return false;
			if (x.BlendWeight[1] != y.BlendWeight[1])
				return false;
			if (x.BlendWeight[2] != y.BlendWeight[2])
				return false;
			if (x.BlendWeight[3] != y.BlendWeight[3])
				return false;

			return true;
		}

		virtual int64 GetHashCode(const FIVertex& obj) const
		{
			FNVHash32 hasher;

			hasher.Accumulate(&obj, sizeof(FIVertex));

			return hasher.GetResult();
		}
	};

	void FbxConverter::Import(const MeshBuildConfig& config)
	{
		ModelData modelData;
		AnimationData animData;

		FbxConverter fbx;
		fbx.m_bakeTransform = config.DstAnimationFile.empty();
		fbx.Initialize(config.SrcFile);
		
		// mesh
		{
			for (HashMap<std::string, FIMesh*>::Enumerator i=fbx.m_meshes.GetEnumerator();i.MoveNext();)
			{
				FIMesh* mesh = *i.getCurrentValue();
				const std::vector<FIMeshPart*>& parts = mesh->getParts();
				FastList<MaterialData*> materialData;
				// materials
				{
					bool* useTable = new bool[fbx.m_materials.getCount()];
					memset(useTable, 0, sizeof(bool)*fbx.m_materials.getCount());
					
					for (size_t j=0;j<parts.size();j++)
					{
						int mtrlIndex = fbx.m_materials.IndexOf(parts[j]->GetMaterial());
						assert(mtrlIndex>=0);
						useTable[mtrlIndex] = true;
					}
					for (int j=0;j<fbx.m_materials.getCount();j++)
					{
						if (useTable[j])
							materialData.Add(fbx.m_materials[j]);
					}

					delete[] useTable;
				}

				// should not merge mesh part here
				int totalVertexCount=0;
				bool use2TexCoords = false;
				bool useSkinnedFormat = false;
				for (size_t j=0;j<parts.size();j++)
				{
					FIMeshPart* part= parts[j];
					use2TexCoords |= !!part->getTexCoord1().size();
					totalVertexCount+=(int32)part->getPosition().size();
					useSkinnedFormat |= part->IsSkinnedModel();
				}
				
				

				MeshData* meshData = new MeshData();
				// add materials
				{
					for (int j=0;j<materialData.getCount();j++)
					{
						MaterialData* mtrlData = materialData[j];
						mtrlData->Cull = CULL_Clockwise;
						meshData->Materials.Add(mtrlData);
					}
				}
				meshData->Name = StringUtils::toPlatformWideString(mesh->GetName());
				meshData->BoundingSphere.Center = Vector3::Zero;
				meshData->BoundingSphere.Radius = 0;

				// duplicated vertex removal using hashtable
				// build face data at the same time
				FIVertexEqualityComparer comparer;
				HashMap<FIVertex, int> vtxHashTable(totalVertexCount, &comparer);
				FastList<FIVertex> vertexList(totalVertexCount);
				meshData->Faces.ResizeDiscard(totalVertexCount/3+2);

				// vertex hashing & add to list
				for (size_t j=0;j<parts.size();j++)
				{
					FIMeshPart* part= parts[j];
					const std::vector<Vector3>& Positions = part->getPosition();
					const std::vector<Vector3>& Normals = part->getNormal();
					const std::vector<Vector2>& TexCoord0 = part->getTexCoord0();
					const std::vector<Vector2>& TexCoord1 = part->getTexCoord1();
					const std::vector<BoneWeight>& BoneWeights = part->getBoneWeights();
					
					int mtrlIndex = materialData.IndexOf( part->GetMaterial());

					for (size_t k=0;k<Positions.size();k+=3)
					{
						FIVertex fiv;
						int vi = k;
						if (use2TexCoords)
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi]);
							}
						}
						else
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi]);
							}
						}

						int existA;
						if (!vtxHashTable.TryGetValue(fiv, existA))
						{
							existA = vertexList.getCount();
							vertexList.Add(fiv);
							vtxHashTable.Add(fiv, existA);
							meshData->BoundingSphere.Center = 
								Vector3::Add(meshData->BoundingSphere.Center, Positions[k]);
						}

						//============================================================================
						vi++;
						if (use2TexCoords)
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi]);
							}
						}
						else
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi]);
							}
						}

						int existB;
						if (!vtxHashTable.TryGetValue(fiv, existB))
						{
							existB = vertexList.getCount();
							vertexList.Add(fiv);
							vtxHashTable.Add(fiv, existB);
							meshData->BoundingSphere.Center = 
								Vector3::Add(meshData->BoundingSphere.Center, Positions[k+1]);
						}

						//============================================================================

						vi++;
						if (use2TexCoords)
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], TexCoord1[vi]);
							}
						}
						else
						{
							if (useSkinnedFormat)
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi], 
									BoneWeights[vi].GetBlendIndex(), BoneWeights[vi].GetBlendWeight());
							}
							else
							{
								fiv = FIVertex(Positions[vi], Normals[vi], 
									TexCoord0[vi]);
							}
						}


						int existC;
						if (!vtxHashTable.TryGetValue(fiv, existC))
						{
							existC = vertexList.getCount();
							vertexList.Add(fiv);
							vtxHashTable.Add(fiv, existC);
							meshData->BoundingSphere.Center = 
								Vector3::Add(meshData->BoundingSphere.Center, Positions[k+2]);
						}
						meshData->Faces.Add(MeshFace(existA,existB,existC,mtrlIndex));
					}
				}
				meshData->BoundingSphere.Center =
					Vector3::Divide(meshData->BoundingSphere.Center, static_cast<float>(vertexList.getCount()));
				
				meshData->VertexCount = vertexList.getCount();

				FastList<VertexElement>& vtxElem = meshData->VertexElements;
				// fill vertex element
				vtxElem.Add(VertexElement(0, VEF_Vector3, VEU_Position, 0));
				
				if (useSkinnedFormat)
				{
					vtxElem.Add(VertexElement(
						vtxElem[vtxElem.getCount()-1].getSize() + vtxElem[vtxElem.getCount()-1].getOffset(), 
						VEF_Vector4, VEU_BlendIndices, 0));
					vtxElem.Add(VertexElement(
						vtxElem[vtxElem.getCount()-1].getSize() + vtxElem[vtxElem.getCount()-1].getOffset(), 
						VEF_Vector4, VEU_BlendWeight, 0));
				}
				vtxElem.Add(VertexElement(
					vtxElem[vtxElem.getCount()-1].getSize() + vtxElem[vtxElem.getCount()-1].getOffset(), 
					VEF_Vector3, VEU_Normal, 0));
				

				vtxElem.Add(VertexElement(
					vtxElem[vtxElem.getCount()-1].getSize() + vtxElem[vtxElem.getCount()-1].getOffset(), 
					VEF_Vector2, VEU_TextureCoordinate, 0));
				if (use2TexCoords)
					vtxElem.Add(VertexElement(
					vtxElem[vtxElem.getCount()-1].getSize() + vtxElem[vtxElem.getCount()-1].getOffset(), 
					VEF_Vector2, VEU_TextureCoordinate, 1));
				
				meshData->VertexSize = meshData->ComputeVertexSize(vtxElem);

				// make the vertex to the final position in mesh when needed
				if (fbx.m_bakeTransform)
				{
					Matrix absTransform = mesh->GetAbsoluteTransform();
					
					Matrix postTransform0;
					Matrix::CreateRotationX(postTransform0, -Math::Half_PI);

					Matrix postTransform1;
					Matrix::CreateRotationY(postTransform1, Math::PI);

					Matrix postTransfrom;
					Matrix::Multiply(postTransfrom, postTransform0, postTransform1);


					for (int j=0;j<vertexList.getCount();j++)
					{
						for (int k=0;k<vtxElem.getCount();k++)
						{
							const VertexElement& e = vtxElem[k];
							switch (e.getUsage())
							{
							case VEU_Position:
								{
									Vector3 p(vertexList[j].Position);
									p = Vector3::TransformCoordinate(p, absTransform);

									p.Z = -p.Z;

									p = Vector3::TransformCoordinate(p, postTransfrom);

									p.Store(vertexList[j].Position);
									break;
								}
							case VEU_Normal:
								{
									Vector3 p(vertexList[j].Normal);
									p = Vector3::TransformNormal(p, absTransform);

									p.X = -p.X;
									p.Y = -p.Y;

									p = Vector3::TransformNormal(p, postTransfrom);

									p.Store(vertexList[j].Normal);
									break;
								}
							}
						}
					}
				}


				// Fill vertex data. foreach vertex, pass the vertex elements array, 
				// fill data according to the element definition,
				meshData->VertexData = new char[meshData->VertexSize*meshData->VertexCount];
				for (int j=0;j<(int)meshData->VertexCount;j++)
				{
					int baseOffset = j * meshData->VertexSize;
					for (int k=0;k<vtxElem.getCount();k++)
					{
						const VertexElement& e = vtxElem[k];
						float* vertexPtr = (float*)(meshData->VertexData + baseOffset+e.getOffset());
						switch (e.getUsage())
						{
						case VEU_Position:
							*(vertexPtr) = vertexList[j].Position[0];
							*(vertexPtr+1) = vertexList[j].Position[1];
							*(vertexPtr+2) = vertexList[j].Position[2];

							{
								Vector3 pos(
									vertexList[j].Position[0], 
									vertexList[j].Position[1], 
									vertexList[j].Position[2]);

								float rr = Vector3::DistanceSquared(meshData->BoundingSphere.Center, pos);
								if (rr > meshData->BoundingSphere.Radius)
								{
									meshData->BoundingSphere.Radius = rr;
								}
							}
							
							break;
						case VEU_Normal:
							*(vertexPtr) = vertexList[j].Normal[0];
							*(vertexPtr+1) = vertexList[j].Normal[1];
							*(vertexPtr+2) = vertexList[j].Normal[2];
							break;
						case VEU_TextureCoordinate:
							if (e.getIndex() == 0)
							{
								*(vertexPtr) = vertexList[j].TexCoords[0];
								*(vertexPtr+1) = vertexList[j].TexCoords[1];
							}
							else
							{
								*(vertexPtr) = vertexList[j].TexCoords[2];
								*(vertexPtr+1) = vertexList[j].TexCoords[3];
							}
							break;
						case VEU_BlendIndices:
							*(vertexPtr) = vertexList[j].BlendIndex[0];
							*(vertexPtr+1) = vertexList[j].BlendIndex[1];
							*(vertexPtr+2) = vertexList[j].BlendIndex[2];
							*(vertexPtr+3) = vertexList[j].BlendIndex[2];
							break;
						case VEU_BlendWeight:
							*(vertexPtr) = vertexList[j].BlendWeight[0];
							*(vertexPtr+1) = vertexList[j].BlendWeight[1];
							*(vertexPtr+2) = vertexList[j].BlendWeight[2];
							*(vertexPtr+3) = vertexList[j].BlendWeight[2];
							break;
						}
					}
				}
				meshData->BoundingSphere.Radius = sqrtf(meshData->BoundingSphere.Radius);

				modelData.Entities.Add(meshData);
			}
		}
		MeshBuild::ConvertVertexData(&modelData, config);
		MeshBuild::CollapseMeshs(&modelData, config);

		// animation
		if (!config.DstAnimationFile.empty())
		{
			AnimationData::ClipTable skeletonAnimations;
			AnimationData::ClipTable rigidAnimations;

			List<Bone> bones;
			if (fbx.m_pSkeleton)
			{
				fbx.m_pSkeleton->FlattenBones(bones);
				fbx.m_pSkeleton->FlattenAnimation(&skeletonAnimations);
			}

			fbx.FlattenAnimation(&rigidAnimations);
			
			animData.setBones(bones);
			animData.setSkinnedAnimationClips(skeletonAnimations);
			animData.setRigidAnimationClips(rigidAnimations);
			animData.RigidEntityCount = modelData.Entities.getCount();
		}


		FileOutStream* fs = new FileOutStream(config.DstFile);
		modelData.Save(fs);

		if (!config.DstAnimationFile.empty())
		{
			fs = new FileOutStream(config.DstAnimationFile);
			animData.Save(fs);
		}
	}
}