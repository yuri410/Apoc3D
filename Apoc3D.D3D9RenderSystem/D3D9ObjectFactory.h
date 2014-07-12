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
			class D3D9ObjectFactory : public ObjectFactory
			{
			private:
				D3D9RenderDevice* m_device;
			public:
				D3D9ObjectFactory(D3D9RenderDevice* device);

				virtual Texture* CreateTexture(const ResourceLocation& rl, TextureUsage usage, bool managed);
				virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format);

				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, const String& multisampleMode);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, const String& multisampleMode);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt);

				virtual IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage);
				virtual VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage);

				virtual VertexDeclaration* CreateVertexDeclaration(const FastList<VertexElement> &elements);

				virtual Shader* CreateVertexShader(const ResourceLocation& resLoc);
				virtual Shader* CreatePixelShader(const ResourceLocation& resLoc);

				virtual Shader* CreateVertexShader(const byte* byteCode);
				virtual Shader* CreatePixelShader(const byte* byteCode);

				virtual Sprite* CreateSprite();
			};
		}
	}
}

#endif