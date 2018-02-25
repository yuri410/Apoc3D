#pragma once

/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#ifndef NRSOBJECTFACTORY_H
#define NRSOBJECTFACTORY_H

#include "NRSCommon.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSObjectFactory final : public ObjectFactory
			{
			public:
				NRSObjectFactory(NRSRenderDevice* device);

				Texture* CreateTexture(const ResourceLocation& rl, TextureUsage usage, bool managed) override;
				Texture* CreateTexture(int32 width, int32 height, int32 levelCount, TextureUsage usage, PixelFormat format) override;
				Texture* CreateTexture(int32 width, int32 height, int32 depth, int32 levelCount, TextureUsage usage, PixelFormat format) override;
				Texture* CreateTexture(int32 length, int32 levelCount, TextureUsage usage, PixelFormat format) override;

				RenderTarget* CreateRenderTarget(int32 width, int32 height, PixelFormat clrFmt, const String& multisampleMode) override;
				DepthStencilBuffer* CreateDepthStencilBuffer(int32 width, int32 height, DepthFormat depFmt, const String& multisampleMode) override;

				CubemapRenderTarget* CreateCubemapRenderTarget(int32 length, PixelFormat clrFmt) override;


				IndexBuffer* CreateIndexBuffer(IndexBufferFormat type, int32 count, BufferUsageFlags usage) override;
				VertexBuffer* CreateVertexBuffer(int32 vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage) override;

				VertexDeclaration* CreateVertexDeclaration(const List<VertexElement> &elements) override;

				Shader* CreateVertexShader(const ResourceLocation& resLoc) override;
				Shader* CreatePixelShader(const ResourceLocation& resLoc) override;

				Shader* CreateVertexShader(const byte* byteCode) override;
				Shader* CreatePixelShader(const byte* byteCode) override;

				Sprite* CreateSprite() override;

			private:
				NRSRenderDevice* m_device;

			};
		}
	}
}

#endif