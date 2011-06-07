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

#include "D3D9ObjectFactory.h"
#include "D3D9RenderDevice.h"
#include "D3D9Texture.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "Buffer/D3D9IndexBuffer.h"
#include "D3D9VertexShader.h"
#include "D3D9PixelShader.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9RenderTarget.h"


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
			Texture* D3D9ObjectFactory::CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed)
			{
				return new D3D9Texture(m_device, rl, usage, managed);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, width, height, 1, levelCount, format, usage);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, width, height, depth, levelCount, format, usage);
			}
			Texture* D3D9ObjectFactory::CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format)
			{
				return new D3D9Texture(m_device, length, levelCount, format, usage);
			}

			RenderTarget* D3D9ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount)
			{
				return new D3D9RenderTarget(m_device, width, height, sampleCount, clrFmt, depthFmt)
			}
			RenderTarget* D3D9ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt)
			{
				return new D3D9RenderTarget(m_device, width, height, clrFmt, depthFmt);
			}
			RenderTarget* D3D9ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt)
			{
				return new D3D9RenderTarget(m_device, width, height, clrFmt);
			}

			IndexBuffer* D3D9ObjectFactory::CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage)
			{
				return new D3D9IndexBuffer(m_device, type, count * (type == IBT_Bit16 ? sizeof(ushort) : sizeof(uint)), usage);
			}
			VertexBuffer* D3D9ObjectFactory::CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
			{
				return new D3D9VertexBuffer(m_device, vertexCount * vtxDecl->GetVertexSize(), usage);
			}

			VertexDeclaration* D3D9ObjectFactory::CreateVertexDeclaration(const FastList<VertexElement>& elements)
			{
				return new D3D9VertexDeclaration(m_device, elements);
			}

			VertexShader* D3D9ObjectFactory::CreateVertexShader(const ResourceLocation* resLoc)
			{
				return new D3D9VertexShader(m_device, resLoc);
			}
			PixelShader* D3D9ObjectFactory::CreatePixelShader(const ResourceLocation* resLoc)
			{
				return new D3D9PixelShader(m_device, resLoc);
			}
		}
	}
}