
#pragma once

typedef unsigned int uint;
typedef unsigned short ushort;

#include "Include\d3d9.h"
#include "Include\d3dx9.h"

#include <windows.h>

#define _Export __declspec( dllexport )


// Forward Declarations
namespace Apoc3D
{
	class Material;
	class SceneRenderer;
	class GameTime;

	namespace Core
	{
		class IRenderable;
		class Camera;
		class RenderOperation;
		class RenderOperationBuffer;
		class SceneManager;
	}

}
//struct IDirect3DBaseTexture9;
//struct IDirect3DVertexBuffer9;
//struct IDirect3DIndexBuffer9;
//struct IDirect3DIndexBuffer9;
//struct IDirect3DDevice9;
//
//void memcpyf(void* src, void* dst, uint size);
//int memcmpf(void* src, void* dst, uint size);
//void memsetf(void* dst, int value, uint count);