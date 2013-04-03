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

#include "GL1ObjectFactory.h"
#include "GL1RenderDevice.h"
#include "GL1Texture.h"
#include "Buffer/GL1VertexBuffer.h"
#include "Buffer/GL1IndexBuffer.h"
#include "GL1VertexShader.h"
#include "GL1PixelShader.h"
#include "GL1VertexDeclaration.h"
#include "GL1RenderTarget.h"
#include "GL1Sprite.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			GL1ObjectFactory::GL1ObjectFactory(GL1RenderDevice* device)
				: ObjectFactory(device), m_device(device)
			{

			}
			Texture* GL1ObjectFactory::CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed)
			{
				return new GL1Texture(m_device, rl, usage, managed);
			}
			Texture* GL1ObjectFactory::CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL1Texture(m_device, width, height, 1, levelCount, format, usage);
			}
			Texture* GL1ObjectFactory::CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL1Texture(m_device, width, height, depth, levelCount, format, usage);
			}
			Texture* GL1ObjectFactory::CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new GL1Texture(m_device, length, levelCount, format, usage);
			}

			RenderTarget* GL1ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount)
			{
				return new GL1RenderTarget(m_device, width, height, sampleCount, clrFmt, depthFmt);
			}
			RenderTarget* GL1ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt)
			{
				return new GL1RenderTarget(m_device, width, height, clrFmt, depthFmt);
			}
			RenderTarget* GL1ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt)
			{
				return new GL1RenderTarget(m_device, width, height, clrFmt);
			}

			IndexBuffer* GL1ObjectFactory::CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage)
			{
				//return new D3D9IndexBuffer(m_device, type, count * (type == IBT_Bit16 ? sizeof(ushort) : sizeof(uint)), usage);
			}
			VertexBuffer* GL1ObjectFactory::CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
			{
				//return new D3D9VertexBuffer(m_device, vertexCount * vtxDecl->GetVertexSize(), usage);
			}

			VertexDeclaration* GL1ObjectFactory::CreateVertexDeclaration(const FastList<VertexElement>& elements)
			{
				//return new D3D9VertexDeclaration(m_device, elements);
			}

			VertexShader* GL1ObjectFactory::CreateVertexShader(const ResourceLocation* resLoc)
			{
				//return new D3D9VertexShader(m_device, resLoc);
			}
			PixelShader* GL1ObjectFactory::CreatePixelShader(const ResourceLocation* resLoc)
			{
				//return new D3D9PixelShader(m_device, resLoc);
			}

			VertexShader* GL1ObjectFactory::CreateVertexShader(const byte* byteCode)
			{
				//return new D3D9VertexShader(m_device, byteCode);
			}
			PixelShader* GL1ObjectFactory::CreatePixelShader(const byte* byteCode)
			{
				//return new D3D9PixelShader(m_device, byteCode);
			}

			Sprite* GL1ObjectFactory::CreateSprite()
			{
				//return new D3D9Sprite(m_device);
			}
		}
	}
}