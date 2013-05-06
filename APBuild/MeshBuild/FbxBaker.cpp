
#include "FbxBaker.h"

void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene)
{
	// The first thing to do is to create the FBX SDK manager which is the 
	// object allocator for almost all the classes in the SDK.
	pSdkManager = FbxManager::Create();

	if (!pSdkManager)
	{
		printf("Unable to create the FBX SDK manager\n");
		exit(0);
	}

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(pSdkManager, IOSROOT );
	pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	FbxString lPath = FbxGetApplicationDirectory();

	pSdkManager->LoadPluginsDirectory(lPath.Buffer());

	// Create the entity that will hold the scene.
	pScene = FbxScene::Create(pSdkManager,"");
}

void DestroySdkObjects(FbxManager* pSdkManager)
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}
