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

#ifndef D3D9UTILS_H
#define D3D9UTILS_H

#include "D3D9Common.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9Utils
			{
			private:
				static DWORD ptTable[PT_Count];
				static D3DDECLTYPE vefTable[VEF_Count];
				static D3DDECLUSAGE veuTable[VEU_Count];
				static D3DFILLMODE fillTable[FILL_Count];
				static D3DCULL cullTable[CULL_Count];
				static D3DBLEND blendTable[BLEND_Count];
				static D3DCMPFUNC comfunTable[COMFUN_Count];
				static D3DBLENDOP blendopTable[BLFUN_Count];
				static D3DSTENCILOP stencilTable[STOP_Count];
				static D3DFORMAT pixFmtTable[FMT_Count];
				static D3DFORMAT depFmtTable[DEPFMT_Count];
				static D3DTEXTUREFILTERTYPE tfltTable[TFLT_Count];
				static D3DCUBEMAP_FACES cubeTable[CUBE_Count];
				
			private:
				static void InitPrimitiveTable();
				static void InitVertexElementFormat();
				static void InitVertexElementUsage();
				static void InitFillTable();
				static void InitCullTable();
				static void InitBlendTable();
				static void InitCompareFunctionTable();
				static void InitBlendOperationTable();
				static void InitStencilTable();
				static void InitPixelFormatTable();
				static void InitDepthFormatTable();
				static void InitTFLTTable();
				static void InitCubeTable();

				D3D9Utils();
			public:
				~D3D9Utils() { }

				
				static int32 GetD3DTextureWidth(D3DTexture2D* tex)
				{
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Width);
				}
				static int32 GetD3DTextureHeight(D3DTexture2D* tex)
				{
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Height);
				}
				static PixelFormat GetD3DTextureFormat(D3DTexture2D* tex);
				static TextureUsage GetD3DTextureUsage(D3DTexture2D* tex);


				static int32 GetD3DTextureWidth(D3DTexture3D* tex)
				{
					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Width);
				}
				static int32 GetD3DTextureHeight(D3DTexture3D* tex)
				{
					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Height);
				}
				static int32 GetD3DTextureDepth(D3DTexture3D* tex)
				{
					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Depth);
				}
				static PixelFormat GetD3DTextureFormat(D3DTexture3D* tex);
				static TextureUsage GetD3DTextureUsage(D3DTexture3D* tex);


				static int32 GetD3DTextureLength(D3DTextureCube* tex)
				{
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(0, &desc);

					return static_cast<int32>(desc.Width);
				}

				static PixelFormat GetD3DTextureFormat(D3DTextureCube* tex);
				static TextureUsage GetD3DTextureUsage(D3DTextureCube* tex);


				static DWORD ConvertLockMode(LockMode mode);
				static DWORD ConvertClearFlags(ClearFlags flags);

				static DWORD ConvertBufferUsage(BufferUsageFlags usage);

				static DWORD ConvertPrimitiveType(PrimitiveType type);

				static D3DDECLUSAGE ConvertVertexElementUsage(VertexElementUsage usage);

				static D3DDECLTYPE ConvertVertexElementFormat(VertexElementFormat type);

				static D3DFILLMODE ConvertFillMode(FillMode mode);

				static D3DCULL ConvertCullMode(CullMode mode);



				static D3DBLEND ConvertBlend(Blend dv);

				static D3DCMPFUNC ConvertCompare(CompareFunction fun);

				static D3DBLENDOP ConvertBlendFunction(BlendFunction fun);

				static D3DSTENCILOP ConvertStencilOperation(StencilOperation op);

				static D3DFORMAT ConvertPixelFormat(PixelFormat format);
				static D3DFORMAT ConvertDepthFormat(DepthFormat format);

				static D3DTEXTUREFILTERTYPE ConvertTextureFilter(TextureFilter filter);

				static DWORD ConvertTextureUsage(TextureUsage usage);
				static D3DCUBEMAP_FACES ConvertCubeMapFace(CubeMapFace face);
			};
		}
	}
}

#endif