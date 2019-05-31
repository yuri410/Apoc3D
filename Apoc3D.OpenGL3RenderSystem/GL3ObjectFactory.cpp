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

#include "GL3ObjectFactory.h"
#include "GL3RenderDevice.h"
#include "GL3Texture.h"
#include "GL3Buffers.h"
#include "GL3Sprite.h"
#include "GL3Shader.h"
#include "GL3VertexDeclaration.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3ObjectFactory::GL3ObjectFactory(GL3RenderDevice* device)
				: ObjectFactory(device), m_device(device)
			{

			}
			Texture* GL3ObjectFactory::CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed)
			{
				return new GL3Texture(m_device, rl, usage, managed);
			}
			Texture* GL3ObjectFactory::CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL3Texture(m_device, width, height, 1, levelCount, format, usage);
			}
			Texture* GL3ObjectFactory::CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL3Texture(m_device, width, height, depth, levelCount, format, usage);
			}
			Texture* GL3ObjectFactory::CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL3Texture(m_device, length, levelCount, format, usage);
			}

			RenderTarget* GL3ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount)
			{
				return nullptr;
			}
			RenderTarget* GL3ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt)
			{
				return nullptr;
			}
			RenderTarget* GL3ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt)
			{
				return nullptr;
			}

			IndexBuffer* GL3ObjectFactory::CreateIndexBuffer(IndexBufferFormat type, int count, BufferUsageFlags usage)
			{
				return new GL3IndexBuffer(m_device, type, count*(type == IndexBufferFormat::Bit16 ? sizeof(ushort) : sizeof(uint)), usage);
			}
			VertexBuffer* GL3ObjectFactory::CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
			{
				return new GL3VertexBuffer(m_device, vertexCount, vtxDecl->GetVertexSize(), usage);
			}

			VertexDeclaration* GL3ObjectFactory::CreateVertexDeclaration(const List<VertexElement>& elements)
			{
				return new GL3VertexDeclaration(elements);
			}

			Shader* GL3ObjectFactory::CreateVertexShader(const ResourceLocation* resLoc)
			{
				return GL3VertexShader(m_device, *resLoc);
			}
			Shader* GL3ObjectFactory::CreatePixelShader(const ResourceLocation* resLoc)
			{
				return GL3PixelShader(m_device, *resLoc);
			}

			Shader* GL3ObjectFactory::CreateVertexShader(const byte* byteCode)
			{
				return GL3VertexShader(m_device, byteCode);
			}
			Shader* GL3ObjectFactory::CreatePixelShader(const byte* byteCode)
			{
				return GL3PixelShader(m_device, byteCode);
			}

			Sprite* GL3ObjectFactory::CreateSprite()
			{
				return new GL3Sprite(m_device);
			}
		}
	}
}