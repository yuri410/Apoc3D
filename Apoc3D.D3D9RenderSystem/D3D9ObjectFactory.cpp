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

#include "D3D9ObjectFactory.h"
#include "D3D9RenderDevice.h"
#include "D3D9Texture.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "Buffer/D3D9IndexBuffer.h"
#include "D3D9TypedShader.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9RenderTarget.h"
#include "D3D9Sprite.h"
#include "D3D9DepthBuffer.h"
#include "GraphicsDeviceManager.h"
#include "D3D9Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9ObjectFactory::D3D9ObjectFactory(D3D9RenderDevice* device)
				: ObjectFactory(device), m_device(device)
			{

			}
			Texture* D3D9ObjectFactory::CreateTexture(const ResourceLocation& rl, TextureUsage usage, bool managed)
			{
				return new D3D9Texture(m_device, rl, usage, managed);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int32 width, int32 height, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, width, height, 1, levelCount, format, usage);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int32 width, int32 height, int32 depth, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, width, height, depth, levelCount, format, usage);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int32 length, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, length, levelCount, format, usage);
			}


			RenderTarget* D3D9ObjectFactory::CreateRenderTarget(int32 width, int32 height, PixelFormat clrFmt, const String& multisampleMode)
			{
				return new D3D9RenderTarget(m_device, width, height, clrFmt, multisampleMode);
			}
			DepthStencilBuffer* D3D9ObjectFactory::CreateDepthStencilBuffer(int32 width, int32 height, DepthFormat depFmt, const String& multisampleMode)
			{
				return new D3D9DepthBuffer(m_device, width, height, depFmt, multisampleMode);
			}
			CubemapRenderTarget* D3D9ObjectFactory::CreateCubemapRenderTarget(int32 length, PixelFormat clrFmt)
			{
				return new D3D9CubemapRenderTarget(m_device, length, clrFmt);
			}


			IndexBuffer* D3D9ObjectFactory::CreateIndexBuffer(IndexBufferFormat type, int32 count, BufferUsageFlags usage)
			{
				return new D3D9IndexBuffer(m_device, type, count * (type == IndexBufferFormat::Bit16 ? sizeof(ushort) : sizeof(uint)), usage);
			}
			VertexBuffer* D3D9ObjectFactory::CreateVertexBuffer(int32 vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
			{
				return new D3D9VertexBuffer(m_device, vertexCount, vtxDecl->GetVertexSize(), usage);
			}

			VertexDeclaration* D3D9ObjectFactory::CreateVertexDeclaration(const List<VertexElement>& elements)
			{
				return new D3D9VertexDeclaration(m_device, elements);
			}

			Shader* D3D9ObjectFactory::CreateVertexShader(const ResourceLocation& resLoc)
			{
				return new D3D9VertexShader(m_device, resLoc);
			}
			Shader* D3D9ObjectFactory::CreatePixelShader(const ResourceLocation& resLoc)
			{
				return new D3D9PixelShader(m_device, resLoc);
			}

			Shader* D3D9ObjectFactory::CreateVertexShader(const byte* byteCode)
			{
				return new D3D9VertexShader(m_device, byteCode);
			}
			Shader* D3D9ObjectFactory::CreatePixelShader(const byte* byteCode)
			{
				return new D3D9PixelShader(m_device, byteCode);
			}

			Sprite* D3D9ObjectFactory::CreateSprite()
			{
				return new D3D9Sprite(m_device);
			}
		}
	}
}