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

				Texture* CreateTexture(const ResourceLocation& rl, TextureUsage usage, bool managed) override;
				Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format) override;
				Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format) override;
				Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format) override;

				RenderTarget* CreateRenderTarget(int32 width, int32 height, PixelFormat clrFmt, const String& multisampleMode) override;
				DepthStencilBuffer* CreateDepthStencilBuffer(int32 width, int32 height, DepthFormat depFmt, const String& multisampleMode) override;
				CubemapRenderTarget* CreateCubemapRenderTarget(int32 length, PixelFormat clrFmt) override;

				IndexBuffer* CreateIndexBuffer(IndexBufferFormat type, int count, BufferUsageFlags usage) override;
				VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage) override;

				VertexDeclaration* CreateVertexDeclaration(const List<VertexElement>& elements) override;

				Shader* CreateVertexShader(const byte* byteCode) override;
				Shader* CreatePixelShader(const byte* byteCode) override;

				Sprite* CreateSprite() override;

			private:
				GL3RenderDevice* m_device;

			};
		}
	}
}

#endif