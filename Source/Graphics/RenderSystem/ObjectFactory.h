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
#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "Common.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;


namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* Factory that can create/destroy resources like textures, buffers, shaders. 
			* Application can use this to create resources that are not managed by resource managers.
			*/
			class APAPI ObjectFactory
			{
			private:
				RenderDevice* m_renderDevice;

			protected:
				ObjectFactory(RenderDevice* rd)
					: m_renderDevice(rd)
				{
				}

			public:
				virtual Texture* CreateTexture(const ResourceLocation* rl, TextureUsage usage, bool managed) = 0;
				virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format) = 0;
				virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format) = 0;
				virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format) = 0;

				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt) = 0;
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt) = 0;

				virtual Sprite* CreateSprite();

				virtual IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage, bool useSysMem) = 0;
				virtual VertexBuffer CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage, bool useSysMem) = 0;

				VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
				{
					return CreateVertexBuffer(vertexCount, vtxDecl, usage, false);
				}
				IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage)
				{
					return CreateIndexBuffer(type, count, usage, false);
				}

				virtual VertexDeclaration* CreateVertexDeclaration(const vector<VertexElement> &elements) = 0;
				virtual StateBlock* CreateStateBlock() = 0;

				VertexShader* CreateVertexShader(const ResourceLocation* resLoc) = 0;
				PixelShader* CreatePixelShader(const ResourceLocation* resLoc) = 0;

				VertexShader* CreateVertexShader(const byte* byteCode, int len) = 0;
				PixelShader* CreatePixelShader(const byte* byteCode, int len) = 0;
			};
		}
	}
}


#endif