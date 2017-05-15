#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#ifndef D3D9OBJECTFACTORY_H
#define D3D9OBJECTFACTORY_H

#include "D3D9Common.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/** The place for creating graphics objects under D3D9.
			 *  See ObjectFactory for general description.
			 */
			class D3D9ObjectFactory final : public ObjectFactory
			{
			public:
				D3D9ObjectFactory(D3D9RenderDevice* device);

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
				D3D9RenderDevice* m_device;

			};
		}
	}
}

#endif