#pragma once
#include "../APBCommon.h"

#include <fbxsdk.h>


namespace APBuild
{
	class FbxBacker
	{
	private:
		FbxManager * mSdkManager;
		FbxScene * mScene;
		FbxImporter * mImporter;
		FbxAnimLayer * mCurrentAnimLayer;
		FbxNode * mSelectedNode;
	public:

	};
}
