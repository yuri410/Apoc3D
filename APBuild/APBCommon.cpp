#include "APBCommon.h"

#pragma comment(lib, "Apoc3D.lib")

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#ifdef APOC3D_MT
#pragma comment(lib, "assimp_mt.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")

#else
#pragma comment(lib, "assimp.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

#endif

#ifdef _DEBUG
#	ifdef APOC3D_MT
#		pragma comment(lib, "OIS_static_d_mt.lib")
#	else
#		pragma comment(lib, "OIS_static_d.lib")
#	endif
#else
#	ifdef APOC3D_MT
#		pragma comment(lib, "OIS_static_mt.lib")
#	else
#		pragma comment(lib, "OIS_static.lib")
#	endif
#endif