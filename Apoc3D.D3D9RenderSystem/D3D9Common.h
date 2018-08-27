#pragma once
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#ifndef D3DCOMMON_H
#define D3DCOMMON_H

#include "apoc3d/ApocCommon.h"
#include "apoc3d.Win32/Win32Common.h"

#if _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <Windows.h>
#include <d3d9.h>
#include <d3d9types.h>
#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")
#pragma comment(lib, "Apoc3D.Win32.lib")

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")

typedef IDirect3DDevice9 D3DDevice;
typedef IDirect3DBaseTexture9 D3DBaseTexture;
typedef IDirect3DTexture9 D3DTexture2D;
typedef IDirect3DVolumeTexture9 D3DTexture3D;
typedef IDirect3DCubeTexture9 D3DTextureCube;

//typedef ID3DXSprite D3DSprite;
typedef IDirect3DIndexBuffer9 D3DIndexBuffer;
typedef IDirect3DVertexBuffer9 D3DVertexBuffer;
typedef IDirect3DVertexShader9 D3DVertexShader;
typedef IDirect3DPixelShader9 D3DPixelShader;
typedef IDirect3DVertexDeclaration9 D3DVertexDeclaration;

using namespace Apoc3D::Win32;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class GraphicsDeviceManager;

			class D3D9RenderWindow;
			class D3D9RenderView;
			class D3D9RenderViewSet;
			class D3D9DeviceContext;

			class D3D9Texture;
			class D3D9RenderTarget;

			class D3D9RenderDevice;
			class D3D9RenderStateManager;
			class NativeD3DStateManager;

			class D3D9VertexBuffer;
			class D3D9IndexBuffer;
			class D3D9DepthBuffer;			
			class D3D9VertexDeclaration;

			class D3D9InstancingData;

			class ConstantTable;

			class D3D9VertexShader;
			class D3D9PixelShader;

			class VolatileResource;

			struct RawSettings;
		}
	}
}
#endif