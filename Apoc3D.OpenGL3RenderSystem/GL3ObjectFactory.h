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

#ifndef GL3OBJECTFACTORY_H
#define GL3OBJECTFACTORY_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			/** The place for creating graphics objects under OpenGL.
			 *  See ObjectFactory for general description.
			 */
			class GL3ObjectFactory : public ObjectFactory
			{
			public:
				GL3ObjectFactory(GL3RenderDevice* device);

				virtual Texture* CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed);
				virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format);

				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt);

				virtual IndexBuffer* CreateIndexBuffer(IndexBufferFormat type, int count, BufferUsageFlags usage);
				virtual VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage);

				virtual VertexDeclaration* CreateVertexDeclaration(const List<VertexElement>& elements);

				virtual Shader* CreateVertexShader(const ResourceLocation* resLoc);
				virtual Shader* CreatePixelShader(const ResourceLocation* resLoc);

				virtual Shader* CreateVertexShader(const byte* byteCode);
				virtual Shader* CreatePixelShader(const byte* byteCode);

				virtual Sprite* CreateSprite();

			private:
				GL3RenderDevice* m_device;

			};
		}
	}
}

#endif