#include "FbxImporter.h"

#include "CompileLog.h"
#include "BuildConfig.h"
#include "Utility/StringUtils.h"
#include "IOLib/Streams.h"
#include "IOLib/MaterialData.h"
#include "IOLib/ModelData.h"

using namespace Apoc3D::Utility;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(m_pFBXSdkManager->GetIOSettings()))
#endif

namespace APBuild
{

	bool HasFBXAnimation(KFbxNode *pNode)
	{
		//pNode->GetAnimationInterval();
		bool hasKeys = false;

		KFbxProperty prop = pNode->GetFirstProperty();

		while(prop.IsValid())
		{
			KFbxAnimCurveNode* cn = prop.GetCurveNode();
			if (cn)
			{
				KFCurveNode *fcn = cn->GetKFCurveNode();
				if(fcn && fcn->KeyGetCount() > 0)
				{
					hasKeys = true;
				}
			}
			
			prop = pNode->GetNextProperty(prop);
		}

		return hasKeys;
	}

	Vector3 ConvertVector3(const KFbxVector4& v)
	{
		return Vector3Utils::LDVector( 
			static_cast<float>(v.GetAt(0)),
			static_cast<float>(v.GetAt(1)),
			static_cast<float>(v.GetAt(2)) );
	}
	Vector2 ConvertVector2(const KFbxVector2& v)
	{
		return Vector2Utils::LDVector(
			static_cast<float>(v.GetAt(0)),
			static_cast<float>(v.GetAt(1)) );
	}
	Vector2 ConvertTexCoord(const KFbxVector2& v)
	{
		return Vector2Utils::LDVector(
			static_cast<float>(v.GetAt(0)), 
			static_cast<float>(1-v.GetAt(1)) );
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
#pragma error "FBX SDK does not not support this platform."
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
		//int i, lAnimStackCount;
		bool lStatus;
		//char lPassword[256];

		string mbFilename = StringUtils::toString(pFilename);

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
			CompileLog::WriteError(L"Unable to initialize FBX", pFilename);
			CompileLog::WriteError(StringUtils::toWString(lImporter->GetLastErrorString()), pFilename);

			if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
			{
				CompileLog::WriteError(
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
			CompileLog::WriteError(L"Cannot import files with password.", pFilename);

			return false;
		}

		KFbxAxisSystem currentAxisSystem = m_pFBXScene->GetGlobalSettings().GetAxisSystem();

		KFbxAxisSystem axisSystem = KFbxAxisSystem(KFbxAxisSystem::YAxis, 
			KFbxAxisSystem::ParityOdd, 
			KFbxAxisSystem::RightHanded);

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
					pMaterial->TextureName[i] = StringUtils::toWString( strFileName );
				}
			}
			
			KFbxSurfaceLambert* pLambert = KFbxCast<KFbxSurfaceLambert>(pFBXMaterial);
			KFbxSurfacePhong* pPhong = KFbxCast<KFbxSurfacePhong>(pFBXMaterial);

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

				KFbxPropertyDouble1 FBXTransparencyProperty = pLambert->TransparencyFactor;
				if( FBXTransparencyProperty.IsValid() )
					fTransparency = static_cast<float>( FBXTransparencyProperty.Get() );
			}
			if( pPhong )
			{
				SpecularColor = GetMaterialColor(pPhong->Specular, pPhong->SpecularFactor);
				
				KFbxPropertyDouble1 FBXSpecularPowerProperty = pPhong->Shininess;
				if( FBXSpecularPowerProperty.IsValid() )
					fSpecularPower = static_cast<float>( FBXSpecularPowerProperty.Get() );
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
					FISkeletonBone* pBone = m_pSkeleton->FindBone(pNode->GetName());

					if( pBone )
					{

						FIPartialAnimation* pAnimationKeyFrames = new FIPartialAnimation(strTakeName);

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

							pAnimationKeyFrames->AddKeyFrame(FIAnimationKeyframe(matTransform, static_cast<float>(fTime)));

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
					FIPartialAnimation* pAnimationKeyFrames = new FIPartialAnimation(strTakeName);

					double fTime = 0;
					while( fTime <= fStop )
					{
						KTime takeTime;
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

				float fStart = static_cast<float>(KStart.GetSecondDouble());
				float fStop = static_cast<float>(KStop.GetSecondDouble());

				if( fStart < fStop )
				{
					int nKeyFrames = static_cast<int>((fStop-fStart)*fFrameRate);

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
			m_pSkeleton = new FISkeleton();
		}

		int nParentBoneIndex = -1;
		KFbxNode* pParentNode = pNode->GetParent();
		if( pParentNode )
			nParentBoneIndex = m_pSkeleton->FindBoneIndex(pParentNode->GetName());

		FISkeletonBone* pSkeletonBone = new FISkeletonBone( pNode->GetName(), nParentBoneIndex );
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
					boneWeights[nVertexIndex]
				);
			}

		}
		mesh->FinishAndOptimize();
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

			FISkeletonBone* pSkeletonBone = m_pSkeleton->GetSkeletonBone(nBoneIndex);

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
	MaterialData* FbxImporter::GetMaterialLinkedWithPolygon(KFbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		if( nLayerIndex < 0 || nLayerIndex > pFBXMesh->GetLayerCount() )
			return 0;

		KFbxNode* pNode = pFBXMesh->GetNode();

		if( !pNode )
			return 0;

		KFbxLayerElementMaterial* pFBXMaterial = pFBXMesh->GetLayer(nLayerIndex)->GetMaterials();

		if( pFBXMaterial )
		{
			int nMappingIndex = GetMappingIndex( pFBXMaterial->GetMappingMode(), nPolygonIndex, 0, nVertexIndex );

			if( nMappingIndex < 0 )
				return 0;

			KFbxLayerElement::EReferenceMode referenceMode = pFBXMaterial->GetReferenceMode();


			if( referenceMode == KFbxLayerElement::eDIRECT )
			{
				if( nMappingIndex < pNode->GetMaterialCount() )
				{
					return GetMaterial(pNode->GetMaterial(nMappingIndex));
				}
			}
			else if( referenceMode == KFbxLayerElement::eINDEX_TO_DIRECT )
			{
				const KFbxLayerElementArrayTemplate<int>& pMaterialIndexArray = pFBXMaterial->GetIndexArray();

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

	Vector2 FbxImporter::GetTexCoord(KFbxMesh* pFBXMesh, 
		int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
	{
		int nLayerCount = pFBXMesh->GetLayerCount();
		if( nLayerIndex < nLayerCount )//for( int i = 0; i < nLayerCount; ++i )
		{
			KFbxLayer* pFBXLayer = pFBXMesh->GetLayer(nLayerIndex);

			if( pFBXLayer )
			{
				KFbxLayerElementUV* pUVs = pFBXLayer->GetUVs(KFbxLayerElement::eDIFFUSE_TEXTURES);
				if( pUVs )
				{
					KFbxLayerElement::EMappingMode mappingMode = pUVs->GetMappingMode();
					KFbxLayerElement::EReferenceMode referenceMode = pUVs->GetReferenceMode();

					const KFbxLayerElementArrayTemplate<KFbxVector2>& pUVArray = pUVs->GetDirectArray();
					const KFbxLayerElementArrayTemplate<int>& pUVIndexArray = pUVs->GetIndexArray();

					switch(mappingMode)
					{
					case KFbxLayerElement::eBY_CONTROL_POINT:
						{
							int nMappingIndex = nVertexIndex;
							switch(referenceMode)
							{
							case KFbxLayerElement::eDIRECT:
								if( nMappingIndex < pUVArray.GetCount() )
								{
									return ConvertTexCoord( pUVArray.GetAt(nMappingIndex) ) ;
								}
								break;
							case KFbxLayerElement::eINDEX_TO_DIRECT:
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
						}
						break;

					case KFbxLayerElement::eBY_POLYGON_VERTEX:
						{
							int nMappingIndex = pFBXMesh->GetTextureUVIndex(nPolygonIndex, nPolygonVertexIndex, KFbxLayerElement::eDIFFUSE_TEXTURES);
							switch(referenceMode)
							{
							case KFbxLayerElement::eDIRECT:
							case KFbxLayerElement::eINDEX_TO_DIRECT: //I have no idea why the index array is not used in this case.
								if( nMappingIndex < pUVArray.GetCount() )
								{
									return ConvertTexCoord(  pUVArray.GetAt(nMappingIndex)  );
								}
								break;
							};
						}
						break;
					};
				}
			}
		}
		return Vector2Utils::LDVector(0);
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
			Color = Vector3Utils::LDVector( 
				static_cast<float>(FBXColor[0]),
				static_cast<float>(FBXColor[1]),
				static_cast<float>(FBXColor[2]) );

			if( FBXFactorProperty.IsValid() )
			{
				double FBXFactor = FBXFactorProperty.Get();
				Color = Vector3Utils::Multiply(Color, static_cast<float>(FBXFactor));
			}
		}
		return Color;
	}

	FbxImporter::~FbxImporter()
	{
		for (int i=0;i<m_materials.getCount();i++)
		{
			delete m_materials[i];
		}
		for (FastMap<string, FIMesh*>::Enumerator i=m_meshes.GetEnumerator();i.MoveNext();)
		{
			delete *i.getCurrentValue();
		}
		delete m_pSkeleton;
	}

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
			Position[0] = _V3X(pos);
			Position[1] = _V3Y(pos);
			Position[2] = _V3Z(pos);

			Normal[0] = _V3X(n); Normal[1] = _V3Y(n); Normal[2] = _V3Z(n);

			TexCoords[0] = Vector2Utils::GetX(t1);
			TexCoords[1] = Vector2Utils::GetY(t1);
			TexCoords[2] = TexCoords[3] = 0;

			BlendIndex[0] = BlendIndex[1] = BlendIndex[2] = BlendIndex[3] = 0;
			BlendWeight[0] = BlendWeight[1] = BlendWeight[2] = BlendWeight[3] = 0;
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector2 t2)
		{
			Position[0] = _V3X(pos);
			Position[1] = _V3Y(pos);
			Position[2] = _V3Z(pos);

			Normal[0] = _V3X(n); Normal[1] = _V3Y(n); Normal[2] = _V3Z(n);

			TexCoords[0] = Vector2Utils::GetX(t1);
			TexCoords[1] = Vector2Utils::GetY(t1);
			TexCoords[2] = Vector2Utils::GetX(t2);
			TexCoords[3] = Vector2Utils::GetY(t2);


			BlendIndex[0] = BlendIndex[1] = BlendIndex[2] = BlendIndex[3] = 0;
			BlendWeight[0] = BlendWeight[1] = BlendWeight[2] = BlendWeight[3] = 0;
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector2 t2, Vector4 bi, Vector4 bw)
		{
			Position[0] = _V3X(pos);
			Position[1] = _V3Y(pos);
			Position[2] = _V3Z(pos);

			Normal[0] = _V3X(n); Normal[1] = _V3Y(n); Normal[2] = _V3Z(n);

			TexCoords[0] = Vector2Utils::GetX(t1);
			TexCoords[1] = Vector2Utils::GetY(t1);
			TexCoords[2] = Vector2Utils::GetX(t2);
			TexCoords[3] = Vector2Utils::GetY(t2);

			BlendIndex[0] = Vector4Utils::GetX(bi);
			BlendIndex[1] = Vector4Utils::GetY(bi);
			BlendIndex[2] = Vector4Utils::GetZ(bi);
			BlendIndex[3] = Vector4Utils::GetW(bi);


			BlendWeight[0] = Vector4Utils::GetX(bw);
			BlendWeight[1] = Vector4Utils::GetY(bw);
			BlendWeight[2] = Vector4Utils::GetZ(bw);
			BlendWeight[3] = Vector4Utils::GetW(bw);
		}
		FIVertex(Vector3 pos, Vector3 n, Vector2 t1, Vector4 bi, Vector4 bw)
		{
			Position[0] = _V3X(pos);
			Position[1] = _V3Y(pos);
			Position[2] = _V3Z(pos);

			Normal[0] = _V3X(n); Normal[1] = _V3Y(n); Normal[2] = _V3Z(n);

			TexCoords[0] = Vector2Utils::GetX(t1);
			TexCoords[1] = Vector2Utils::GetY(t1);
			TexCoords[2] = TexCoords[3] = 0;

			BlendIndex[0] = Vector4Utils::GetX(bi);
			BlendIndex[1] = Vector4Utils::GetY(bi);
			BlendIndex[2] = Vector4Utils::GetZ(bi);
			BlendIndex[3] = Vector4Utils::GetW(bi);

			BlendWeight[0] = Vector4Utils::GetX(bw);
			BlendWeight[1] = Vector4Utils::GetY(bw);
			BlendWeight[2] = Vector4Utils::GetZ(bw);
			BlendWeight[3] = Vector4Utils::GetW(bw);
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
			const float* chPtr = reinterpret_cast<const float*>(&obj);
			uint even = 0x15051505;
			uint odd = even;
			const uint* numPtr = reinterpret_cast<const uint*>(chPtr);
			for (int i = sizeof(FIVertex); i > 0; i -= 8)
			{
				even = ((even << 5) + even + (even >> 0x1b)) ^ numPtr[0];
				if (i <= 7)
				{
					break;
				}
				odd = ((odd << 5) + odd + (odd >> 0x1b)) ^ numPtr[1];
				numPtr += (sizeof(wchar_t) * 4) / sizeof(uint);
			}
			return even + odd * 0x5d588b65;
		}
	};

	void FbxImporter::Import(const MeshBuildConfig& config)
	{
		ModelData modelData;
		AnimationData animData;

		FbxImporter fbx;
		fbx.Initialize(config.SrcFile);
		
		// mesh
		{
			for (FastMap<string, FIMesh*>::Enumerator i=fbx.m_meshes.GetEnumerator();i.MoveNext();)
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
						meshData->Materials.Add(*materialData[j]);
					}
				}
				meshData->Name = StringUtils::toWString(mesh->GetName());
				meshData->BoundingSphere.Center = Vector3Utils::Zero;
				meshData->BoundingSphere.Radius = 0;

				// duplicated vertex removal using hashtable
				// build face data at the same time
				FIVertexEqualityComparer comparer;
				FastMap<FIVertex, int> vtxHashTable(totalVertexCount, &comparer);
				FastList<FIVertex> vertexList(totalVertexCount);
				meshData->Faces.ResizeDiscard(totalVertexCount/3+2);
				
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
								Vector3Utils::Add(meshData->BoundingSphere.Center, Positions[k]);
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
								Vector3Utils::Add(meshData->BoundingSphere.Center, Positions[k+1]);
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
								Vector3Utils::Add(meshData->BoundingSphere.Center, Positions[k+2]);
						}
						meshData->Faces.Add(MeshFace(existA,existB,existC,mtrlIndex));
					}
				}
				meshData->BoundingSphere.Center =
					Vector3Utils::Divide(meshData->BoundingSphere.Center, static_cast<float>(vertexList.getCount()));
				
				meshData->VertexCount = vertexList.getCount();

				// fill vertex element
				meshData->VertexElements.Add(VertexElement(0, VEF_Vector3, VEU_Position, 0));
				meshData->VertexElements.Add(VertexElement(
					meshData->VertexElements[meshData->VertexElements.getCount()-1].getSize(), VEF_Vector3, VEU_Normal, 0));
				meshData->VertexElements.Add(VertexElement(
					meshData->VertexElements[meshData->VertexElements.getCount()-1].getSize(), VEF_Vector2, VEU_TextureCoordinate, 0));
				if (use2TexCoords)
					meshData->VertexElements.Add(VertexElement(
					meshData->VertexElements[meshData->VertexElements.getCount()-1].getSize(), VEF_Vector2, VEU_TextureCoordinate, 1));
				if (useSkinnedFormat)
				{
					meshData->VertexElements.Add(VertexElement(
						meshData->VertexElements[meshData->VertexElements.getCount()-1].getSize(), VEF_Vector4, VEU_BlendIndices, 0));
					meshData->VertexElements.Add(VertexElement(
						meshData->VertexElements[meshData->VertexElements.getCount()-1].getSize(), VEF_Vector4, VEU_BlendWeight, 0));
				}
				meshData->VertexSize = meshData->ComputeVertexSize(meshData->VertexElements);

				// Fill vertex data. foreach vertex, pass the vertex elements array, 
				// fill data according to the element definition,
				meshData->VertexData = new char[meshData->VertexSize*meshData->VertexCount];
				for (int j=0;j<(int)meshData->VertexCount;j++)
				{
					int baseOffset = j * meshData->VertexSize;
					for (int k=0;k<meshData->VertexElements.getCount();k++)
					{
						const VertexElement& e = meshData->VertexElements[k];
						float* vertexPtr = (float*)(meshData->VertexData + baseOffset+e.getOffset());
						switch (e.getUsage())
						{
						case VEU_Position:
							*(vertexPtr) = vertexList[j].Position[0];
							*(vertexPtr+1) = vertexList[j].Position[1];
							*(vertexPtr+2) = vertexList[j].Position[2];

							{
								Vector3 pos = Vector3Utils::LDVector(
									vertexList[j].Position[0], 
									vertexList[j].Position[1], 
									vertexList[j].Position[2]);

								float rr = Vector3Utils::DistanceSquared(meshData->BoundingSphere.Center, pos);
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

		
		// animation
		if (!config.DstAnimationFile.empty())
		{
			AnimationData::ClipTable skeletonAnimations;
			AnimationData::ClipTable rigidAnimations;

			FastList<Bone> bones;
			if (fbx.m_pSkeleton)
			{
				fbx.m_pSkeleton->FlattenBones(bones);
				fbx.m_pSkeleton->FlattenAnimation(&skeletonAnimations);
			}

			fbx.FlattenAnimation(&rigidAnimations);
			
			animData.setBones(bones);
			animData.setSkinnedAnimationClips(skeletonAnimations);
			animData.setRigidAnimationClips(rigidAnimations);
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