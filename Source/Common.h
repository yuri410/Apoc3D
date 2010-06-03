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
	class GameTime;
	class Material;
	class SceneRenderer;

	#define Matrix D3DXMATRIX
	#define Vector2 D3DXVECTOR2
	#define Vector3 D3DXVECTOR3
	#define Vector4 D3DXVECTOR4
	#define Plane D3DXPLANE



	//typedef _Export D3DMATRIX Matrix;
	//typedef _Export D3DXVECTOR2 Vector2;
	//typedef _Export D3DXVECTOR3 Vector3;
	//typedef _Export D3DXVECTOR4 Vector4;
	//typedef _Export D3DXPLANE Plane;
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