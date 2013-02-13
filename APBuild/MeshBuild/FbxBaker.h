#ifndef FBXBAKER_H
#define FBXBAKER_H

#include "../APBCommon.h"

#include <fbxsdk.h>


namespace APBuild
{
	class FbxBacker
	{
	private:
		KFbxSdkManager * mSdkManager;
		KFbxScene * mScene;
		KFbxImporter * mImporter;
		KFbxAnimLayer * mCurrentAnimLayer;
		KFbxNode * mSelectedNode;
	public:

	};
}

#endif