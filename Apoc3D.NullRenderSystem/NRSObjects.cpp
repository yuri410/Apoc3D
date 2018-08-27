
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NRSObjects.h"
#include "Apoc3D/Math/MathCommon.h"
#include "NRSDeviceContext.h"
#include "NRSTexture.h"
#include "NRSRenderTarget.h"
#include "NRSSprite.h"
#include "NRSShader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSObjectFactory::NRSObjectFactory(NRSRenderDevice* device)
				: ObjectFactory(device), m_device(device) { }

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

			Shader* NRSObjectFactory::CreateVertexShader(const ResourceLocation& resLoc) { return new NRSVertexShader(m_device, resLoc); }
			Shader* NRSObjectFactory::CreatePixelShader(const ResourceLocation& resLoc)  { return new NRSPixelShader(m_device, resLoc); }

			Shader* NRSObjectFactory::CreateVertexShader(const byte* byteCode) { return new NRSVertexShader(m_device, byteCode); }
			Shader* NRSObjectFactory::CreatePixelShader(const byte* byteCode)  { return new NRSPixelShader(m_device, byteCode); }

			Sprite* NRSObjectFactory::CreateSprite() { return new NRSSprite(m_device); }

			//////////////////////////////////////////////////////////////////////////

			NRSIndexBuffer::NRSIndexBuffer(NRSRenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage)
				: IndexBuffer(type, size, usage), m_device(device)
			{
				int32 sizeInBytes = size * (type == IndexBufferFormat::Bit16 ? sizeof(uint16) : sizeof(uint32));
				m_data = new char[sizeInBytes]();
			}

			NRSIndexBuffer::~NRSIndexBuffer()
			{
				if (m_data)
				{
					delete[] m_data;
					m_data = nullptr;
				}
			}

			void* NRSIndexBuffer::lock(int offset, int size, LockMode mode)
			{
				return m_data + offset;
			}

			void NRSIndexBuffer::unlock() { }

			//////////////////////////////////////////////////////////////////////////

			NRSVertexBuffer::NRSVertexBuffer(NRSRenderDevice* device, int32 vertexCount, int32 vertexSize, BufferUsageFlags usage)
				: VertexBuffer(vertexCount, vertexSize, usage), m_device(device)
			{
				m_data = new char[vertexCount * vertexSize]();
			}

			NRSVertexBuffer::~NRSVertexBuffer()
			{
				if (m_data)
				{
					delete[] m_data;
					m_data = nullptr;
				}
			}

			void* NRSVertexBuffer::lock(int offset, int size, LockMode mode)
			{
				return m_data + offset;
			}

			void NRSVertexBuffer::unlock() { }

			//////////////////////////////////////////////////////////////////////////

			NRSDepthBuffer::NRSDepthBuffer(NRSRenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const String& multisampleMode)
				: DepthStencilBuffer(device, width, height, depthFormat, multisampleMode) { }

			NRSDepthBuffer::NRSDepthBuffer(NRSRenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const NRSCapabilities::AAProfile* aamode)
				: DepthStencilBuffer(device, width, height, depthFormat, aamode->Name), m_explicitAAMode(aamode) { }

			NRSDepthBuffer::~NRSDepthBuffer()
			{ }

			//////////////////////////////////////////////////////////////////////////

			NRSVertexDeclaration::NRSVertexDeclaration(NRSRenderDevice* device, const List<VertexElement>& elements)
				: VertexDeclaration(elements), m_device(device) { }

			NRSVertexDeclaration::~NRSVertexDeclaration()
			{ }

			//////////////////////////////////////////////////////////////////////////

			NRSInstancingData::NRSInstancingData(NRSRenderDevice* device)
				: InstancingData(device) { }

			NRSInstancingData::~NRSInstancingData()
			{ }

			int NRSInstancingData::Setup(const RenderOperation* op, int count, int beginIndex)
			{
				return Math::Min(MaxOneTimeInstances, count - beginIndex);
			}

			//////////////////////////////////////////////////////////////////////////

			APIDescription NRSGraphicsAPIFactory::GetDescription()
			{
				APIDescription desc;
				desc.Name = L"Null";
				desc.SupportedPlatforms.Add(PlatformAPISupport{ 100, L"WINDOWS" });
				desc.SupportedPlatforms.Add(PlatformAPISupport{ 100, L"LINUX" });
				return desc;
			}

			DeviceContext* NRSGraphicsAPIFactory::CreateDeviceContext()
			{
				return new NRSDeviceContext();
			}
		}
	}
}