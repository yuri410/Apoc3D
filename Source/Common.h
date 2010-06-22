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
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef uint16 ushort;
typedef uint32 uint;
typedef uint64 ulong;
typedef unsigned long long BatchHandle;



#if APOC3D_DLLEX
	#define _Export __declspec( dllexport )
#else
	#define _Export __declspec( dllimport )
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

#include "Include\d3d9.h"
#include "Include\d3dx9.h"

#include <cassert>

#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>


// Forward Declarations
namespace Apoc3D
{
	#define DELEGATE(name, retn) typedef retn (CALLBACK *name)

	DELEGATE(EventHandler, void)();
	DELEGATE(CancellableEventHandler, void)(bool* cancel);

	template<typename T>
	class _Export DelegateEvent
	{
	private:
		std::vector<T> m_funcList;
	public:
		DelegateEvent(){}
		~DelegateEvent(){}

		inline DelegateEvent<T> &operator +=(const T &value) 
		{
			m_funcList.push_back(value);
			return *this;
		}

		void add(T value)
		{
			m_funcList.push_back(value);
		}

		int32 count() const { return m_funcList.size(); }
		const T& operator[](int i) const { return m_funcList[i]; }
		
		void reset() { m_funcList.clear(); }
	private:
		void* operator new(size_t sz)
		{
			return 0;
		}
	};

	#define INVOKE(de) for (int32 i=0;i<de.count();i++) de[i]

	class Game;
	class GameClock;
	class GameTime;
	class GameWindow;




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
	typedef D3DFORMAT Format;
	typedef D3DMULTISAMPLE_TYPE MSAAType;
	typedef D3DDEVTYPE DeviceType;
	typedef std::wstring String;
	
	const int32 MaxInt32 = 0x7fffffff;

	const Vector3 UnitX3(1,0,0);
	const Vector3 UnitY3(0,1,0);
	const Vector3 UnitZ3(0,0,1);
	const Vector3 One3(1,1,1);
	const Vector3 Zero3(0,0,0);

	struct _Export Size 
	{
		int32 Width, Height;
		Size(){}
		Size(const int32 width, const int height) { Width = width; Height = height; }
	
		inline bool operator == ( const Size& value ) const
        {
            return ( Width == value.Width && Height == value.Height );
        }

        inline bool operator != ( const Size& value ) const
        {
            return ( Width != value.Width || Height != value.Height );
        }
	};

	int32 convint32(const char* const src)
	{
#if LITTLE_INDIAN
		return *reinterpret_cast<const int32*>(src);
#else
		return (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
#endif
	}
	int16 convint16(const char* const src)
	{
#if LITTLE_INDIAN
		return *reinterpret_cast<const int16*>(src);
#else
		return (src[0] << 8) | src[1];
#endif
	}
	int64 convint64(const char* const src)
	{
#if LITTLE_INDIAN
		return *reinterpret_cast<const int64*>(src);
#else
		return (src[0] << 56) | (src[1] << 48) | (src[2] << 40) | (src[3] << 32) | 
			(src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7];
#endif
	}
	const float convr32(const char* const src)
	{
#if LITTLE_INDIAN
		return *reinterpret_cast<const float*>(src);
#else
		return reinterpret_cast<float>((src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3]);		
#endif
	}
	const double convr64(const char* const src)
	{
#if LITTLE_INDIAN
		return *reinterpret_cast<const double*>(src);
#else
		return reinterpret_cast<double>(
			(src[0] << 56) | (src[1] << 48) | (src[2] << 40) | (src[3] << 32) | 
			(src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7]);
#endif
	}
	
	namespace Core
	{
		class Camera;
		class Resource;
	};
	namespace IO
	{
		class Stream;
		class FileStream;
		class BinaryReader;

	};
	namespace EffectSystem
	{
		class Effect;
		class EffectManager;
		class EffectAtom;
	};
	namespace Scene
	{
		class SceneManager;
		class SceneNode;
		class SceneObject;
	};
	namespace Graphics
	{
		class IRenderable;
		class GeometryData;
		class RenderOperation;
		class RenderOperationBuffer;
		class Material;
		class SceneRenderer;
		class GraphicsDeviceManager;
		class ScenePass;
		class SceneProcedure;
		class SceneRenderer;
	};
};

#endif