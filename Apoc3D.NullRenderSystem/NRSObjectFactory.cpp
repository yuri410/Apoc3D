
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

#include "NRSObjectFactory.h"
#include "NRSRenderDevice.h"
#include "NRSTexture.h"
#include "NRSObjects.h"
#include "NRSShader.h"
#include "NRSRenderTarget.h"
#include "NRSSprite.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSObjectFactory::NRSObjectFactory(NRSRenderDevice* device)
				: ObjectFactory(device), m_device(device)
			{

			}
			Texture* NRSObjectFactory::CreateTexture(const ResourceLocation& rl, TextureUsage usage, bool managed)
			{
				return new NRSTexture(m_device, rl, usage, managed);
			}
			Texture* NRSObjectFactory::CreateTexture(int32 width, int32 height, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new NRSTexture(m_device, width, height, 1, levelCount, format, usage);
			}
			Texture* NRSObjectFactory::CreateTexture(int32 width, int32 height, int32 depth, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new NRSTexture(m_device, width, height, depth, levelCount, format, usage);
			}
			Texture* NRSObjectFactory::CreateTexture(int32 length, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return new NRSTexture(m_device, length, levelCount, format, usage);
			}


			RenderTarget* NRSObjectFactory::CreateRenderTarget(int32 width, int32 height, PixelFormat clrFmt, const String& multisampleMode)
			{
				return new NRSRenderTarget(m_device, width, height, clrFmt, multisampleMode);
			}
			DepthStencilBuffer* NRSObjectFactory::CreateDepthStencilBuffer(int32 width, int32 height, DepthFormat depFmt, const String& multisampleMode)
			{
				return new NRSDepthBuffer(m_device, width, height, depFmt, multisampleMode);
			}
			CubemapRenderTarget* NRSObjectFactory::CreateCubemapRenderTarget(int32 length, PixelFormat clrFmt)
			{
				return new NRSCubemapRenderTarget(m_device, length, clrFmt);
			}


			IndexBuffer* NRSObjectFactory::CreateIndexBuffer(IndexBufferFormat type, int32 count, BufferUsageFlags usage)
			{
				return new NRSIndexBuffer(m_device, type, count * (type == IndexBufferFormat::Bit16 ? sizeof(ushort) : sizeof(uint)), usage);
			}
			VertexBuffer* NRSObjectFactory::CreateVertexBuffer(int32 vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage)
			{
				return new NRSVertexBuffer(m_device, vertexCount, vtxDecl->GetVertexSize(), usage);
			}

			VertexDeclaration* NRSObjectFactory::CreateVertexDeclaration(const List<VertexElement>& elements)
			{
				return new NRSVertexDeclaration(m_device, elements);
			}

			Shader* NRSObjectFactory::CreateVertexShader(const ResourceLocation& resLoc)
			{
				return new NRSVertexShader(m_device, resLoc);
			}
			Shader* NRSObjectFactory::CreatePixelShader(const ResourceLocation& resLoc)
			{
				return new NRSPixelShader(m_device, resLoc);
			}

			Shader* NRSObjectFactory::CreateVertexShader(const byte* byteCode)
			{
				return new NRSVertexShader(m_device, byteCode);
			}
			Shader* NRSObjectFactory::CreatePixelShader(const byte* byteCode)
			{
				return new NRSPixelShader(m_device, byteCode);
			}

			Sprite* NRSObjectFactory::CreateSprite()
			{
				return new NRSSprite(m_device);
			}
		}
	}
}