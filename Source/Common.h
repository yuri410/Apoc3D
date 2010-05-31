#ifndef COMMON_H
#define COMMON_H
#pragma once

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;

#include "Include\d3d9.h"
#include "Include\d3dx9.h"

#include <windows.h>
#include <algorithm>
#include <string>

#define _Export __declspec( dllexport )


// Forward Declarations
namespace Apoc3D
{
	class Material;
	class SceneRenderer;
	class GameTime;
	
	typedef _Export D3DMATRIX Matrix;
	typedef _Export D3DXVECTOR2 Vector2;
	typedef _Export D3DXVECTOR3 Vector3;
	typedef _Export D3DXVECTOR4 Vector4;
	typedef _Export D3DXPLANE Plane;
	typedef _Export D3DXQUATERNION Quaternion;
	typedef _Export IDirect3DDevice9 Device;
	typedef _Export IDirect3DBaseTexture9 BaseTexture;
	typedef _Export IDirect3DTexture9 Texture;
	typedef _Export ID3DXSprite Sprite;
	typedef _Export IDirect3DIndexBuffer9 IndexBuffer;
	typedef _Export IDirect3DVertexBuffer9 VertexBuffer;
	typedef _Export IDirect3DVertexShader9 VertexShader;
	typedef _Export IDirect3DPixelShader9 PixelShader;
	typedef _Export IDirect3DVertexDeclaration9 VertexDeclaration;
	typedef D3DCOLORVALUE Color4;

	typedef std::string String;
	
	namespace Core
	{
		class IRenderable;
		class Camera;
		class GeomentryData;
		class RenderOperation;
		class RenderOperationBuffer;
		class SceneManager;
		class SceneObject;
		class Resource;
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

#endif