/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/


#ifndef COMMON_H
#define COMMON_H
#pragma once

typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long BatchHandle;

#if APOC3D_DLLEX
	#define _Export __declspec( dllexport )
#else
	#define _Export 
#endif


struct _Export D3DXPLANE;
struct _Export D3DXVECTOR2;
struct _Export D3DXVECTOR3;
struct _Export D3DXVECTOR4;
struct _Export D3DXMATRIX;
struct _Export D3DXQUATERNION;
struct _Export _D3DCOLORVALUE;

struct _Export IDirect3DDevice9;
struct _Export IDirect3DBaseTexture9;
struct _Export IDirect3DTexture9;
struct _Export ID3DXSprite;
struct _Export IDirect3DIndexBuffer9;
struct _Export IDirect3DVertexBuffer9;
struct _Export IDirect3DVertexShader9;
struct _Export IDirect3DPixelShader9;
struct _Export IDirect3DVertexDeclaration9;

enum _Export D3DPRIMITIVETYPE;

#include "Include\d3d9.h"
#include "Include\d3dx9.h"

#include <windows.h>
#include <algorithm>
#include <string>


// Forward Declarations
namespace Apoc3D
{
	class GameTime;
	class Material;
	class SceneRenderer;

	typedef D3DXMATRIX Matrix;
	typedef D3DXPLANE Plane;
	typedef D3DXVECTOR2 Vector2;
	typedef D3DXVECTOR3 Vector3;
	typedef D3DXVECTOR4 Vector4;
	typedef D3DXQUATERNION Quaternion;
	typedef D3DCOLORVALUE Color4;
	
	typedef IDirect3DDevice9 Device;
	typedef IDirect3DBaseTexture9 BaseTexture;
	typedef IDirect3DTexture9 Texture;
	typedef ID3DXSprite Sprite;
	typedef IDirect3DIndexBuffer9 IndexBuffer;
	typedef IDirect3DVertexBuffer9 VertexBuffer;
	typedef IDirect3DVertexShader9 VertexShader;
	typedef IDirect3DPixelShader9 PixelShader;
	typedef IDirect3DVertexDeclaration9 VertexDeclaration;

	typedef D3DPRIMITIVETYPE PrimitiveType;
	typedef std::string String;
	
	const Vector3 UnitX3(1,0,0);
	const Vector3 UnitY3(0,1,0);
	const Vector3 UnitZ3(0,0,1);
	const Vector3 One3(1,1,1);
	const Vector3 Zero3(0,0,0);


	namespace Core
	{
		class IRenderable;
		class Camera;
		class GeometryData;
		class RenderOperation;
		class RenderOperationBuffer;
		class SceneManager;
		class SceneObject;
		class Resource;
	};
	namespace IO
	{
		class Stream;
		class FileStream;
	};
	namespace EffectSystem
	{
		class Effect;
		class EffectManager;
		class EffectAtom;
	};
	namespace Rendering
	{
		class ScenePass;
		class SceneProcedure;
	};
};

#endif