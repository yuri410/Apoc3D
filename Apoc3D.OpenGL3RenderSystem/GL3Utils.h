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
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			namespace GLUtils
			{
				GLenum ConvertCompare(CompareFunction cmp);
				GLenum ConvertBlend(Blend dv);
				GLenum ConvertBlendFunction(BlendFunction dv);
				GLenum ConvertStencilOperation(StencilOperation so, bool invert);

				GLenum ConvertTextureAddress(TextureAddressMode ta);
				GLenum ConvertCubemapFace(CubeMapFace cubeface);

				GLenum ConvertPrimitiveType(PrimitiveType pt);
				
				GLenum ConvertIndexBufferFormat(IndexBufferFormat ibf);

				/** Converts PixelFormat to "GLenum format, GLenum type, GLenum internalFormat" */
				bool ConvertPixelFormat(PixelFormat fmt, GLenum& format, GLenum& type, GLenum& internalFormat);
				bool ConvertDepthFormat(DepthFormat fmt, GLenum& format, GLenum& type, GLenum& internalFormat);

				/** Converts TextureType to GL_TEXTURE_1D, GL_TEXTURE_2D.. */
				GLenum GetTextureTarget(TextureType type);

				GLbitfield ConvertLockMode(LockMode mode);

				bool ConvertVertexElementFormat(VertexElementFormat vef, GLenum& elementType, GLuint& elementCount, GLboolean& normalized);
				
				bool CheckError(const char* file, unsigned line);
				bool CheckFramebufferError(const char* file, unsigned line);

				extern const TypeDualConverter<VertexElementUsage> VertexElementUsageConverter;
			}

			//	static D3DMULTISAMPLE_TYPE ConvertMultisample(uint32 sampleCount);

		}
	}
}

#if _DEBUG
	#define GL_CHECK { Apoc3D::Graphics::GL3RenderSystem::GLUtils::CheckError(_CRT_WIDE(__FILE__), (unsigned)(__LINE__)); }
	#define GL_CHECKFB { Apoc3D::Graphics::GL3RenderSystem::GLUtils::CheckFramebufferError(_CRT_WIDE(__FILE__), (unsigned)(__LINE__)); }
#else
	#define GL_CHECK
	#define GL_CHECKFB
#endif

#endif