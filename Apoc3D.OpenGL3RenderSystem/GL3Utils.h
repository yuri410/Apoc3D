#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GLUTILS_H
#define GLUTILS_H

#include "GL3Common.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GLUtils
			{
			public:
				static GLenum ConvertCompare(CompareFunction cmp);
				static GLenum ConvertBlend(Blend dv);
				static GLenum ConvertBlendFunction(BlendFunction dv);
				static GLenum ConvertStencilOperation(StencilOperation so);
				static GLenum ConvertStencilOperation(StencilOperation so, bool invert);
			private:
			//	static GLUtils s_initlaizer;
			//	static D3DPRIMITIVETYPE ptTable[PT_Count];
			//	static D3DDECLTYPE vefTable[VEF_Count];
			//	static D3DDECLUSAGE veuTable[VEU_Count];

				static GLenum blendTable[];
				static GLenum comfunTable[];
				static GLenum blendopTable[];
			//	static D3DFORMAT pixFmtTable[FMT_Count];
			//	static D3DFORMAT depFmtTable[DEPFMT_Count];
			//	static D3DTEXTUREFILTERTYPE tfltTable[TFLT_Count];
			//	static D3DCUBEMAP_FACES cubeTable[CUBE_Count];
			//	static D3DTEXTUREADDRESS taTable[TA_Count];
			//private:
			//	static void InitPrimitiveTable();
			//	static void InitVertexElementFormat();
			//	static void InitVertexElementUsage();
			//	static void InitFillTable();
			//	static void InitCullTable();
				static void InitBlendTable();
				static void InitCompareFunctionTable();
				static void InitBlendOperationTable();
			//	static void InitPixelFormatTable();
			//	static void InitDepthFormatTable();
			//	static void InitTFLTTable();
			//	static void InitCubeTable();
			//	static void InitTATable();

				GLUtils();
			public:
				~GLUtils() { }

			//	static BufferUsageFlags GetBufferUsage(DWORD usage);
			//	static PixelFormat ConvertBackPixelFormat(DWORD format);
			//	static DepthFormat ConvertBackDepthFormat(DWORD format);


				/** Converts PixelFormat to "GLenum format, GLenum type, GLenum internalFormat"
				*/
				static bool ConvertPixelFormat(PixelFormat fmt, GLenum& format, GLenum& type, GLenum& internalFormat);

				/** Converts TextureType to GL_TEXTURE_1D, GL_TEXTURE_2D..
				*/
				static GLenum GetTextureTarget(TextureType type)
				{
					switch (type)
					{
						case TextureType::Texture1D: return GL_TEXTURE_1D;
						case TextureType::Texture2D: return GL_TEXTURE_2D;
						case TextureType::Texture3D: return GL_TEXTURE_3D;
						case TextureType::CubeTexture: return GL_TEXTURE_CUBE_MAP;
					}
					// keep the compiler happy
					return GL_TEXTURE_1D; 
				}




			//	static PixelFormat GetD3DTextureFormat(D3DTexture3D* tex);
			//	static TextureUsage GetD3DTextureUsage(D3DTexture3D* tex);


			//	static int32 GetD3DTextureLength(D3DTextureCube* tex)
			//	{
			//		D3DSURFACE_DESC desc;
			//		tex->GetLevelDesc(0, &desc);

			//		return static_cast<int32>(desc.Width);
			//	}

			//	static PixelFormat GetD3DTextureFormat(D3DTextureCube* tex);
			//	static TextureUsage GetD3DTextureUsage(D3DTextureCube* tex);


			//	static VertexElementFormat ConvertBackVertexElementFormat(D3DDECLTYPE type);
			//	static VertexElementUsage ConvertBackVertexElementUsage(D3DDECLUSAGE usage);

			//	static D3DMULTISAMPLE_TYPE ConvertMultisample(uint32 sampleCount);
			//	static uint32 ConvertBackMultiSample(D3DMULTISAMPLE_TYPE type);


			//	static DWORD ConvertLockMode(LockMode mode);
			//	static DWORD ConvertClearFlags(ClearFlags flags);

			//	static DWORD ConvertBufferUsage(BufferUsageFlags usage);

			//	static D3DPRIMITIVETYPE ConvertPrimitiveType(PrimitiveType type);

			//	static D3DDECLUSAGE ConvertVertexElementUsage(VertexElementUsage usage);

			//	static D3DDECLTYPE ConvertVertexElementFormat(VertexElementFormat type);

			//	static D3DFILLMODE ConvertFillMode(FillMode mode);

			//	static D3DCULL ConvertCullMode(CullMode mode);


			//	static D3DCMPFUNC ConvertCompare(CompareFunction fun);

			//	static D3DBLENDOP ConvertBlendFunction(BlendFunction fun);

			//	static D3DSTENCILOP ConvertStencilOperation(StencilOperation op);

			//	static D3DFORMAT ConvertPixelFormat(PixelFormat format);
			//	static D3DFORMAT ConvertDepthFormat(DepthFormat format);

			//	static D3DTEXTUREFILTERTYPE ConvertTextureFilter(TextureFilter filter);

			//	static DWORD ConvertTextureUsage(TextureUsage usage);
			//	static D3DCUBEMAP_FACES ConvertCubeMapFace(CubeMapFace face);

			//	static D3DTEXTUREADDRESS ConvertTextureAddress(TextureAddressMode ta);
			};
		}
	}
}

#endif